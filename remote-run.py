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
  devices[name].type       — "linux" (ssh, default), "android" (adb), "dolphin", or "docker"
  devices[name].hostname   — SSH host or ADB serial; omit for android to auto-pick via adb
  devices[name].scratchdir — working directory data is staged into (Linux ssh,
                             docker and android; docker defaults to
                             <build_dir>/.docker_scratch, android to
                             /sdcard/Android/data/<package>/files)
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
                             (android: `adb push`, same trailing-slash meaning)
  presets[name].args       — command-line args passed to the binary
  presets[name].extras     — Android --es intent extras (merged with device env).
                             Values expand $SCRATCH_DIR/$BUILD_DIR/$SRC_DIR.
                             Lowercase keys reach the app as `--key value` argv;
                             COFFEE_* and DUMMY_PLUG_* keys become environment
                             variables. Anything else is dropped by the app.

Flags:
  --build-type dbg|rel — which Android APK flavour to install (default
                          $BUILD_TYPE, else dbg). Release APKs come out of
                          gradle unsigned; they are re-signed with the local
                          Android debug keystore before install, since adb
                          rejects unsigned APKs with
                          INSTALL_PARSE_FAILED_NO_CERTIFICATES.
  --log FILE           — tee all received output (setup + program) to FILE
  --collect-profile DIR — after the run, copy profile.json and *-chrome.json
                          into DIR (docker: from bind-mount/scratch dirs;
                          ssh: scp'd from the remote /tmp; android: pulled from
                          app-private storage via `adb run-as`, debug builds only)
"""

import argparse
from collections import defaultdict
import concurrent.futures
import fnmatch
import glob
import json
import os
import re
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


def find_android_apk(build_root, target_dir, package, build_type=None):
    """Locate the APK for `package`."""
    apk_dir = os.path.join(build_root, target_dir, 'packaged', 'android-apk')
    matches = glob.glob(os.path.join(apk_dir, f'{package}*.apk'))
    if not matches:
        return None

    build_type = build_type or os.environ.get('BUILD_TYPE', 'dbg')
    want = 'release' if build_type.lower().startswith('rel') else 'debug'
    # Exact name first (excludes e.g. `<package>-legacy_debug.apk`), then any
    # APK of the requested flavour, then whatever is newest.
    exact = os.path.join(apk_dir, f'{package}_{want}.apk')
    if exact in matches:
        return exact
    flavour = [m for m in matches if m.endswith(f'_{want}.apk')]
    return max(flavour or matches, key=os.path.getmtime)


def _android_sdk_dir(build_root, target_dir):
    """Resolve the Android SDK: env first, then the sdk.dir gradle writes into
    the generated project's local.properties."""
    for var in ('ANDROID_SDK_ROOT', 'ANDROID_HOME', 'ANDROID_SDK'):
        path = os.environ.get(var)
        if path and os.path.isdir(path):
            return path
    for props in glob.glob(os.path.join(
            build_root, target_dir, 'deploy', 'android-apk', '*', 'local.properties')):
        try:
            with open(props) as f:
                for line in f:
                    key, _, value = line.partition('=')
                    if key.strip() == 'sdk.dir' and os.path.isdir(value.strip()):
                        return value.strip()
        except OSError:
            continue
    return None


def _find_apksigner(build_root, target_dir):
    path = shutil.which('apksigner')
    if path:
        return path
    sdk = _android_sdk_dir(build_root, target_dir)
    if not sdk:
        return None
    # Highest build-tools version available.
    candidates = glob.glob(os.path.join(sdk, 'build-tools', '*', 'apksigner'))
    return max(candidates) if candidates else None


def _debug_keystore():
    """Return the standard Android debug keystore, creating it if absent.
    Returns None if it cannot be produced (no keytool)."""
    keystore = os.path.expanduser('~/.android/debug.keystore')
    if os.path.isfile(keystore):
        return keystore
    keytool = shutil.which('keytool')
    if not keytool:
        return None
    os.makedirs(os.path.dirname(keystore), exist_ok=True)
    res = subprocess.run(
        [keytool, '-genkeypair', '-keystore', keystore, '-storepass', 'android',
         '-keypass', 'android', '-alias', 'androiddebugkey', '-keyalg', 'RSA',
         '-keysize', '2048', '-validity', '10000',
         '-dname', 'CN=Android Debug,O=Android,C=US'],
        capture_output=True, text=True)
    return keystore if res.returncode == 0 else None


