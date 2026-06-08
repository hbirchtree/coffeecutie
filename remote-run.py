#!/usr/bin/env python3
"""Run a built binary on a remote device using .targets.json configuration.

Example .targets.json:

    {
        "devices": {
            "myboard": {
                "target": "desktop:aarch64-buildroot-linux-gnu:multi",
                "hostname": "myboard.local",
                "scratchdir": "/home/user/scratch",
                "env": {
                    "DISPLAY": ":0"
                },
                "viewer": {
                    "type": "web",
                    "address": "http://myboard.local:8888"
                }
            },
            "myphone": {
                "target": "android:arm64:32",
                "hostname": "192.168.1.10:5555",
                "env": {},
                "viewer": {
                    "type": "scrcpy"
                }
            },
            "docker": {
                "target": "desktop:x86_64-buildroot-linux-gnu:multi",
                "type": "docker",
                "image": "ubuntu:24.04",
                "memory": "4g",
                "cpus": "4",
                "dri_device": "/dev/dri/renderD128",
                "mounts": [
                    {"local": "/mnt/blam", "remote": "/mnt/blam", "readonly": true},
                    {"local": "/tmp/Blam Graphics", "remote": "/tmp/Blam Graphics"}
                ],
                "env": {
                    "LIBGL_ALWAYS_SOFTWARE": "0"
                }
            }
        },
        "presets": {
            "MyApp": {
                "binary": "MyApp",
                "package": "com.example.MyApp",
                "workdir": "$SCRATCH_DIR",
                "files": [
                    {"local": "$BUILD_DIR/assets/", "remote": "$SCRATCH_DIR/assets/"}
                ],
                "env": {},
                "args": ["$SCRATCH_DIR/assets"]
            }
        }
    }

Keys:
  devices[name].target     — platform:arch:sysroot (determines build dir under multi_build/)
  devices[name].type       — "linux" (ssh, default), "android" (adb), or "docker"
  devices[name].hostname   — SSH host or ADB serial; omit for android to auto-pick via adb
  devices[name].scratchdir — working directory data is staged into (Linux ssh
                             and docker; docker defaults to <build_dir>/.docker_scratch)
  devices[name].env        — environment variables set on the remote
  devices[name].viewer     — optional display viewer: {type: "web"|"scrcpy", address: "..."}

  Docker-only device keys (type == "docker"):
  devices[name].image      — docker image to run the binary in (required)
  devices[name].memory     — max memory, e.g. "4g" (--memory)
  devices[name].cpus       — max CPU cores, e.g. "4" or "1.5" (--cpus)
  devices[name].dri_device — DRI device(s) for GPU access; string or list,
                             default "/dev/dri" (--device, render/video gid auto-added)
  devices[name].groups     — extra supplementary group ids/names (--group-add)
  devices[name].user       — container user, default current "uid:gid"; set null to skip
  devices[name].mounts     — list of {local, remote, readonly?} host bind mounts.
                             The repo root and scratch dir are always mounted at
                             their own paths so $BUILD_DIR/$SRC_DIR/$SCRATCH_DIR
                             resolve identically inside. `files` are staged into
                             the scratch dir host-side, so the standard executable
                             presets work on a docker device unchanged. Set a
                             headless DUMMY_PLUG_CONFIG in env for graphics apps.
  presets[name].binary     — executable name under multi_build/<target>/bin/
  presets[name].package    — Android package name (required for android targets)
  presets[name].workdir    — remote cwd, supports $SCRATCH_DIR, $BUILD_DIR, and $SRC_DIR
  presets[name].files      — list of {local, remote} rsync transfers before launch
  presets[name].args       — command-line args passed to the binary
  presets[name].extras     — Android --es intent extras (merged with device env)

Flags:
  --log FILE           — tee all received output (setup + program) to FILE
  --collect-profile DIR — after the run, copy profile.json and *-chrome.json
                          into DIR (docker: from bind-mount/scratch dirs;
                          ssh: scp'd from the remote /tmp; android: pulled from
                          app-private storage via `adb run-as`, debug builds only)
"""

import argparse
import concurrent.futures
import fnmatch
import glob
import json
import os
import shlex
import shutil
import signal
import subprocess
import sys
import time

from rich.text import Text as RichText
from textual import work
from textual.app import App, ComposeResult
from textual.widgets import Markdown, OptionList, RichLog, Rule, Static
from textual.widgets.option_list import Option


def load_config(script_dir):
    config_path = os.path.join(script_dir, '.targets.json')
    with open(config_path) as f:
        return json.load(f)


def target_to_dir(target):
    """Convert 'platform:arch:sysroot' to 'platform-arch-sysroot'."""
    return target.replace(':', '-')


def expand_vars(value, scratchdir, build_dir, src_dir=None):
    """Expand $SCRATCH_DIR, $BUILD_DIR, and $SRC_DIR in a string."""
    result = value.replace('$SCRATCH_DIR', scratchdir).replace('$BUILD_DIR', build_dir)
    if src_dir is not None:
        result = result.replace('$SRC_DIR', src_dir)
    return result


def toptext_message(device, hostname=None):
    hostname = hostname or device.get('hostname', '<unknown>')
    def _viewer():
        viewer = device.get('viewer', {})
        kind = viewer.get('type', '')
        address = viewer.get('address', '') or hostname
        if kind == 'web':
            return f"View the display at **{address}**"
        elif kind == 'scrcpy':
            return f"View the display by pointing **scrcpy** at **{address}**"
        elif kind:
            return f"View the display ({kind}) at **{address}**"
        return ""
    target = device.get('target', '<unknown>')
    return f"""Deployed **{target}** to **{hostname}**

{_viewer()}"""


