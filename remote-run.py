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
  devices[name].hostname   — SSH host or ADB serial; omit for android to auto-pick via adb
  devices[name].scratchdir — remote working directory (Linux only)
  devices[name].env        — environment variables set on the remote
  devices[name].viewer     — optional display viewer: {type: "web"|"scrcpy", address: "..."}
  presets[name].binary     — executable name under multi_build/<target>/bin/
  presets[name].package    — Android package name (required for android targets)
  presets[name].workdir    — remote cwd, supports $SCRATCH_DIR, $BUILD_DIR, and $SRC_DIR
  presets[name].files      — list of {local, remote} rsync transfers before launch
  presets[name].args       — command-line args passed to the binary
  presets[name].extras     — Android --es intent extras (merged with device env)
"""

import argparse
import glob
import json
import os
import shlex
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

    def __init__(self, cmd, toptext_msg, setup_cmds=None, on_quit=None):
        super().__init__()
        self.cmd = cmd          # list[str] or callable returning list[str]
        self.toptext_msg = toptext_msg
        self.setup_cmds = setup_cmds or []
        self._on_quit = on_quit
        self._proc = None
        self._quitting = False
        self._on_quit_called = False
        self._stream_done = False

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
        self.call_from_thread(log.write, "")
        self.call_from_thread(log.write, RichText("$ " + " ".join(str(c) for c in cmd), style="bold"))
        self._proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        for line in self._proc.stdout:
            def write_line(l=line.rstrip()):
                log.auto_scroll = log.is_vertical_scroll_end
                log.write(l)
            self.call_from_thread(write_line)
        self._proc.wait()
        code = self._proc.returncode
        self.call_from_thread(log.write, RichText(f"exit {code}", style="green" if code == 0 else "bold red"))
        return code == 0

    @work(thread=True)
    def _stream(self) -> None:
        log = self.query_one(RichLog)

        for cmd in self.setup_cmds:
            if not self._run_setup(log, cmd):
                break
        else:
            actual_cmd = self.cmd() if callable(self.cmd) else self.cmd
            self.call_from_thread(log.write, "")
            self.call_from_thread(log.write, RichText("$ " + " ".join(str(c) for c in actual_cmd), style="bold"))
            self._proc = subprocess.Popen(
                actual_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
            )
            for line in self._proc.stdout:
                def write_line(l=line.rstrip()):
                    log.auto_scroll = log.is_vertical_scroll_end
                    log.write(l)
                self.call_from_thread(write_line)
            self._proc.wait()
            code = self._proc.returncode
            self.call_from_thread(log.write, RichText(f"exit {code}", style="green" if code == 0 else "bold red"))

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


def stream_logcat(hostname, package, cmd, device, setup_cmds=None):
    def cleanup():
        subprocess.run(['adb', '-s', hostname, 'shell', 'am', 'force-stop', package])
    OutputViewerApp(
        cmd,
        toptext_message(device, hostname),
        setup_cmds=setup_cmds,
        on_quit=cleanup,
    ).run()


def find_linux_binary(build_root, target_dir, binary):
    path = os.path.join(build_root, target_dir, 'bin', binary)
    return path if os.path.isfile(path) else None


def find_android_apk(build_root, target_dir, package):
    apk_dir = os.path.join(build_root, target_dir, 'packaged', 'android-apk')
    matches = glob.glob(os.path.join(apk_dir, f'{package}*.apk'))
    return matches[0] if matches else None


def print_dry_run(cmds):
    for cmd in cmds:
        print('$', ' '.join(shlex.quote(str(c)) for c in cmd))


def run_linux(device_name, device, preset_name, preset, extra_args, script_dir, build_root, target_dir, dry_run=False):
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
        ['rsync', '-av', '--checksum', '--chmod=+x', binary_path, f'{hostname}:{remote_binary}'],
    ]
    for entry in preset.get('files', []):
        local_abs = ev(entry['local'])
        if not os.path.isabs(local_abs):
            local_abs = os.path.join(script_dir, local_abs)
        remote_path = ev(entry['remote'])
        setup_cmds.append(['rsync', '-av', '--checksum', local_abs, f'{hostname}:{remote_path}'])

    if dry_run:
        print_dry_run(setup_cmds + [['ssh', hostname, '--', remote_cmd]])
        return

    OutputViewerApp(
        ['ssh', hostname, '--', remote_cmd],
        toptext_message(device, hostname),
        setup_cmds=setup_cmds,
    ).run()


def run_android(device_name, device, preset_name, preset, extra_args, build_root, target_dir, dry_run=False):
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

    stream_logcat(hostname, package, get_logcat_cmd, device, setup_cmds=setup_cmds)


def print_config_list(config):
    devices = config.get('devices', {})
    presets = config.get('presets', {})

    name_w = max((len(n) for n in devices), default=0)
    target_w = max((len(d.get('target', '')) for d in devices.values()), default=0)
    print("Devices:")
    for name, dev in devices.items():
        target = dev.get('target', '')
        hostname = dev.get('hostname', '(no hostname)')
        print(f"  {name:<{name_w}}  {target:<{target_w}}  {hostname}")

    print()

    name_w = max((len(n) for n in presets), default=0)
    print("Presets:")
    for name, preset in presets.items():
        parts = [f"binary: {preset.get('binary', '')}"]
        if preset.get('package'):
            parts.append(f"package: {preset['package']}")
        print(f"  {name:<{name_w}}  {'  '.join(parts)}")


def main():
    parser = argparse.ArgumentParser(
        description='Run a built binary on a remote device using .targets.json'
    )
    parser.add_argument('--list', action='store_true', help='List available devices and presets')
    parser.add_argument('--dry-run', action='store_true', help='Print commands that would be executed without running them')
    parser.add_argument('device', nargs='?', help='Device name from .targets.json')
    parser.add_argument('preset', nargs='?', help='Preset name from .targets.json')
    parser.add_argument('extra_args', nargs='*', help='Extra arguments passed to the binary')
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.realpath(__file__))
    config = load_config(script_dir)

    if args.list:
        print_config_list(config)
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

    if target.startswith('android:'):
        run_android(args.device, device, args.preset, preset, args.extra_args, build_root, target_dir, dry_run=args.dry_run)
    else:
        run_linux(args.device, device, args.preset, preset, args.extra_args, script_dir, build_root, target_dir, dry_run=args.dry_run)


if __name__ == '__main__':
    main()