def ensure_signed_apk(apk_path, build_root, target_dir):
    """Return a signed APK path for `apk_path`."""
    apksigner = _find_apksigner(build_root, target_dir)
    if not apksigner:
        return apk_path

    verify = subprocess.run(
        [apksigner, 'verify', apk_path], capture_output=True, text=True)
    if verify.returncode == 0:
        return apk_path

    keystore = _debug_keystore()
    if not keystore:
        print("warning: APK is unsigned and no debug keystore could be created "
              "(keytool missing) — install will likely fail")
        return apk_path

    # Kept in a side directory so it is never picked up by find_android_apk.
    signed_dir = os.path.join(os.path.dirname(apk_path), '.signed')
    os.makedirs(signed_dir, exist_ok=True)
    signed = os.path.join(signed_dir, os.path.basename(apk_path))
    # Re-sign whenever the source is newer than a previously signed copy.
    if not (os.path.isfile(signed)
            and os.path.getmtime(signed) >= os.path.getmtime(apk_path)):
        shutil.copy2(apk_path, signed)
        res = subprocess.run(
            [apksigner, 'sign', '--ks', keystore, '--ks-pass', 'pass:android',
             '--key-pass', 'pass:android', '--ks-key-alias', 'androiddebugkey',
             signed],
            capture_output=True, text=True)
        if res.returncode != 0:
            print(f"warning: apksigner failed on {apk_path}:\n{res.stderr.strip()}")
            if os.path.isfile(signed):
                os.remove(signed)
            return apk_path
    print(f"note: {os.path.basename(apk_path)} was unsigned — "
          f"installing debug-signed copy {os.path.basename(signed)}")
    return signed


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


def android_push_cmds(hostname, local, remote):
    """`adb push` with rsync's trailing-slash semantics: "dir/" pushes the
    contents of dir, "dir" pushes the directory itself. adb does not create
    intermediate directories, so each destination is mkdir'd first."""
    def push(src, dst):
        return ['adb', '-s', hostname, 'push', src, dst]

    def mkdir(path):
        return ['adb', '-s', hostname, 'shell', 'mkdir', '-p', path]

    local_path = local.rstrip('/')
    if local.endswith('/') and os.path.isdir(local_path):
        # Contents-of: name every top-level entry, so the destination name
        # never has to match the source name. `adb push` takes many sources.
        dest = remote.rstrip('/')
        entries = [os.path.join(local_path, name)
                   for name in sorted(os.listdir(local_path))]
        if not entries:
            return [mkdir(dest)]
        return [mkdir(dest), ['adb', '-s', hostname, 'push'] + entries + [dest]]
    if os.path.isdir(local_path) or local.endswith('/'):
        dest = remote.rstrip('/')
        return [mkdir(dest), push(local_path, dest)]

    dest = remote.rstrip('/') if remote.endswith('/') else remote
    dest_dir = dest if remote.endswith('/') else (os.path.dirname(dest) or '/')
    return [mkdir(dest_dir), push(local_path, dest)]