def parse_adb_devices():
    """Return list of (identifier, model, device_name) from `adb devices -l`."""
    result = subprocess.run(['adb', 'devices', '-l'], capture_output=True, text=True)
    if result.returncode != 0:
        sys.exit("Error: adb devices failed — is adb installed and on PATH?")
    devices = []
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) < 2 or parts[1] != 'device':
            continue
        identifier = parts[0]
        attrs = {}
        for part in parts[2:]:
            if ':' in part:
                k, _, v = part.partition(':')
                attrs[k] = v
        model = attrs.get('model', identifier).replace('_', ' ')
        device_name = attrs.get('device', '')
        devices.append((identifier, model, device_name))
    return devices


class DevicePickerApp(App):
    CSS = """
    Markdown { height: auto; margin: 0 1; }
    Rule { height: 1; margin: 0; }
    OptionList { height: 1fr; }
    """
    BINDINGS = [("ctrl+c", "quit", "Cancel")]

    def __init__(self, devices):
        super().__init__()
        self.devices = devices
        self.selected = None

    def compose(self) -> ComposeResult:
        yield Markdown("**Select an ADB device:**", open_links=False)
        yield Rule()
        yield OptionList(*[
            Option(f"{model}  ({device_name})  —  {identifier}", id=identifier)
            for identifier, model, device_name in self.devices
        ])

    def on_option_list_option_selected(self, event: OptionList.OptionSelected) -> None:
        self.selected = str(event.option.id)
        self.exit()

    def action_quit(self) -> None:
        self.exit()


def pick_adb_device():
    devices = parse_adb_devices()
    if not devices:
        sys.exit("No ADB devices found. Connect a device or set 'hostname' in .targets.json.")
    if len(devices) == 1:
        return devices[0][0]
    app = DevicePickerApp(devices)
    app.run()
    if not app.selected:
        sys.exit("No device selected.")
    return app.selected


class OutputViewerApp(App):
    CSS = """
    Static {
        height: 1;
        background: $panel-darken-1;
        padding: 0 1;
    }
    Rule {
        height: 1;
        margin: 0;
    }
    #close-prompt {
        display: none;
        text-align: center;
    }
    """
    BINDINGS = [("ctrl+c", "quit", "Quit"), ("enter", "close", "Close")]

    def __init__(self, cmd, toptext_msg, setup_cmds=None, on_quit=None, log_file=None, env=None):
        super().__init__()
        self.cmd = cmd          # list[str] or callable returning list[str]
        self.toptext_msg = toptext_msg
        self.setup_cmds = setup_cmds or []
        self._on_quit = on_quit
        self._log_file = log_file
        self._env = env
        self._log_fh = None
        self._proc = None
        self._quitting = False
        self._on_quit_called = False
        self._stream_done = False

    def _log(self, text) -> None:
        if self._log_fh:
            self._log_fh.write(str(text) + "\n")
            self._log_fh.flush()

    def compose(self) -> ComposeResult:
        if self.toptext_msg:
            yield Markdown(self.toptext_msg, open_links=False)
            yield Rule()
        yield RichLog(highlight=False, markup=False)
        yield Static("Press Enter to close", id="close-prompt")

    def on_mount(self) -> None:
        self._stream()

    def _run_setup(self, log, cmd) -> bool:
        """Run a setup command, writing heading/output/exit code. Returns True on success."""
        heading = "$ " + " ".join(str(c) for c in cmd)
        self.call_from_thread(log.write, "")
        self.call_from_thread(log.write, RichText(heading, style="bold"))
        self._log(heading)
        self._proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        for line in self._proc.stdout:
            stripped = line.rstrip()
            def write_line(l=stripped):
                log.auto_scroll = log.is_vertical_scroll_end
                log.write(l)
            self.call_from_thread(write_line)
            self._log(stripped)
        self._proc.wait()
        code = self._proc.returncode
        self.call_from_thread(log.write, RichText(f"exit {code}", style="green" if code == 0 else "bold red"))
        self._log(f"exit {code}")
        return code == 0

    @work(thread=True)
    def _stream(self) -> None:
        log = self.query_one(RichLog)
        if self._log_file:
            self._log_fh = open(self._log_file, "w")

        for cmd in self.setup_cmds:
            if not self._run_setup(log, cmd):
                break
        else:
            actual_cmd = self.cmd() if callable(self.cmd) else self.cmd
            heading = "$ " + " ".join(str(c) for c in actual_cmd)
            self.call_from_thread(log.write, "")
            self.call_from_thread(log.write, RichText(heading, style="bold"))
            self._log(heading)
            
            merged_env = {**os.environ, **(self._env or {})}
            self._proc = subprocess.Popen(
                actual_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, env=merged_env
            )
            for line in self._proc.stdout:
                stripped = line.rstrip()
                def write_line(l=stripped):
                    log.auto_scroll = log.is_vertical_scroll_end
                    log.write(l)
                self.call_from_thread(write_line)
                self._log(stripped)
            self._proc.wait()
            code = self._proc.returncode
            self.call_from_thread(log.write, RichText(f"exit {code}", style="green" if code == 0 else "bold red"))
            self._log(f"exit {code}")

        if self._log_fh:
            self._log_fh.close()
            self._log_fh = None
        self._do_cleanup()
        self._stream_done = True
        self.call_from_thread(self._show_close_prompt)

    def _do_cleanup(self) -> None:
        if not self._on_quit_called:
            self._on_quit_called = True
            if self._on_quit:
                self._on_quit()

    def _show_close_prompt(self) -> None:
        self.query_one("#close-prompt").display = True

    def action_close(self) -> None:
        if self._stream_done:
            self.exit()

    def action_quit(self) -> None:
        if self._proc and self._proc.poll() is None:
            if not self._quitting:
                self._quitting = True
                self._proc.send_signal(signal.SIGINT)
                # _stream will drain remaining output, show exit code, then prompt
            else:
                # Second Ctrl-C: force terminate and exit immediately
                self._proc.terminate()
                self._do_cleanup()
                self.exit()
        else:
            self._do_cleanup()
            self.exit()


