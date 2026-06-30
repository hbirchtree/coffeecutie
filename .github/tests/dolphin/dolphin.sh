#!/bin/bash

set -e

DOLPHIN_DIR=$(realpath $(dirname $0)/../../../multi_build/runtime/dolphin)

TIMEOUT=${TIMEOUT:-30}
DUMP_FILE=${DUMP_FILE:-dump_$RANDOM.mp4}
echo "::info::Virtual GameCube will run for ${TIMEOUT} seconds"
echo "::info::Recording will be saved to \"${DUMP_FILE}\""

REQUIRED_SHM_MB=128
SHM_MB=$(df -m --output=size /dev/shm 2>/dev/null | tail -1 | tr -dc '0-9')
if [ -z "${SHM_MB}" ]; then
    echo "::warning::Could not determine /dev/shm size; Dolphin may fail to start"
elif [ "${SHM_MB}" -lt "${REQUIRED_SHM_MB}" ]; then
    echo "::error::/dev/shm is only ${SHM_MB} MiB, but Dolphin needs >= ${REQUIRED_SHM_MB} MiB for its memory arena (otherwise it dies with SIGBUS during memory init)."
    echo "::error::Increase it via the container runtime, e.g.: docker run --shm-size=1g | compose 'shm_size: 1gb' | devcontainer runArgs '--shm-size=1g' | GitHub Actions container 'options: --shm-size=1g'."
    exit 1
fi

RUN_PREFIX=()
if [ "${DOLPHIN_XVFB:-1}" = "1" ]; then
    if ! command -v xvfb-run >/dev/null 2>&1; then
        echo "::error::xvfb-run not found, but Dolphin needs an X display to find the GPU. Install it (e.g. apt-get install -y xvfb) or set DOLPHIN_XVFB=0 for a software backend."
        exit 1
    fi
    RUN_PREFIX=(xvfb-run -a -s "-screen 0 640x480x24")
fi

echo "::group::Dolphin run"
DEFAULT_ISO_ARG=()
if [ -n "${DVD_DIR:-}" ]; then
    DEFAULT_ISO_ARG=(-C "Dolphin.Core.DefaultISO=${DVD_DIR}")
fi

# Capture Dolphin's stdout/stderr so the app's OSReport output (cDebug ->
# SYS_STDIO_Report) is visible after the run instead of being lost to setsid.
DOLPHIN_LOG=${DOLPHIN_LOG:-/tmp/dolphin_run_$$.log}
: > "${DOLPHIN_LOG}"

# Enable dumping A/V
# Enable memcard on slot A
# Enable USB Gecko on slot B
# Enable writing to console
# Enable bunch of log categories
setsid "${RUN_PREFIX[@]}" \
    ${DOLPHIN_DIR}/bin/dolphin-emu-nogui \
    -p headless \
    -u ${DOLPHIN_DIR}/config/User \
    -C Dolphin.Movie.DumpFrames=True \
    -C Dolphin.DSP.DumpAudio=True \
    -C Dolphin.Core.SlotA=8 \
    -C Dolphin.Core.SlotB=7 \
    -C Dolphin.Core.GFXBackend="${DOLPHIN_RENDERER:-Vulkan}" \
    -C GFX.Settings.DumpCodec=utvideo \
    -C Logger.Options.WriteToConsole=True \
    -C Logger.Options.WriteToFile=True \
    -C Logger.Logs.Core=True \
    -C Logger.Logs.EXI=True \
    -C Logger.Logs.PowerPC=True \
    -C Logger.Logs.OSREPORT_HLE=True \
    -C Logger.Logs.OSREPORT=True \
    "${DEFAULT_ISO_ARG[@]}" \
    "${@}" > "${DOLPHIN_LOG}" 2>&1 &
DOLPHIN_PGID=$!
trap 'kill -KILL -"${DOLPHIN_PGID}" 2>/dev/null || true' EXIT

sleep ${TIMEOUT:-30}

kill -TERM -"${DOLPHIN_PGID}" 2>/dev/null || true
sleep 2
kill -KILL -"${DOLPHIN_PGID}" 2>/dev/null || true
wait "${DOLPHIN_PGID}" 2>/dev/null || true
echo "::endgroup::"

echo "::group::Dolphin OSReport output"
# Surface the emulated app's own logging (and any panics) from the run.
grep -aE 'OSREPORT|PanicAlert|Exception|terminate|>>>|bsp:' "${DOLPHIN_LOG}" || \
    tail -40 "${DOLPHIN_LOG}"
echo "::endgroup::"

AUDIO_DUMP=$(find ${DOLPHIN_DIR}/config/User/Dump/Audio -name *_dspdump.wav)
VIDEO_DUMP=$(find ${DOLPHIN_DIR}/config/User/Dump/Frames -name *.avi)
echo "::group::Encode dumped frames to file"
ffmpeg \
    -i ${VIDEO_DUMP} \
    -i ${AUDIO_DUMP} \
    -c:v libx264 -b:v 4M \
    -c:a aac -b:a 128k \
    "${DUMP_FILE}"
rm -r ${DOLPHIN_DIR}/config/User/Dump/{Audio,Frames}
echo "::endgroup::"