def run_android(device_name, device, preset_name, preset, extra_args, script_dir, build_root, target_dir, dry_run=False, log_file=None, collect_profile=None, build_type=None):
    hostname = device.get('hostname') or (None if dry_run else pick_adb_device())
    if dry_run and not hostname:
        hostname = '<adb-device>'
    package = preset.get('package')
    if not package:
        sys.exit(f"Error: preset '{preset_name}' is missing required 'package' for Android target")

    # The app's external data dir: the one location adb can write to without
    # run-as, and what the app resolves RSCA::AssetFile against — so a pushed
    # map or dummy plug config is reachable by plain name.
    scratchdir = device.get('scratchdir') or f'/sdcard/Android/data/{package}/files'
    build_dir = os.path.join(build_root, target_dir)

    def ev(s):
        return expand_vars(str(s), scratchdir, build_dir, script_dir)

    apk_path = find_android_apk(build_root, target_dir, package, build_type)
    if not apk_path and not dry_run:
        apk_dir = f"multi_build/{target_dir}/packaged/android-apk/"
        sys.exit(
            f"Error: no APK found for package '{package}' in {apk_dir}\n"
            "Did you run the build first?"
        )
    if not apk_path:
        apk_path = os.path.join(build_root, target_dir, 'packaged', 'android-apk', f'{package}.apk')
    else:
        apk_path = ensure_signed_apk(apk_path, build_root, target_dir)

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
        key: ev(value) for key, value in {
            **device.get('env', {}),
            **preset.get('env', {}),
            **preset.get('extras', {}),
        }.items()
    }

    launch_cmd = ['adb', '-s', hostname, 'shell', 'am', 'start', '-n', component]
    for key, value in merged_extras.items():
        launch_cmd += ['--es', key, value]

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
        if not pid:
            return ['adb', '-s', hostname, 'logcat']
        # `adb logcat --pid` keeps tailing after the app is gone, so a scenario
        # that closes its own window (dummy plug "end_time") would never hand
        # control back — and the profile would never be collected. Follow the
        # pid instead, with a grace period for the last lines to arrive.
        adb = f'adb -s {shlex.quote(hostname)}'
        return ['sh', '-c', '; '.join([
            f'{adb} logcat --pid {shlex.quote(pid)} & log_pid=$!',
            f'while {adb} shell pidof -s {shlex.quote(package)} >/dev/null 2>&1'
            '; do sleep 1; done',
            'sleep 2',
            'kill $log_pid 2>/dev/null',
            'wait $log_pid 2>/dev/null',
            'exit 0',
        ])]

    setup_cmds = []
    if device.get('hostname'):
        setup_cmds.append(['adb', 'connect', hostname])
    setup_cmds.append(['adb', '-s', hostname, 'install', '-r', apk_path])

    for entry in preset.get('files', []):
        # Trailing slashes survive expansion and os.path.join, and they are
        # what tells a directory apart from its contents.
        local_abs = ev(entry['local'])
        if not os.path.isabs(local_abs):
            local_abs = os.path.join(script_dir, local_abs)
        setup_cmds += android_push_cmds(hostname, local_abs, ev(entry['remote']))

    setup_cmds += [
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

    # Environment for WebAssembly in Playwright
    params = preset.get("extras", {}).copy()
    params.update(preset.get("web_extras", {}))

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

    dummy_plug_cfg = preset.get("env", {}).get("DUMMY_PLUG_CONFIG")
    if dummy_plug_cfg:
        merged_env["DUMMY_PLUG"] = ev(dummy_plug_cfg)

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

def run_dolphin(
        device_name,
        device,
        preset_name,
        preset,
        extra_args,
        script_dir,
        build_root,
        target_dir,
        dry_run=False,
        log_file=None,
        collect_profile=None):
    binary = preset['binary']
    binary_path = find_linux_binary(build_root, target_dir, binary)
    if not binary_path and not dry_run:
        sys.exit(
            f"Error: binary '{binary}' not found at multi_build/{target_dir}/bin/{binary}\n"
            "Did you run the build first?"
        )
    if not binary_path:
        binary_path = os.path.join(build_root, target_dir, 'bin', binary)

    # Dolphin does not like when a binary does not have an extension
    staged_binary = f'/tmp/{time.time()}.elf'
    dump_file = f'dump_{time.time()}.mp4'
    if collect_profile:
        os.makedirs(collect_profile, exist_ok=True)
        dump_file = f'{collect_profile}/{dump_file}'
        if log_file is None:
            log_file = f'{collect_profile}/output.log'
    else:
        dump_file = f'/tmp/{dump_file}'

    cmd = ['bash', '-c', f'DUMP_FILE={dump_file} {script_dir}/.github/tests/dolphin/dolphin.sh {shlex.quote(staged_binary)}']

    setup_cmds = [
        ['ln', '-s', binary_path, staged_binary]
    ]

    if dry_run:
        print_dry_run(setup_cmds + [cmd])
        return

    view_output(
        cmd,
        toptext_message(device, binary),
        setup_cmds=setup_cmds,
        log_file=log_file,
    )
    os.remove(staged_binary)


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


def _linux_list_hwmon(cmd_prefix):
    health_cmd = 'grep . /sys/class/hwmon/*/name /sys/class/hwmon/*/temp*_input'
    health_res = subprocess.run(
        [*cmd_prefix, health_cmd],
        capture_output=True, text=True, timeout=10
    )
    health = health_res.stdout.strip().splitlines()
    health = [x[len('/sys/class/hwmon/'):] for x in health]
    health_bucket = defaultdict(list)
    for stat in health:
        sensor, data = stat.split('/')
        health_bucket[sensor].append(data)
    health = {}
    for sensor, data in health_bucket.items():
        name = None
        values = []
        for prop in data:
            key, value = prop.split(':')
            if key == 'name':
                name = value
            else:
                values.append(value)
        if len(values) == 0:
            continue
        health[name] = values
    if len(health) > 4 and 'cpuss0_thermal' in health:
        remove_keys = set()
        for key in health.keys():
            if 'puss' in key:
                continue
            remove_keys.add(key)
        for key in remove_keys:
            health.pop(key)

    return health


def check_ssh(hostname):
    if not hostname:
        return False, None, None, None
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
            health = _linux_list_hwmon(
                ['ssh', '-o', 'ConnectTimeout=5', '-o', 'BatchMode=yes',
                 '-o', 'StrictHostKeyChecking=accept-new', hostname])
            
            # Use distro description if available, otherwise kernel version
            version = distro if distro else kernel
            return True, arch, version, health
        return False, None, None, None
    except (subprocess.TimeoutExpired, OSError):
        return False, None, None, None


def check_adb(hostname):
    # If hostname is IP:PORT, try connect first
    if hostname and ':' in hostname:
        try:
            subprocess.run(['adb', 'connect', hostname], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=2)
        except (subprocess.TimeoutExpired, OSError):
            return False, None, None, None

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
            health = _linux_list_hwmon(['adb', '-s', hostname, 'shell'])
            return True, arch, f"Android {ver} (API {api})", health
        return False, None, None, None
    except (subprocess.TimeoutExpired, OSError):
        return False, None, None, None


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
                return "image_missing", None, f'Docker {server_ver}'
            return True, res.stdout.strip(), f'Docker {server_ver}'

        return True, None, server_ver
    except (subprocess.TimeoutExpired, OSError):
        return False, None, None


def check_web():
    try:
        res = subprocess.run(['node', '--version'], capture_output=True, text=True, timeout=2)
        if res.returncode == 0:
            return True, "x86_64", f'Node {res.stdout.strip()}' # Web is local-headless, assume host arch
        return False, None, None
    except (subprocess.TimeoutExpired, OSError):
        return False, None, None


def check_dolphin():
    src_dir = os.path.dirname(__file__)
    if not os.path.exists(f'{src_dir}/multi_build/runtime/dolphin/bin/dolphin-emu-nogui'):
        return 'not_built', None, None, 'Build with .github/tests/dolphin/build.sh'
    try:
        res = subprocess.run(
            [f'{src_dir}/multi_build/runtime/dolphin/bin/dolphin-emu-nogui', '--version'],
            capture_output=True,
            text=True,
            timeout=2)
        return True, None, res.stdout.strip(), None
    except (subprocess.TimeoutExpired, OSError):
        return 'broken', None, None, 'Version check failed'


def _run_remote(cmd_prefix, shell_cmd, timeout=40):
    """Run a single shell command on the remote (cmd_prefix is the ssh/adb
    invocation prefix). Returns (returncode, stdout, stderr)."""
    try:
        res = subprocess.run(
            [*cmd_prefix, shell_cmd], capture_output=True, text=True, timeout=timeout)
        return res.returncode, res.stdout, res.stderr
    except (subprocess.TimeoutExpired, OSError) as e:
        return 1, '', str(e)


def _split_markers(out):
    """Split output produced with `echo "@@name"` headers into a dict of
    name -> list[line]."""
    sections = {}
    cur = None
    for line in out.splitlines():
        if line.startswith('@@'):
            cur = line[2:].strip()
            sections[cur] = []
        elif cur is not None:
            sections[cur].append(line)
    return sections


def _parse_os_release(lines):
    """Parse KEY=VALUE lines from /etc/os-release into a dict (quotes stripped)."""
    data = {}
    for line in lines:
        if '=' not in line or line.strip().startswith('#'):
            continue
        key, _, value = line.partition('=')
        data[key.strip()] = value.strip().strip('"').strip("'")
    return data


def _collect_hwmon(cmd_prefix):
    """Enumerate every hwmon temperature sensor on the remote.
    Returns {chip_dir: {'name': str, 'temps': {tempN: {'label':, 'value':}}}}."""
    grep = ('grep -H . /sys/class/hwmon/*/name '
            '/sys/class/hwmon/*/temp*_input '
            '/sys/class/hwmon/*/temp*_label 2>/dev/null')
    _, out, _ = _run_remote(cmd_prefix, grep)
    prefix = '/sys/class/hwmon/'
    chips = {}
    for line in out.strip().splitlines():
        if not line.startswith(prefix) or ':' not in line:
            continue
        path, _, value = line.partition(':')
        chip, _, prop = path[len(prefix):].partition('/')
        entry = chips.setdefault(chip, {'name': chip, 'temps': {}})
        if prop == 'name':
            entry['name'] = value
        elif prop.endswith('_input'):
            entry['temps'].setdefault(prop[:-len('_input')], {})['value'] = value
        elif prop.endswith('_label'):
            entry['temps'].setdefault(prop[:-len('_label')], {})['label'] = value
    return chips


def _print_hwmon(chips):
    rows = []
    for chip in sorted(chips):
        info = chips[chip]
        for key in sorted(info['temps'], key=lambda k: (len(k), k)):
            temp = info['temps'][key]
            if 'value' not in temp:
                continue
            try:
                celsius = f"{float(temp['value']) / 1000:.1f} °C"
            except ValueError:
                celsius = temp['value']
            rows.append((info['name'], chip, temp.get('label', key), celsius))
    if not rows:
        print("  (no hwmon temperature sensors found)")
        return
    name_w = max(len(r[0]) for r in rows)
    chip_w = max(len(r[1]) for r in rows)
    label_w = max(len(r[2]) for r in rows)
    for name, chip, label, celsius in rows:
        print(f"  {name:<{name_w}}  {chip:<{chip_w}}  {label:<{label_w}}  {celsius:>9}")


# PCI vendor id -> friendly name for DRI device listing.
_PCI_VENDORS = {
    '8086': 'Intel', '10de': 'NVIDIA', '1002': 'AMD', '1022': 'AMD',
    '1af4': 'virtio', '1234': 'QEMU', '15ad': 'VMware', '1b36': 'QEMU',
}


def _clean_lspci(desc):
    """Reduce an `lspci -D -nn` description to just the device model name.
    'VGA compatible controller [0300]: Intel Corporation ... [UHD Graphics 620]
    [8086:3ea0] (rev 02)' -> 'Intel Corporation ... [UHD Graphics 620]'."""
    desc = re.sub(r'^[^:]*:\s*', '', desc)                       # drop class prefix
    desc = re.sub(r'\s*\[[0-9A-Fa-f]{4}:[0-9A-Fa-f]{4}\].*$', '', desc)  # drop id/rev tail
    return desc.strip()


def _print_dri_linux(sec):
    """Print DRI render/card nodes from /dev/dri with their kernel driver and a
    resolved GPU model name. Model comes from lspci (by PCI slot) when present,
    else the device-tree OF_COMPATIBLE_0 (SoC GPUs), else vendor:device id.
    dri lines: 'node|driver|PCI_ID=..|SLOT=0000:00:02.0|OF_COMPATIBLE_0=..'."""
    lspci = {}
    for line in sec.get('lspci', []):
        line = line.strip()
        if not line:
            continue
        slot, _, desc = line.partition(' ')
        lspci[slot] = desc.strip()

    rows = []
    for line in sec.get('dri', []):
        parts = line.split('|')
        if not parts or not parts[0].strip():
            continue
        node = parts[0].strip()
        driver = (parts[1].strip() if len(parts) > 1 else '') or '?'
        rest = ' '.join(parts[2:])

        model = None
        slot = re.search(r'SLOT=(\S+)', rest)
        if slot and slot.group(1) in lspci:
            model = _clean_lspci(lspci[slot.group(1)])
        if not model:
            of = re.search(r'OF_COMPATIBLE_0=(\S+)', rest)
            if of:
                model = of.group(1)
        if not model:
            pci = re.search(r'PCI_ID=([0-9A-Fa-f]{4}):([0-9A-Fa-f]{4})', rest)
            if pci:
                vid, did = pci.group(1).lower(), pci.group(2).lower()
                model = f"{_PCI_VENDORS.get(vid, vid)}:{did}"
        rows.append((node, driver, model or '-'))

    if not rows:
        print("  (no /dev/dri devices found)")
        return
    node_w = max(len(r[0]) for r in rows)
    drv_w = max(len(r[1]) for r in rows)
    for node, driver, model in rows:
        print(f"  {node:<{node_w}}  {driver:<{drv_w}}  {model}")


def _print_display_linux(sec):
    """Print framebuffer sizes (/sys/class/graphics/fb*) and DRM connector
    native modes (/sys/class/drm/*/modes) from marker sections."""
    rows = []
    for line in sec.get('fb', []):
        parts = line.split('|')
        if len(parts) != 3:
            continue
        dev, name, size = parts
        size = size.strip().replace(',', 'x')
        if not size:
            continue
        label = f"Framebuffer {dev}" + (f" ({name.strip()})" if name.strip() else "")
        rows.append((label, size))

    # DRM: first (preferred) mode per connector, connected ones only.
    prefix = '/sys/class/drm/'
    seen = set()
    for line in sec.get('drm', []):
        if not line.startswith(prefix) or '/modes:' not in line:
            continue
        path, _, mode = line.partition(':')
        conn = path[len(prefix):].split('/modes')[0]
        if conn in seen or not mode.strip():
            continue
        seen.add(conn)
        rows.append((f"Connector {conn}", mode.strip()))

    if not rows:
        print("  (no framebuffer or DRM connector found)")
        return
    label_w = max(len(r[0]) for r in rows)
    for label, size in rows:
        print(f"  {label:<{label_w}}  {size}")


# Shell that emits every marker section consumed by _checkup_linux_sections.
# Runs on any POSIX sh (ssh host, or `docker run ... image sh -c`).
_LINUX_INFO_CMD = (
    'echo "@@kernel"; uname -r; '
    'echo "@@arch"; uname -m; '
    'echo "@@os"; cat /etc/os-release 2>/dev/null; '
    'echo "@@socmodel"; cat /proc/device-tree/model 2>/dev/null | tr -d "\\0"; echo; '
    'echo "@@soc"; cat /proc/device-tree/compatible 2>/dev/null | tr "\\0" " "; echo; '
    'echo "@@fb"; for f in /sys/class/graphics/fb*; do '
    '[ -e "$f/virtual_size" ] || continue; '
    'echo "$(basename "$f")|$(cat "$f/name" 2>/dev/null)|$(cat "$f/virtual_size" 2>/dev/null)"; done; '
    'echo "@@drm"; grep -H . /sys/class/drm/*/modes 2>/dev/null || true; '
    'echo "@@dri"; for b in /dev/dri/card* /dev/dri/renderD*; do '
    '[ -e "$b" ] || continue; n=$(basename "$b"); u=/sys/class/drm/$n/device/uevent; '
    'echo "$n|$(basename "$(readlink /sys/class/drm/$n/device/driver 2>/dev/null)" 2>/dev/null)'
    '|$(grep -h PCI_ID $u 2>/dev/null)|SLOT=$(basename "$(readlink /sys/class/drm/$n/device 2>/dev/null)")'
    '|$(grep -h OF_COMPATIBLE_0 $u 2>/dev/null)"; done; '
    'echo "@@lspci"; lspci -D -nn 2>/dev/null || true; '
    'echo "@@apt"; '
    'if command -v apt-get >/dev/null 2>&1; then '
    'apt-get --just-print upgrade 2>/dev/null | grep "^Inst" || true; '
    'else echo "@@noapt"; fi'
)


def _checkup_linux_sections(cmd_prefix, unreachable):
    """Run the Linux info command via cmd_prefix (ssh host / docker run prefix)
    and print the System/Display/DRI/apt/temperature sections."""
    rc, out, err = _run_remote(cmd_prefix, _LINUX_INFO_CMD)
    if rc != 0 and not out:
        print(f"Error: {unreachable}")
        if err.strip():
            print("  " + err.strip())
        return
    sec = _split_markers(out)
    os_rel = _parse_os_release(sec.get('os', []))

    kernel = '\n'.join(sec.get('kernel', [])).strip() or '(unknown)'
    arch = '\n'.join(sec.get('arch', [])).strip() or '(unknown)'
    distro = os_rel.get('PRETTY_NAME') or os_rel.get('NAME') or '(unknown)'
    version = os_rel.get('VERSION_ID') or os_rel.get('VERSION') or '(unknown)'

    soc_model = '\n'.join(sec.get('socmodel', [])).strip()
    soc_compat = '\n'.join(sec.get('soc', [])).strip()

    print("System")
    print(f"  Distro:   {distro}")
    print(f"  Version:  {version}")
    print(f"  Kernel:   {kernel}")
    print(f"  Arch:     {arch}")
    if soc_model or soc_compat:
        if soc_model and soc_compat:
            print(f"  SoC:      {soc_model}  [{soc_compat}]")
        else:
            print(f"  SoC:      {soc_model or soc_compat}")

    print()
    print("Display")
    _print_display_linux(sec)

    print()
    print("Rendering devices (DRI)")
    _print_dri_linux(sec)

    print()
    print("Pending apt updates")
    if 'noapt' in sec:
        print("  (apt-get not available on this device)")
    else:
        inst = [l for l in sec.get('apt', []) if l.startswith('Inst')]
        if not inst:
            print("  (system up to date)")
        else:
            print(f"  {len(inst)} package(s) can be updated")

    print()
    print("Temperature sensors (hwmon)")
    _print_hwmon(_collect_hwmon(cmd_prefix))


def _checkup_linux(hostname):
    cmd_prefix = ['ssh', '-o', 'ConnectTimeout=5', '-o', 'BatchMode=yes',
                  '-o', 'StrictHostKeyChecking=accept-new', hostname]
    _checkup_linux_sections(cmd_prefix, f"could not reach {hostname} over ssh")


def _checkup_docker(device_name, device):
    image = device.get('image')
    if not image:
        sys.exit(f"Error: docker device '{device_name}' is missing required 'image'")

    dri = device.get('dri_device', '/dev/dri')
    dri_list = dri if isinstance(dri, list) else [dri]
    groups = sorted(
        {str(g) for g in device.get('groups', [])} |
        {str(g) for g in _dri_gids(dri_list)})
    user = device.get('user', f'{os.getuid()}:{os.getgid()}') \
        if 'user' in device else f'{os.getuid()}:{os.getgid()}'

    print("Docker config")
    print(f"  Image:    {image}")
    if device.get('memory'):
        print(f"  Memory:   {device['memory']}")
    if device.get('cpus'):
        print(f"  CPUs:     {device['cpus']}")
    if user:
        print(f"  User:     {user}")
    if groups:
        print(f"  Groups:   {' '.join(groups)}")
    print("  DRI devices:")
    for d in dri_list:
        exists = "present" if os.path.exists(d) else "MISSING on host"
        print(f"    {d}  ({exists})")
    mounts = device.get('mounts', [])
    if mounts:
        print("  Mounts:")
        for m in mounts:
            ro = " (ro)" if m.get('readonly') else ""
            print(f"    {m['local']} -> {m.get('remote', m['local'])}{ro}")

    # Build a `docker run` prefix that mirrors run_docker's device/group access,
    # then reuse the Linux section printer inside the container. /sys is mounted
    # in containers, so hwmon/DRM sysfs and the passed /dev/dri nodes are all
    # visible and reflect what the app actually sees at runtime.
    docker_prefix = ['docker', 'run', '--rm']
    if device.get('memory'):
        docker_prefix += ['--memory', str(device['memory'])]
    if device.get('cpus'):
        docker_prefix += ['--cpus', str(device['cpus'])]
    for d in dri_list:
        if os.path.exists(d):
            docker_prefix += ['--device', d]
    for g in groups:
        docker_prefix += ['--group-add', g]
    if user:
        docker_prefix += ['--user', str(user)]
    docker_prefix += [image, 'sh', '-c']

    print()
    _checkup_linux_sections(
        docker_prefix, f"could not run container from image '{image}'")


def _checkup_android(hostname):
    if hostname and ':' in hostname:
        try:
            subprocess.run(['adb', 'connect', hostname],
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=5)
        except (subprocess.TimeoutExpired, OSError):
            pass
    cmd_prefix = ['adb', '-s', hostname, 'shell']
    info_cmd = (
        'echo "@@kernel"; uname -r; '
        'echo "@@arch"; getprop ro.product.cpu.abi; '
        'echo "@@rel"; getprop ro.build.version.release; '
        'echo "@@sdk"; getprop ro.build.version.sdk; '
        'echo "@@model"; getprop ro.product.model; '
        'echo "@@patch"; getprop ro.build.version.security_patch; '
        'echo "@@wmsize"; wm size 2>/dev/null; '
        'echo "@@wmdensity"; wm density 2>/dev/null; '
        'echo "@@egl"; getprop ro.hardware.egl; '
        'echo "@@vulkan"; getprop ro.hardware.vulkan; '
        'echo "@@gles"; dumpsys SurfaceFlinger 2>/dev/null | grep -i "GLES:"; '
        'echo "@@gpunodes"; ls -d /dev/dri/* /dev/kgsl-3d0 /dev/mali* 2>/dev/null'
    )
    rc, out, err = _run_remote(cmd_prefix, info_cmd)
    if rc != 0 and not out:
        print(f"Error: could not reach {hostname} over adb")
        if err.strip():
            print("  " + err.strip())
        return
    sec = _split_markers(out)

    def one(name):
        return '\n'.join(sec.get(name, [])).strip() or '(unknown)'

    print("System")
    print(f"  Distro:   Android {one('rel')} (API {one('sdk')})")
    print(f"  Model:    {one('model')}")
    print(f"  Patch:    {one('patch')}")
    print(f"  Kernel:   {one('kernel')}")
    print(f"  Arch:     {one('arch')}")

    print()
    print("Display")
    disp = sec.get('wmsize', []) + sec.get('wmdensity', [])
    disp = [l.strip() for l in disp if l.strip()]
    if disp:
        for line in disp:
            print(f"  {line}")
    else:
        print("  (unavailable)")

    print()
    print("GPU / rendering devices")
    gles = ' '.join(l.strip() for l in sec.get('gles', []) if l.strip())
    gles = re.sub(r'^GLES:\s*', '', gles)
    nodes = [l.strip() for l in sec.get('gpunodes', []) if l.strip()]
    print(f"  EGL HAL:    {one('egl')}")
    print(f"  Vulkan HAL: {one('vulkan')}")
    print(f"  Renderer:   {gles or '(unavailable)'}")
    print(f"  Nodes:      {' '.join(nodes) if nodes else '(none)'}")

    print()
    print("Pending apt updates")
    print("  N/A (Android has no apt)")

    print()
    print("Temperature sensors (hwmon)")
    _print_hwmon(_collect_hwmon(cmd_prefix))


def run_checkup(config, device_name):
    devices = config.get('devices', {})
    if device_name not in devices:
        available = ', '.join(devices.keys())
        sys.exit(f"Error: unknown device '{device_name}'\nAvailable devices: {available}")

    device = devices[device_name]
    target = device.get('target', '')
    dev_type = device.get('type') or ('android' if target.startswith('android:') else 'linux')
    hostname = device.get('hostname', '')

    print(f"Checkup: {device_name}  [{dev_type}]  {target or '(no target)'}")
    print("=" * 64)

    if dev_type == 'linux':
        if not hostname:
            sys.exit(f"Error: device '{device_name}' has no hostname")
        _checkup_linux(hostname)
    elif dev_type == 'android':
        _checkup_android(hostname or pick_adb_device())
    elif dev_type == 'docker':
        _checkup_docker(device_name, device)
    else:
        sys.exit(f"Error: --checkup only supports linux, android, and docker devices, not '{dev_type}'")


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
            return (name, hostname, expected_arch, "no_hostname", None, None, None)

        status = False
        actual_arch = None
        version = None
        health = None
        if dev_type == 'linux':
            status, actual_arch, version, health = check_ssh(hostname)
        elif dev_type == 'android':
            status, actual_arch, version, health = check_adb(hostname)
        elif dev_type == 'docker':
            hostname = device.get('image', '')
            status, actual_arch, version = check_docker(hostname)
        elif dev_type == 'web':
            status, actual_arch, version = check_web()
            expected_arch = expected_arch.split('-')[0]
            actual_arch = expected_arch
        elif dev_type == 'dolphin':
            status, actual_arch, version, health = check_dolphin()
            expected_arch = expected_arch.split('-')[0]
            actual_arch = expected_arch

        return (name, hostname, expected_arch, status, actual_arch, version, health)

    with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
        future_to_device = {executor.submit(check_device, name, dev): name for name, dev in devices.items()}
        for future in concurrent.futures.as_completed(future_to_device):
            results.append(future.result())

    # Sort results by name
    results.sort(key=lambda x: x[0])

    name_w = max((len(r[0]) for r in results), default=0)
    host_w = max((len(r[1] or '(no hostname)') for r in results), default=0)
    arch_w = 12 # Usually plenty for aarch64/x86_64
    vers_w = 32

    print(f"{' ':1}  {'NAME':<{name_w}}  {'HOSTNAME/IMAGE':<{host_w}}  {'REMOTE ARCH':<{arch_w}}  {'VERSION':<{vers_w}}  HEALTH")
    for name, host, expected, status, actual, version, health in results:
        if status == "no_hostname" or status == 'not_built':
            icon = "×"
        elif status == "image_missing" or status == 'broken':
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
        health_str = None
        if health is None:
            health_str = '-'
        elif type(health) is str:
            health_str = health
        elif len(health) == 1:
            health_str = f'thermal={float([v for _, v in health.items()][0][0]) / 1000}'
        else:
            health_str = ' '.join([
                f'{k}={float(v[0]) / 1000}'
                for k, v in health.items()
                if 'cpu' in k or 'gpu' in k or 'soc' in k
            ])

        print(f"{icon:1}  {name:<{name_w}}  {(host or '(no hostname)'):<{host_w}}  {arch_str:<{arch_w}}  {ver_str:<{vers_w}}  {health_str}")


def main():
    parser = argparse.ArgumentParser(
        description='Run a built binary on a remote device using .targets.json'
    )
    parser.add_argument('--list', action='store_true', help='List available devices and presets')
    parser.add_argument('--connectivity-check', action='store_true', help='Check reachability of all configured devices')
    parser.add_argument('--checkup', metavar='DEVICE', default=None,
                        help='Collect info from one linux/android/docker device: '
                             'system specs, display, DRI/GPU devices, temperature '
                             'sensors, and pending apt updates')
    parser.add_argument('--dry-run', action='store_true', help='Print commands that would be executed without running them')
    parser.add_argument('--log', metavar='FILE', default=None,
                        help='Write all received output (setup + program) to FILE')
    parser.add_argument('--collect-profile', metavar='DIR', default=None,
                        help='After the run, copy profile.json and *-chrome.json into DIR')
    parser.add_argument('--build-type', metavar='dbg|rel', default=None,
                        help='Which Android APK flavour to install '
                             '(default: $BUILD_TYPE, else dbg)')
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

    if args.checkup:
        run_checkup(config, args.checkup)
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
        run_android(args.device, device, args.preset, preset, args.extra_args, script_dir, build_root, target_dir, dry_run=args.dry_run, log_file=args.log, collect_profile=args.collect_profile, build_type=args.build_type)
    elif dev_type == 'web':
        run_web(args.device, device, args.preset, preset, args.extra_args, script_dir, build_root, target_dir, dry_run=args.dry_run, log_file=args.log, collect_profile=args.collect_profile)
    elif dev_type == 'dolphin':
        run_dolphin(args.device, device, args.preset, preset, args.extra_args, script_dir, build_root, target_dir, dry_run=args.dry_run, log_file=args.log, collect_profile=args.collect_profile)
    else:
        run_linux(args.device, device, args.preset, preset, args.extra_args, script_dir, build_root, target_dir, dry_run=args.dry_run, log_file=args.log, collect_profile=args.collect_profile)


if __name__ == '__main__':
    main()