def run_plain(cmd, toptext_msg=None, setup_cmds=None, on_quit=None, log_file=None, env=None):
    """Non-TUI fallback used when stdout is not a terminal: stream the setup
    commands and the main command as plain text (pipe/redirect friendly).
    Returns the exit code of the main command (or the first failed setup)."""
    log_fh = open(log_file, 'w') if log_file else None
    merged_env = {**os.environ, **(env or {})} if env else None
    rc = 0
    try:
        if toptext_msg:
            print(toptext_msg)
        ok = True
        for c in (setup_cmds or []):
            heading = '$ ' + ' '.join(str(x) for x in c)
            print(heading)
            if log_fh:
                log_fh.write(heading + '\n')
            code = _tee_run(c, log_fh)
            if log_fh:
                log_fh.write(f'exit {code}\n')
            if code != 0:
                rc, ok = code, False
                break
        if ok:
            actual = cmd() if callable(cmd) else cmd
            heading = '$ ' + ' '.join(str(x) for x in actual)
            print(heading)
            if log_fh:
                log_fh.write(heading + '\n')
            rc = _tee_run(actual, log_fh, env=merged_env)
            if log_fh:
                log_fh.write(f'exit {rc}\n')
    finally:
        if on_quit:
            on_quit()
        if log_fh:
            log_fh.close()
    return rc


def view_output(cmd, toptext_msg=None, setup_cmds=None, on_quit=None, log_file=None, env=None):
    """Run a command (with optional setup) showing its output. Uses the textual
    TUI when stdout is an interactive terminal, otherwise falls back to plain
    streaming so output is pipe/redirect friendly."""
    if sys.stdout.isatty():
        OutputViewerApp(
            cmd, toptext_msg, setup_cmds=setup_cmds,
            on_quit=on_quit, log_file=log_file, env=env,
        ).run()
    else:
        run_plain(
            cmd, toptext_msg, setup_cmds=setup_cmds,
            on_quit=on_quit, log_file=log_file, env=env,
        )


def stream_logcat(hostname, package, cmd, device, setup_cmds=None, log_file=None):
    def cleanup():
        subprocess.run(['adb', '-s', hostname, 'shell', 'am', 'force-stop', package])
    view_output(
        cmd,
        toptext_message(device, hostname),
        setup_cmds=setup_cmds,
        on_quit=cleanup,
        log_file=log_file,
    )


def find_linux_binary(build_root, target_dir, binary):
    path = os.path.join(build_root, target_dir, 'bin', binary)
    return path if os.path.isfile(path) else None


def find_android_apk(build_root, target_dir, package):
    apk_dir = os.path.join(build_root, target_dir, 'packaged', 'android-apk')
    matches = glob.glob(os.path.join(apk_dir, f'{package}*.apk'))
    return matches[0] if matches else None


def find_web_bundle(build_root, target_dir):
    """Search for any directory ending in .bundle under build_root/target_dir."""
    search_path = os.path.join(build_root, target_dir, "**", "*.bundle")
    matches = glob.glob(search_path, recursive=True)
    return matches[0] if matches else None


def print_dry_run(cmds):
    for cmd in cmds:
        print('$', ' '.join(shlex.quote(str(c)) for c in cmd))


def _tee_run(cmd, log_fh=None, **kwargs):
    """Run a command, streaming combined stdout/stderr to our stdout and,
    when given, a log file handle. Returns the exit code."""
    proc = subprocess.Popen(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, **kwargs)
    for line in proc.stdout:
        sys.stdout.write(line)
        sys.stdout.flush()
        if log_fh:
            log_fh.write(line)
            log_fh.flush()
    proc.wait()
    return proc.returncode


def collect_profiles(dest, source_dir, cleanup=False):
    """Copy profile.json and *-chrome.json from source_dir into dest.
    If cleanup is True, delete source_dir after copying."""
    patterns = ('profile.json', '*-chrome.json', '*/profile.json', '*/*-chrome.json')
    found = []
    if source_dir and os.path.isdir(source_dir):
        for pat in patterns:
            found.extend(glob.glob(os.path.join(source_dir, pat)))
    
    found = sorted(set(found))
    if not found:
        print(f"collect-profile: no profile.json / *-chrome.json found in: {source_dir}")
    else:
        os.makedirs(dest, exist_ok=True)
        for f in found:
            target = os.path.join(dest, os.path.basename(f))
            shutil.copy2(f, target)
            print(f"collect-profile: {f} -> {target}")

    if cleanup and source_dir and os.path.isdir(source_dir):
        shutil.rmtree(source_dir)


