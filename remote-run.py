#!/usr/bin/env python3
"""Run a built binary on a remote device using .targets.json configuration."""

import argparse
import glob
import json
import os
import shlex
import subprocess
import sys
import time

from textual import work
from textual.app import App, ComposeResult
from textual.widgets import RichLog, Rule, Markdown


def load_config(script_dir):
    config_path = os.path.join(script_dir, '.targets.json')
    with open(config_path) as f:
        return json.load(f)


def target_to_dir(target):
    """Convert 'platform:arch:sysroot' to 'platform-arch-sysroot'."""
    return target.replace(':', '-')


def expand_vars(value, scratchdir, build_dir):
    """Expand $SCRATCH_DIR and $BUILD_DIR in a string."""
    return value.replace('$SCRATCH_DIR', scratchdir).replace('$BUILD_DIR', build_dir)


def toptext_message(device):
    def _viewer():
        viewer = device.get('viewer', {})
        kind = viewer.get('type', '')
        address = viewer.get('address', '')
        if kind == 'web':
            return f"View the display at **{address}**"
        elif kind == 'scrcpy':
            return f"View the display by pointing **scrcpy** at **{address}**"
        elif kind:
            return f"View the display ({kind}) at **{address}**"
        return ""
    def _deploy():
        target = device.get('target', '<unknown>')
        hostname = device.get('hostname', '<unknown>')
        return f"Deployed **{target}** to **{hostname}**"
    return f"""{_deploy()}

{_viewer()}"""


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
    """
    BINDINGS = [("ctrl+c", "quit", "Quit")]

    def __init__(self, cmd, toptext_msg, on_quit=None):
        super().__init__()
        self.cmd = cmd
        self.toptext_msg = toptext_msg
        self._on_quit = on_quit
        self._proc = None

    def compose(self) -> ComposeResult:
        if self.toptext_msg:
            yield Markdown(self.toptext_msg, open_links=False)
            yield Rule()
        yield RichLog(highlight=False, markup=False)

    def on_mount(self) -> None:
        self._stream()

    @work(thread=True)
    def _stream(self) -> None:
        self._proc = subprocess.Popen(
            self.cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
        )
        log = self.query_one(RichLog)
        for line in self._proc.stdout:
            def write_line(l=line.rstrip()):
                log.auto_scroll = log.is_vertical_scroll_end
                log.write(l)
            self.call_from_thread(write_line)

    def action_quit(self) -> None:
        if self._proc:
            self._proc.terminate()
            self._proc.wait()
        if self._on_quit:
            self._on_quit()
        self.exit()


def stream_logcat(hostname, package, logcat_cmd, device):
    def cleanup():
        subprocess.run(['adb', '-s', hostname, 'shell', 'am', 'force-stop', package])
    OutputViewerApp(
        logcat_cmd, 
        toptext_message(device), 
        on_quit=cleanup
    ).run()


def find_linux_binary(build_root, target_dir, binary):
    path = os.path.join(build_root, target_dir, 'bin', binary)
    return path if os.path.isfile(path) else None


def find_android_apk(build_root, target_dir, package):
    apk_dir = os.path.join(build_root, target_dir, 'packaged', 'android-apk')
    matches = glob.glob(os.path.join(apk_dir, f'{package}*.apk'))
    return matches[0] if matches else None


def run_linux(device_name, device, preset_name, preset, extra_args, script_dir, build_root, target_dir):
    hostname = device['hostname']
    scratchdir = device.get('scratchdir')
    if not scratchdir:
        sys.exit(f"Error: device '{device_name}' is missing required 'scratchdir'")

    binary = preset['binary']
    binary_path = find_linux_binary(build_root, target_dir, binary)
    if not binary_path:
        sys.exit(
            f"Error: binary '{binary}' not found at multi_build/{target_dir}/bin/{binary}\n"
            "Did you run the build first?"
        )

    remote_binary = f"{scratchdir}/{binary}"
    build_dir = os.path.join(build_root, target_dir)

    def ev(s):
        return expand_vars(s, scratchdir, build_dir)

    # Ensure scratch directory exists
    subprocess.run(['ssh', hostname, f'mkdir -p {shlex.quote(scratchdir)}'], check=True)

    # Copy binary only if changed
    subprocess.run(
        ['rsync', '-av', '--checksum', '--chmod=+x', binary_path, f'{hostname}:{remote_binary}'],
        check=True,
    )

    # Sync data files
    for entry in preset.get('files', []):
        local_abs = ev(entry['local'])
        if not os.path.isabs(local_abs):
            local_abs = os.path.join(script_dir, local_abs)
        remote_path = ev(entry['remote'])
        subprocess.run(
            ['rsync', '-av', '--checksum', local_abs, f'{hostname}:{remote_path}'],
            check=True,
        )

    # Merge env: device env overridden by preset env
    merged_env = {**device.get('env', {}), **preset.get('env', {})}

    # Build remote shell command
    workdir = ev(preset.get('workdir', '$SCRATCH_DIR'))
    all_args = [ev(a) for a in preset.get('args', [])] + extra_args

    env_str = ' '.join(f'{k}={shlex.quote(str(v))}' for k, v in merged_env.items())
    cmd_parts = [shlex.quote(remote_binary)] + [shlex.quote(a) for a in all_args]
    if env_str:
        cmd_parts = [env_str] + cmd_parts
    remote_cmd = f'cd {shlex.quote(workdir)} && {" ".join(cmd_parts)}'

    OutputViewerApp(
        ['ssh', hostname, '--', remote_cmd], 
        toptext_message(device),
    ).run()


def run_android(device_name, device, preset_name, preset, extra_args, build_root, target_dir):
    hostname = device['hostname']
    package = preset.get('package')
    if not package:
        sys.exit(f"Error: preset '{preset_name}' is missing required 'package' for Android target")

    apk_path = find_android_apk(build_root, target_dir, package)
    if not apk_path:
        apk_dir = f"multi_build/{target_dir}/packaged/android-apk/"
        sys.exit(
            f"Error: no APK found for package '{package}' in {apk_dir}\n"
            "Did you run the build first?"
        )

    subprocess.run(['adb', '-s', hostname, 'install', '-r', apk_path], check=True)

    # Resolve the launcher activity so we can use am start (monkey doesn't support extras)
    result = subprocess.run(
        ['adb', '-s', hostname, 'shell', 'cmd', 'package', 'resolve-activity',
         '--brief', '-a', 'android.intent.action.MAIN',
         '-c', 'android.intent.category.LAUNCHER', package],
        capture_output=True, text=True, check=True,
    )
    component = result.stdout.strip().split('\n')[-1]

    # Merged extras: device env + preset env + preset android extras
    merged_extras = {
        **device.get('env', {}),
        **preset.get('env', {}),
        **preset.get('extras', {}),
    }

    # Clear log buffer before launch so we don't see stale output
    subprocess.run(['adb', '-s', hostname, 'logcat', '-c'], check=True)

    launch_cmd = ['adb', '-s', hostname, 'shell', 'am', 'start', '-n', component]
    for key, value in merged_extras.items():
        launch_cmd += ['--es', key, str(value)]
    subprocess.run(launch_cmd, check=True)

    # Poll for the app's PID so logcat can be filtered to this process
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

    logcat_cmd = ['adb', '-s', hostname, 'logcat']
    if pid:
        logcat_cmd += ['--pid', pid]
    else:
        print(f"Warning: could not resolve PID for {package}, logcat will be unfiltered",
              file=sys.stderr)

    stream_logcat(hostname, package, logcat_cmd, device)


def main():
    parser = argparse.ArgumentParser(
        description='Run a built binary on a remote device using .targets.json'
    )
    parser.add_argument('device', help='Device name from .targets.json')
    parser.add_argument('preset', help='Preset name from .targets.json')
    parser.add_argument('extra_args', nargs='*', help='Extra arguments passed to the binary')
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.realpath(__file__))
    config = load_config(script_dir)

    devices = config.get('devices', {})
    presets = config.get('presets', {})

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
        run_android(args.device, device, args.preset, preset, args.extra_args, build_root, target_dir)
    else:
        run_linux(args.device, device, args.preset, preset, args.extra_args, script_dir, build_root, target_dir)


if __name__ == '__main__':
    main()
