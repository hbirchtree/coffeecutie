#!/usr/bin/env bash
# Offline shader statistics from Mesa's real compilers, no GPU required.
# Mesa's drm-shim fakes the kernel driver, so freedreno/lima/radeonsi (and
# panfrost/iris with the clc build) compile on this host and report stats.
#
#   ./shader-stats.sh setup [base|clc]           build Mesa (base: freedreno, lima, radeonsi;
#                                                clc: panfrost, iris, needs LLVM 19)
#   ./shader-stats.sh targets                    list GPU targets
#   ./shader-stats.sh run <target> <shader|dir>...   compile shaders, print stats table
#   ./shader-stats.sh raw <target> <shader|dir>...   same, unparsed driver output
#
# A directory means every .vert/.frag/.comp in it; use a shell glob to pick
# variants, eg. assets/shaders/*.es320.*
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cache="${SHADER_STATS_CACHE:-$HOME/.cache/coffee-shader-stats}"
mesa_ref="${MESA_REF:-25.2}"
jobs="${JOBS:-16}"

export PYTHONPATH="$cache/pydeps${PYTHONPATH:+:$PYTHONPATH}"
meson() { python3 -m mesonbuild.mesonmain "$@"; }

# target -> "<build> <shim> <env...>"
declare -A targets=(
    [a643]="base freedreno FD_CHIP_ID=06030500 FD_MESA_DEBUG=shaderdb"
    [a702]="base freedreno FD_CHIP_ID=07002000 FD_MESA_DEBUG=shaderdb"
    [a618]="base freedreno FD_GPU_ID=618 FD_MESA_DEBUG=shaderdb"
    [a630]="base freedreno FD_GPU_ID=630 FD_MESA_DEBUG=shaderdb"
    [a660]="base freedreno FD_GPU_ID=660 FD_MESA_DEBUG=shaderdb"
    [a740]="base freedreno FD_GPU_ID=740 FD_MESA_DEBUG=shaderdb"
    [mali400]="base lima LIMA_DEBUG=shaderdb"
    [navi21]="base amdgpu AMDGPU_GPU_ID=navi21"
    [navi31]="base amdgpu AMDGPU_GPU_ID=navi31"
    [vangogh]="base amdgpu AMDGPU_GPU_ID=vangogh"
    [g610]="clc panfrost PAN_GPU_ID=a807 MESA_GLES_VERSION_OVERRIDE=3.2 BIFROST_MESA_DEBUG=shaderdb"
    [g57]="clc panfrost PAN_GPU_ID=9003 MESA_GLES_VERSION_OVERRIDE=3.2 BIFROST_MESA_DEBUG=shaderdb"
    [g52]="clc panfrost PAN_GPU_ID=7212 MESA_GLES_VERSION_OVERRIDE=3.2 BIFROST_MESA_DEBUG=shaderdb"
    [tgl]="clc intel INTEL_STUB_GPU_PLATFORM=tgl"
    [rpl]="clc intel INTEL_STUB_GPU_PLATFORM=rpl"
    [mtl]="clc intel INTEL_STUB_GPU_PLATFORM=mtl"
    [dg2]="clc intel INTEL_STUB_GPU_PLATFORM=dg2"
)

die() { echo "error: $*" >&2; exit 1; }

fetch_mesa() {
    [ -d "$cache/mesa" ] ||
        git clone -q --depth 1 --branch "$mesa_ref" \
            https://gitlab.freedesktop.org/mesa/mesa.git "$cache/mesa"
    for p in "$here"/patches/mesa-*.patch; do
        git -C "$cache/mesa" apply --reverse --check "$p" 2>/dev/null ||
            git -C "$cache/mesa" apply "$p"
    done
    python3 -c 'import mesonbuild, mako, yaml' 2>/dev/null ||
        pip install -q --target "$cache/pydeps" meson mako pyyaml packaging
}