def collect_profiles_android(hostname, package, dest):
    """Pull profile.json / *-chrome.json from an app's private storage via
    `adb run-as` (works only for debuggable / debug-mode builds). The profiler
    writes them under /data/data/<package>/{cache,files}/."""
    os.makedirs(dest, exist_ok=True)
    pulled = 0
    for sub in ('cache', 'files'):
        ls = subprocess.run(
            ['adb', '-s', hostname, 'shell', 'run-as', package, 'ls', sub],
            capture_output=True, text=True)
        if ls.returncode != 0:
            if sub == 'cache':  # report the run-as failure once
                print("collect-profile: `adb run-as` failed — the app must be a "
                      "debug (debuggable) build. " + ls.stderr.strip())
                return
            continue
        for name in (n.strip() for n in ls.stdout.split() if n.strip()):
            if not (fnmatch.fnmatch(name, 'profile*.json')
                    or fnmatch.fnmatch(name, '*-chrome.json')):
                continue
            target = os.path.join(dest, name)
            with open(target, 'wb') as out:
                r = subprocess.run(
                    ['adb', '-s', hostname, 'exec-out', 'run-as', package,
                     'cat', f'{sub}/{name}'],
                    stdout=out)
            if r.returncode == 0:
                print(f"collect-profile: {package}:{sub}/{name} -> {target}")
                pulled += 1
    if pulled == 0:
        print(f"collect-profile: no profile.json / *-chrome.json in {package} private storage")


def run_linux(device_name, device, preset_name, preset, extra_args, script_dir, build_root, target_dir, dry_run=False, log_file=None, collect_profile=None):
    hostname = device['hostname']
    scratchdir = device.get('scratchdir')
    if not scratchdir:
        sys.exit(f"Error: device '{device_name}' is missing required 'scratchdir'")

    binary = preset['binary']
    binary_path = find_linux_binary(build_root, target_dir, binary)
    if not binary_path and not dry_run:
        sys.exit(
            f"Error: binary '{binary}' not found at multi_build/{target_dir}/bin/{binary}\n"
            "Did you run the build first?"
        )
    if not binary_path:
        binary_path = os.path.join(build_root, target_dir, 'bin', binary)

    remote_binary = f"{scratchdir}/{binary}"
    build_dir = os.path.join(build_root, target_dir)

    def ev(s):
        return expand_vars(s, scratchdir, build_dir, script_dir)

    # Merge env: device env overridden by preset env
    merged_env = {k: ev(str(v)) for k, v in {**device.get('env', {}), **preset.get('env', {})}.items()}

    # Unique session ID for collisions
    session_id = f"run-{int(time.time())}"
    remote_tmp = None

    if collect_profile:
        remote_tmp = f"{scratchdir}/{session_id}"
        merged_env['TMPDIR'] = remote_tmp

    # Build remote shell command
    workdir = ev(preset.get('workdir', '$SCRATCH_DIR'))
    all_args = [ev(a) for a in preset.get('args', [])] + extra_args

    env_str = ' '.join(f'{k}={shlex.quote(str(v))}' for k, v in merged_env.items())
    cmd_parts = [shlex.quote(remote_binary)] + [shlex.quote(a) for a in all_args]
    if env_str:
        cmd_parts = [env_str] + cmd_parts
    remote_cmd = f'cd {shlex.quote(workdir)} && {" ".join(cmd_parts)}'

    setup_cmds = [
        ['ssh', hostname, f'mkdir -p {shlex.quote(scratchdir)}'],
    ]
    if remote_tmp:
        setup_cmds.append(['ssh', hostname, f'mkdir -p {shlex.quote(remote_tmp)}'])

    setup_cmds.append(['rsync', '-av', '--checksum', '--chmod=+x', binary_path, f'{hostname}:{remote_binary}'])
    for entry in preset.get('files', []):
        local_abs = ev(entry['local'])
        if not os.path.isabs(local_abs):
            local_abs = os.path.join(script_dir, local_abs)
        remote_path = ev(entry['remote'])
        setup_cmds.append(['rsync', '-av', '--checksum', local_abs, f'{hostname}:{remote_path}'])

    if dry_run:
        print_dry_run(setup_cmds + [['ssh', hostname, '--', remote_cmd]])
        return

    view_output(
        ['ssh', hostname, '--', remote_cmd],
        toptext_message(device, hostname),
        setup_cmds=setup_cmds,
        log_file=log_file,
    )

    if collect_profile:
        # Profiler output lands in the remote session dir; pull
        # any profile.json / *-chrome.json back via scp.
        os.makedirs(collect_profile, exist_ok=True)
        # We search both the session dir and one level deep (for AppName/ subdirs)
        pulled = 0
        for pat in (f'{remote_tmp}/profile.json', f'{remote_tmp}/*-chrome.json',
                    f'{remote_tmp}/*/profile.json', f'{remote_tmp}/*/*-chrome.json'):
            res = subprocess.run(
                ['scp', f'{hostname}:{pat}', collect_profile],
                check=False, stderr=subprocess.DEVNULL
            )
            if res.returncode == 0:
                pulled += 1
        
        if pulled == 0:
            print(f"collect-profile: no profiles found in {hostname}:{remote_tmp}")
        
        # Cleanup remote tmp
        subprocess.run(['ssh', hostname, f'rm -rf {shlex.quote(remote_tmp)}'], check=False)


def _dri_gids(paths):
    """Collect the group ids owning each DRI device node so the container user
    can be added to them (needed for render-node access without running root)."""
    gids = set()
    for p in paths:
        try:
            if os.path.isdir(p):
                for child in os.listdir(p):
                    try:
                        gids.add(os.stat(os.path.join(p, child)).st_gid)
                    except OSError:
                        pass
            else:
                gids.add(os.stat(p).st_gid)
        except OSError:
            pass
    return gids


def run_docker(device_name, device, preset_name, preset, extra_args, script_dir, build_root, target_dir, dry_run=False, log_file=None, collect_profile=None):
    image = device.get('image')
    if not image:
        sys.exit(f"Error: docker device '{device_name}' is missing required 'image'")

    binary = preset['binary']
    binary_path = find_linux_binary(build_root, target_dir, binary)
    if not binary_path and not dry_run:
        sys.exit(
            f"Error: binary '{binary}' not found at multi_build/{target_dir}/bin/{binary}\n"
            "Did you run the build first?"
        )
    if not binary_path:
        binary_path = os.path.join(build_root, target_dir, 'bin', binary)

    build_dir = os.path.join(build_root, target_dir)
    lib_dir = os.path.join(build_dir, 'lib')

    # Scratch dir: a real host directory bind-mounted at the same path inside
    # the container. This lets the standard executable presets — which stage
    # data into $SCRATCH_DIR via `files` and reference $SCRATCH_DIR in args —
    # work unchanged, just like the ssh path. Defaults under the build dir.
    scratchdir = device.get('scratchdir') or os.path.join(build_dir, '.docker_scratch')

    # The repo and scratch dir are bind-mounted at their own paths, so
    # $SCRATCH_DIR/$BUILD_DIR/$SRC_DIR all resolve to real host paths that are
    # valid inside the container too.
    def ev(s):
        return expand_vars(s, scratchdir, build_dir, script_dir)

    # Stage `files` into the scratch dir host-side (local rsync), mirroring the
    # ssh path so the same presets work. Done before the container starts.
    setup_cmds = [['mkdir', '-p', scratchdir]]
    for entry in preset.get('files', []):
        local_abs = ev(entry['local'])
        if not os.path.isabs(local_abs):
            local_abs = os.path.join(script_dir, local_abs)
        remote_path = ev(entry['remote'])
        remote_dir = remote_path if remote_path.endswith('/') else os.path.dirname(remote_path)
        if remote_dir:
            setup_cmds.append(['mkdir', '-p', remote_dir])
        setup_cmds.append(['rsync', '-a', '--checksum', local_abs, remote_path])

    docker_cmd = ['docker', 'run', '--rm', '--init']

    # Resource limits
    if device.get('memory'):
        docker_cmd += ['--memory', str(device['memory'])]
    if device.get('cpus'):
        docker_cmd += ['--cpus', str(device['cpus'])]

    # DRI / GPU access
    dri = device.get('dri_device', '/dev/dri')
    dri_list = dri if isinstance(dri, list) else [dri]
    for d in dri_list:
        docker_cmd += ['--device', d]
    groups = set(str(g) for g in device.get('groups', []))
    groups |= {str(g) for g in _dri_gids(dri_list)}
    for g in sorted(groups):
        docker_cmd += ['--group-add', g]

    # Run as the host user by default so files written to mounted dirs
    # (e.g. screenshots) are owned by the user, not root.
    user = device.get('user', f'{os.getuid()}:{os.getgid()}') if 'user' in device else f'{os.getuid()}:{os.getgid()}'
    if user:
        docker_cmd += ['--user', str(user)]

    # Bind mounts: repo + scratch dir at their own paths, plus any extras.
    mounts = [
        {'local': script_dir, 'remote': script_dir},
        {'local': scratchdir, 'remote': scratchdir},
    ] + device.get('mounts', [])

    # Unique session ID for collisions
    session_id = f"run-{int(time.time())}"
    session_dir = None

    # Environment (device env overridden by preset env), with lib_dir prepended
    # to LD_LIBRARY_PATH so the built shared libraries are found.
    merged_env = {k: ev(str(v)) for k, v in {**device.get('env', {}), **preset.get('env', {})}.items()}

    # If collecting profiles, bind-mount the destination and point TMPDIR to it.
    if collect_profile:
        abs_collect = os.path.abspath(collect_profile)
        session_dir = os.path.join(abs_collect, session_id)
        os.makedirs(session_dir, exist_ok=True)
        mounts.append({'local': abs_collect, 'remote': abs_collect})
        merged_env['TMPDIR'] = session_dir

    for m in mounts:
        local = ev(m['local'])
        remote = ev(m.get('remote', m['local']))
        opt = f'{local}:{remote}'
        if m.get('readonly'):
            opt += ':ro'
        docker_cmd += ['-v', opt]

    prev_ld = merged_env.get('LD_LIBRARY_PATH', '')
    merged_env['LD_LIBRARY_PATH'] = f'{lib_dir}:{prev_ld}' if prev_ld else lib_dir
    for k, v in merged_env.items():
        docker_cmd += ['-e', f'{k}={v}']

    workdir = ev(preset.get('workdir', '$SCRATCH_DIR'))
    docker_cmd += ['-w', workdir]

    all_args = [ev(a) for a in preset.get('args', [])] + extra_args
    docker_cmd += [image, binary_path] + all_args

    if dry_run:
        print_dry_run(setup_cmds + [docker_cmd])
        return

    log_fh = open(log_file, 'w') if log_file else None
    try:
        for cmd in setup_cmds:
            heading = '$ ' + ' '.join(shlex.quote(str(c)) for c in cmd)
            print(heading)
            if log_fh:
                log_fh.write(heading + '\n')
            subprocess.run(cmd, check=True)
        heading = '$ ' + ' '.join(shlex.quote(str(c)) for c in docker_cmd)
        print(heading)
        if log_fh:
            log_fh.write(heading + '\n')
        returncode = _tee_run(docker_cmd, log_fh)
    finally:
        if log_fh:
            log_fh.close()

    if collect_profile:
        # The profiler writes to TMPDIR inside the container; collect
        # from the host side session dir specifically.
        collect_profiles(collect_profile, session_dir, cleanup=True)

    sys.exit(returncode)