# Unpack Ubuntu dev packages into a private sysroot; no root needed.
fetch_debs() {
    local sysroot="$cache/sysroot" debs="$cache/debs"
    mkdir -p "$sysroot" "$debs"
    (cd "$debs" && apt-get download -q "$@" >/dev/null)
    for d in "$debs"/*.deb; do dpkg -x "$d" "$sysroot"; done
    find "$sysroot" -name '*.pc' -exec sed -i "s|=/usr\b|=$sysroot/usr|" {} +
    export PKG_CONFIG_PATH="$sysroot/usr/lib/x86_64-linux-gnu/pkgconfig:$sysroot/usr/lib/pkgconfig:$sysroot/usr/share/pkgconfig"
    # some Ubuntu .pc files omit the multiarch libdir, relying on the default search path
    export LIBRARY_PATH="$sysroot/usr/lib/x86_64-linux-gnu"
    # build-time tools (mesa_clc) run against the sysroot libraries
    export LD_LIBRARY_PATH="$sysroot/usr/lib/x86_64-linux-gnu:$sysroot/usr/lib/llvm-19/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
}

setup() {
    local set="${1:-base}" drivers extra=()
    case "$set" in
    base) drivers=freedreno,lima,radeonsi; extra=(-Dllvm=disabled) ;;
    clc)  drivers=panfrost,iris
          extra=(-Dllvm=enabled -Dshared-llvm=enabled -Ddraw-use-llvm=false
                 --native-file "$cache/llvm.ini") ;;
    *)    die "unknown build set '$set'" ;;
    esac
    fetch_debs libdrm-dev libdrm2 libdrm-amdgpu1 libdrm-common libelf-dev libelf1t64 libzstd-dev
    # iris and panfrost precompile OpenCL kernels at build time (mesa_clc)
    if [ "$set" = clc ]; then
        fetch_debs llvm-19 llvm-19-dev libllvm19 libclang-19-dev libclang-cpp19 \
            libclang-cpp19-dev libclang1-19 libclang-common-19-dev libclc-19 \
            libclc-19-dev libllvmspirvlib19.1 libllvmspirvlib-19-dev spirv-tools
        printf "[binaries]\nllvm-config = '%s'\n" \
            "$cache/sysroot/usr/lib/llvm-19/bin/llvm-config" >"$cache/llvm.ini"
    fi
    fetch_mesa
    local build="$cache/build-$set"
    [ -f "$build/build.ninja" ] ||
        meson setup "$build" "$cache/mesa" \
            --prefix="$cache/prefix-$set" --libdir=lib --buildtype=release --wrap-mode=nofallback \
            -Dgallium-drivers="$drivers" -Dvulkan-drivers= -Dplatforms= \
            -Dglx=disabled -Dglvnd=disabled -Degl=enabled -Dgbm=disabled -Dgles1=disabled \
            -Dgles2=enabled -Dtools=drm-shim -Dvalgrind=disabled \
            -Dlibunwind=disabled -Dzstd=disabled -Dbuild-tests=false \
            "${extra[@]}"
    ninja -C "$build" -j"$jobs"
    meson install -C "$build" --quiet
    c++ -std=c++20 -O1 -I"$cache/mesa/include" -o "$cache/runner-$set" \
        "$here/runner.cpp" -L"$cache/prefix-$set/lib" -lEGL \
        -Wl,-rpath,"$cache/prefix-$set/lib"
    echo "built $set into $cache/prefix-$set"
}

shim_lib() {
    local prefix="$1" shim="$2" lib
    lib=$(find "$cache/build-${prefix##*-}" -name "lib${shim}_noop_drm_shim.so" | head -1)
    [ -n "$lib" ] || die "no $shim drm-shim built"
    echo "$lib"
}

run_raw() {
    local target="$1"
    shift
    [ -n "${targets[$target]:-}" ] || die "unknown target '$target' (see: targets)"
    read -r set shim envs <<<"${targets[$target]}"
    local prefix="$cache/prefix-$set"
    [ -x "$cache/runner-$set" ] || die "run '$0 setup $set' first"
    local files=()
    for src in "$@"; do
        if [ -d "$src" ]; then
            while IFS= read -r f; do files+=("$f"); done < <(
                find "$src" -maxdepth 1 -type f \
                    \( -name '*.vert' -o -name '*.frag' -o -name '*.comp' \) | sort)
        elif [ -f "$src" ]; then
            files+=("$src")
        else
            die "no such shader '$src'"
        fi
    done
    [ ${#files[@]} -gt 0 ] || die "no shaders given"
    # threaded context drops synchronous debug callbacks; cache would skip compiles
    # shellcheck disable=SC2086
    env $envs GALLIUM_THREAD=0 MESA_SHADER_CACHE_DISABLE=true \
        LD_PRELOAD="$(shim_lib "$prefix" "$shim")" \
        LD_LIBRARY_PATH="$prefix/lib:$cache/sysroot/usr/lib/x86_64-linux-gnu:$cache/sysroot/usr/lib/llvm-19/lib" \
        "$cache/runner-$set" "${files[@]}"
}

# Driver stat lines ("<stage> shader: <value> <name>, ...") -> one table row per shader.
# Tagged lines come from the GL debug callback; untagged ones (panfrost) from
# stderr, attributed to the preceding "### <shader>" marker.
table() {
    awk -F'\t' -v shim="$1" '
        function row(name, line,    stage, n, i, x, p, key, kv) {
            if(seen[name, line]++) return
            stage = line; sub(/ shader:.*/, "", stage); sub(/^MESA_SHADER_/, "", stage)
            simd = ""
            if(match(stage, / SIMD[0-9]+/)) { simd = substr(stage, RSTART + 5, RLENGTH - 5); stage = substr(stage, 1, RSTART - 1) }
            sub(/^.* shader: /, "", line); sub(/\.?\n?$/, "", line)
            n = split(line, kv, ", ")
            delete v
            for(i = 1; i <= n; i++) {
                split(kv[i], p, " ")
                if(p[1] !~ /^[0-9.:]+$/) continue
                key = p[2]; for(j = 3; j in p; j++) key = key " " p[j]
                v[key] = p[1]
            }
            if(shim == "freedreno") {
                if(!hdr++) printf "%-40s %-8s %6s %6s %5s %5s %5s %5s %5s %5s %5s\n",
                    "shader", "stage", "inst", "nops", "full", "half", "waves", "tex", "ss", "sy", "spill"
                printf "%-40s %-8s %6s %6s %5s %5s %5s %5s %5s %5s %5s\n", name, stage,
                    v["inst"], v["nops"], v["full"], v["half"], v["waves"], v["cat5"],
                    v["(ss)"], v["(sy)"], v["stp"] + v["ldp"]
            } else if(shim == "panfrost") {
                if(!hdr++) printf "%-40s %-8s %6s %7s %6s %6s %6s %6s %6s %6s %7s %11s\n",
                    "shader", "stage", "instrs", "cycles", "fma", "cvt", "sfu", "var", "tex", "ls", "threads", "spill:fill"
                printf "%-40s %-8s %6s %7.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %7s %11s\n", name, stage,
                    v["instrs"], v["cycles"], v["fma"], v["cvt"], v["sfu"], v["v"], v["t"], v["ls"],
                    v["threads"], v["spills:fills"]
            } else if(shim == "intel") {
                if(!hdr++) printf "%-40s %-8s %5s %6s %7s %6s %6s %11s\n",
                    "shader", "stage", "simd", "inst", "cycles", "loops", "sends", "spill:fill"
                printf "%-40s %-8s %5s %6s %7s %6s %6s %11s\n", name, stage, simd,
                    v["inst"], v["cycles"], v["loops"], v["sends"], v["spills:fills"]
            } else {
                print name "\t" stage " shader: " line
            }
        }
        /^renderer: / { print; next }
        /^### / { marker = substr($0, 5); next }
        NF == 1 && / shader: [0-9]+ inst/ { row(marker, $0); next }
        $2 ~ /ERROR/ { printf "%-40s %s\n", $1, $3; next }
        $2 ~ /warning:/ { warn[$1]++; next }
        $2 ~ /error:/ { next }
        $2 ~ / shader: [0-9]+ inst/ { row($1, $2); next }
        NF > 1 { print }
        END { for(f in warn) printf "%s: %d GLSL warnings (see raw)\n", f, warn[f] }'
}

case "${1:-}" in
setup)   setup "${2:-base}" ;;
targets) for t in "${!targets[@]}"; do echo "$t: ${targets[$t]}"; done | sort ;;
raw)     shift; run_raw "$@" ;;
run)     shift
         [ -n "${targets[${1:-}]:-}" ] || die "unknown target '${1:-}' (see: targets)"
         read -r _ shim _ <<<"${targets[$1]}"
         run_raw "$@" 2>&1 | table "$shim" ;;
*)       awk 'NR > 1 && !/^#/ { exit } NR > 1 { sub(/^# ?/, ""); print }' "$0"; exit 1 ;;
esac