def run_android(device_name, device, preset_name, preset, extra_args, build_root, target_dir, dry_run=False, log_file=None, collect_profile=None):
    hostname = device.get('hostname') or (None if dry_run else pick_adb_device())
    if dry_run and not hostname:
        hostname = '<adb-device>'
    package = preset.get('package')
    if not package:
        sys.exit(f"Error: preset '{preset_name}' is missing required 'package' for Android target")

    apk_path = find_android_apk(build_root, target_dir, package)
    if not apk_path and not dry_run:
        apk_dir = f"multi_build/{target_dir}/packaged/android-apk/"
        sys.exit(
            f"Error: no APK found for package '{package}' in {apk_dir}\n"
            "Did you run the build first?"
        )
    if not apk_path:
        apk_path = os.path.join(build_root, target_dir, 'packaged', 'android-apk', f'{package}.apk')

    if dry_run:
        component = f'<launcher-activity-of/{package}>'
    else:
        # Resolve the launcher activity (needed before TUI starts to build launch_cmd)
        result = subprocess.run(
            ['adb', '-s', hostname, 'shell', 'cmd', 'package', 'resolve-activity',
             '--brief', '-a', 'android.intent.action.MAIN',
             '-c', 'android.intent.category.LAUNCHER', package],
            capture_output=True, text=True, check=True,
        )
        component = result.stdout.strip().split('\n')[-1]

    merged_extras = {
        **device.get('env', {}),
        **preset.get('env', {}),
        **preset.get('extras', {}),
    }

    launch_cmd = ['adb', '-s', hostname, 'shell', 'am', 'start', '-n', component]
    for key, value in merged_extras.items():
        launch_cmd += ['--es', key, str(value)]

    def get_logcat_cmd():
        pid = None
        for _ in range(20):
            result = subprocess.run(
                ['adb', '-s', hostname, 'shell', 'pidof', '-s', package],
                capture_output=True, text=True,
            )
            if result.returncode == 0 and result.stdout.strip():
                pid = result.stdout.strip()
                break
            time.sleep(0.5)
        cmd = ['adb', '-s', hostname, 'logcat']
        if pid:
            cmd += ['--pid', pid]
        return cmd

    setup_cmds = []
    if device.get('hostname'):
        setup_cmds.append(['adb', 'connect', hostname])
    setup_cmds += [
        ['adb', '-s', hostname, 'install', '-r', apk_path],
        ['adb', '-s', hostname, 'logcat', '-c'],
        launch_cmd,
    ]

    if dry_run:
        logcat_cmd = ['adb', '-s', hostname, 'logcat', '--pid', f'<pid-of/{package}>']
        print_dry_run(setup_cmds + [logcat_cmd])
        return

    stream_logcat(hostname, package, get_logcat_cmd, device, setup_cmds=setup_cmds, log_file=log_file)

    if collect_profile:
        # Debug builds are debuggable, so the profiler output in the app's
        # private storage can be pulled back with `adb run-as`.
        collect_profiles_android(hostname, package, collect_profile)


def run_web(device_name, device, preset_name, preset, extra_args, script_dir, build_root, target_dir, dry_run=False, log_file=None, collect_profile=None):
    binary = preset.get("binary")
    bundle_path = None
    if binary:
        # Try to find a bundle that matches the binary name first
        matches = glob.glob(os.path.join(build_root, target_dir, "**", f"{binary}.bundle"), recursive=True)
        if matches:
            bundle_path = matches[0]

    if not bundle_path:
        bundle_path = find_web_bundle(build_root, target_dir)

    if not bundle_path and not dry_run:
        sys.exit(
            f"Error: No .bundle found for target {target_dir}\n"
            "Did you run the build first?"
        )
    if not bundle_path:
        bundle_path = os.path.join(build_root, target_dir, f"{binary or 'BlamGraphics'}.bundle")

    bundle_basename = os.path.basename(bundle_path)
    bundle_name = bundle_basename.replace('.bundle', '')
    page_name = f"{bundle_name}.html"

    # Staging directory to avoid polluting the build dir
    web_scratch = os.path.join(build_root, target_dir, ".web_scratch")
    staged_bundle = os.path.join(web_scratch, bundle_basename)

    # Use collect_profile dir or a default tmp dir for output
    out_dir = collect_profile or os.path.join("/tmp", "remote-run-web", preset_name)
    harness_dir = os.path.join(script_dir, ".github", "tests", "web")

    def ev(s):
        return expand_vars(s, staged_bundle, os.path.join(build_root, target_dir), script_dir)

    # Environment for the smoke test
    params = preset.get("extras", {}).copy()

    for arg in extra_args:
        if '=' in arg:
            k, v = arg.split('=', 1)
            params[k.lstrip('-')] = v
        else:
            params[arg.lstrip('-')] = "true"

    param_str = "&".join(f"{k}={v}" for k, v in params.items())

    merged_env = {
        "BUNDLE_DIR": staged_bundle,
        "PAGE": page_name,
        "OUT_DIR": out_dir,
        "SCREENSHOT_NAME": preset_name,
        "ARGS": param_str,
    }

    setup_cmds = [
        ["mkdir", "-p", web_scratch],
        # Sync the base bundle into the scratch dir
        ["rsync", "-a", "--delete", bundle_path + "/", staged_bundle + "/"]
    ]
    # Check if npm install is needed
    if not dry_run and not os.path.exists(os.path.join(harness_dir, "node_modules")):
        setup_cmds.append(["npm", "install", "--prefix", harness_dir])
        
        playwright_install = ["npx", "playwright", "install"]
        if os.environ.get("CI") == "true":
            playwright_install.append("--with-deps")
        playwright_install.append("chromium")
        setup_cmds.append(playwright_install)
    elif dry_run:
        setup_cmds.append(["npm", "install", "--prefix", harness_dir, "(if needed)"])

    # Handle preset files (e.g. maps)
    for entry in preset.get('files', []):
        local_abs = ev(entry['local'])
        if not os.path.isabs(local_abs):
            local_abs = os.path.join(script_dir, local_abs)
        remote_path = ev(entry['remote'])
        remote_dir = os.path.dirname(remote_path)
        if remote_dir:
            setup_cmds.append(['mkdir', '-p', remote_dir])
        setup_cmds.append(['rsync', '-av', '--checksum', local_abs, remote_path])

    test_cmd = ["node", os.path.join(harness_dir, "webgl_smoke.mjs")]

    if dry_run:
        print_dry_run(setup_cmds + [test_cmd])
        return

    view_output(
        test_cmd,
        f"Running WebAssembly smoke test for **{preset_name}**",
        setup_cmds=setup_cmds,
        log_file=log_file,
        env=merged_env,
    )


def print_config_list(config):
    devices = config.get('devices', {})
    presets = config.get('presets', {})

    name_w = max((len(n) for n in devices), default=0)
    target_w = max((len(d.get('target', '')) for d in devices.values()), default=0)
    print("Devices:")
    for name, dev in devices.items():
        target = dev.get('target', '')
        dev_type = dev.get('type') or ('android' if target.startswith('android:') else 'linux')
        endpoint = dev.get('image') if dev_type == 'docker' else dev.get('hostname', '(no hostname)')
        print(f"  {name:<{name_w}}  {target:<{target_w}}  [{dev_type}] {endpoint}")

    print()

    name_w = max((len(n) for n in presets), default=0)
    print("Presets:")
    for name, preset in presets.items():
        parts = [f"binary: {preset.get('binary', '')}"]
        if preset.get('package'):
            parts.append(f"package: {preset['package']}")
        print(f"  {name:<{name_w}}  {'  '.join(parts)}")


def check_ssh(hostname):
    if not hostname:
        return False, None, None
    try:
        # Query architecture, kernel version, and optionally distro info
        cmd = 'uname -m && uname -r && (lsb_release -ds 2>/dev/null || echo "")'
        res = subprocess.run(
            ['ssh', '-o', 'ConnectTimeout=5', '-o', 'BatchMode=yes',
             '-o', 'StrictHostKeyChecking=accept-new', hostname, cmd],
            capture_output=True, text=True, timeout=10
        )
        if res.returncode == 0:
            lines = res.stdout.strip().splitlines()
            arch = lines[0] if len(lines) > 0 else None
            kernel = lines[1] if len(lines) > 1 else None
            distro = lines[2] if len(lines) > 2 else ""
            
            # Use distro description if available, otherwise kernel version
            version = distro if distro else kernel
            return True, arch, version
        return False, None, None
    except (subprocess.TimeoutExpired, OSError):
        return False, None, None


def check_adb(hostname):
    # If hostname is IP:PORT, try connect first
    if hostname and ':' in hostname:
        subprocess.run(['adb', 'connect', hostname], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=2)

    cmd = ['adb']
    if hostname:
        cmd += ['-s', hostname]
    # Query architecture, Android version, and API level
    cmd += ['shell', 'getprop ro.product.cpu.abi && getprop ro.build.version.release && getprop ro.build.version.sdk']

    try:
        res = subprocess.run(cmd, capture_output=True, text=True, timeout=2)
        if res.returncode == 0:
            lines = res.stdout.strip().splitlines()
            arch = lines[0] if len(lines) > 0 else None
            ver = lines[1] if len(lines) > 1 else "?"
            api = lines[2] if len(lines) > 2 else "?"
            return True, arch, f"Android {ver} (API {api})"
        return False, None, None
    except (subprocess.TimeoutExpired, OSError):
        return False, None, None


def check_docker(image=None):
    try:
        # Check if daemon is alive
        res = subprocess.run(['docker', 'info', '--format', '{{.ServerVersion}}'], capture_output=True, text=True, timeout=2)
        if res.returncode != 0:
            return False, None, None
        
        server_ver = res.stdout.strip()

        if image:
            # Check if image exists locally and get its architecture
            res = subprocess.run(
                ['docker', 'image', 'inspect', image, '--format', '{{.Architecture}}'],
                capture_output=True, text=True, timeout=2
            )
            if res.returncode != 0:
                return "image_missing", None, server_ver
            return True, res.stdout.strip(), server_ver

        return True, None, server_ver
    except (subprocess.TimeoutExpired, OSError):
        return False, None, None


def check_web():
    try:
        res = subprocess.run(['node', '--version'], capture_output=True, text=True, timeout=2)
        if res.returncode == 0:
            return True, "x86_64", res.stdout.strip() # Web is local-headless, assume host arch
        return False, None, None
    except (subprocess.TimeoutExpired, OSError):
        return False, None, None


def run_connectivity_check(config):
    devices = config.get('devices', {})
    if not devices:
        print("No devices configured.")
        return

    results = []

    def check_device(name, device):
        target = device.get('target', '')
        # target format: platform:arch:sysroot
        expected_arch = target.split(':')[1] if ':' in target else ''
        dev_type = device.get('type') or ('android' if target.startswith('android:') else 'linux')
        hostname = device.get('hostname', '')

        if dev_type == 'android' and not hostname:
            return (name, hostname, expected_arch, "no_hostname", None, None)

        status = False
        actual_arch = None
        version = None
        if dev_type == 'linux':
            status, actual_arch, version = check_ssh(hostname)
        elif dev_type == 'android':
            status, actual_arch, version = check_adb(hostname)
        elif dev_type == 'docker':
            hostname = device.get('image', '')
            status, actual_arch, version = check_docker(hostname)
        elif dev_type == 'web':
            status, actual_arch, version = check_web()

        return (name, hostname, expected_arch, status, actual_arch, version)

    with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
        future_to_device = {executor.submit(check_device, name, dev): name for name, dev in devices.items()}
        for future in concurrent.futures.as_completed(future_to_device):
            results.append(future.result())

    # Sort results by name
    results.sort(key=lambda x: x[0])

    name_w = max((len(r[0]) for r in results), default=0)
    host_w = max((len(r[1] or '(no hostname)') for r in results), default=0)
    arch_w = 12 # Usually plenty for aarch64/x86_64

    print(f"{' ':1}  {'NAME':<{name_w}}  {'HOSTNAME/IMAGE':<{host_w}}  {'REMOTE ARCH':<{arch_w}}  {'VERSION'}")
    for name, host, expected, status, actual, version in results:
        if status == "no_hostname":
            icon = "×"
        elif status == "image_missing":
            icon = "◑"
        elif status is True:
            icon = "●"
        else:
            icon = "○"

        if status is not True:
            arch_str = "-"
            ver_str = "-"
        elif not actual:
            arch_str = f"({expected}?)"
            ver_str = version or "?"
        else:
            # Normalize common arch names for comparison
            n_actual = actual.lower()
            n_expected = expected.lower()

            if '-' in n_expected:
                n_expected = n_expected.split('-')[0]

            is_match = n_expected in n_actual or n_actual in n_expected
            if not is_match:
                if (n_expected == 'arm' and 'armv7' in n_actual):
                    is_match = True
                elif (n_expected == 'x86_64' and n_actual == 'amd64'):
                    is_match = True
                elif (n_expected in ('arm64', 'aarch64') and n_actual in ('arm64', 'aarch64', 'arm64-v8a')):
                    is_match = True

            if is_match:
                arch_str = f"{actual}"
            else:
                arch_str = f"{actual} (!= {expected})"
            
            ver_str = version or "?"

        print(f"{icon:1}  {name:<{name_w}}  {(host or '(no hostname)'):<{host_w}}  {arch_str:<{arch_w}}  {ver_str}")


def main():
    parser = argparse.ArgumentParser(
        description='Run a built binary on a remote device using .targets.json'
    )
    parser.add_argument('--list', action='store_true', help='List available devices and presets')
    parser.add_argument('--connectivity-check', action='store_true', help='Check reachability of all configured devices')
    parser.add_argument('--dry-run', action='store_true', help='Print commands that would be executed without running them')
    parser.add_argument('--log', metavar='FILE', default=None,
                        help='Write all received output (setup + program) to FILE')
    parser.add_argument('--collect-profile', metavar='DIR', default=None,
                        help='After the run, copy profile.json and *-chrome.json into DIR')
    parser.add_argument('device', nargs='?', help='Device name from .targets.json')
    parser.add_argument('preset', nargs='?', help='Preset name from .targets.json')
    parser.add_argument('extra_args', nargs='*', help='Extra arguments passed to the binary')
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.realpath(__file__))
    config = load_config(script_dir)

    if args.list:
        print_config_list(config)
        sys.exit(0)

    if args.connectivity_check:
        run_connectivity_check(config)
        sys.exit(0)

    devices = config.get('devices', {})
    presets = config.get('presets', {})

    if not args.device or not args.preset:
        parser.error("device and preset are required (or use --list to see available options)")

    if args.device not in devices:
        available = ', '.join(devices.keys())
        sys.exit(f"Error: unknown device '{args.device}'\nAvailable devices: {available}")

    if args.preset not in presets:
        available = ', '.join(presets.keys())
        sys.exit(f"Error: unknown preset '{args.preset}'\nAvailable presets: {available}")

    device = devices[args.device]
    preset = presets[args.preset]
    target = device['target']
    target_dir = target_to_dir(target)
    build_root = os.path.join(script_dir, 'multi_build')

    dev_type = device.get('type') or ('android' if target.startswith('android:') else 'linux')
    if dev_type == 'docker':
        run_docker(args.device, device, args.preset, preset, args.extra_args, script_dir, build_root, target_dir, dry_run=args.dry_run, log_file=args.log, collect_profile=args.collect_profile)
    elif dev_type == 'android':
        run_android(args.device, device, args.preset, preset, args.extra_args, build_root, target_dir, dry_run=args.dry_run, log_file=args.log, collect_profile=args.collect_profile)
    elif dev_type == 'web':
        run_web(args.device, device, args.preset, preset, args.extra_args, script_dir, build_root, target_dir, dry_run=args.dry_run, log_file=args.log, collect_profile=args.collect_profile)
    else:
        run_linux(args.device, device, args.preset, preset, args.extra_args, script_dir, build_root, target_dir, dry_run=args.dry_run, log_file=args.log, collect_profile=args.collect_profile)


if __name__ == '__main__':
    main()
