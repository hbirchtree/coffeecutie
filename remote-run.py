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


def print_viewer_info(device):
    viewer = device.get('viewer')
    if not viewer:
        return
    kind = viewer.get('type')
    address = viewer.get('address', '')
    if kind == 'web':
        print(f"View the display at {address}")
    elif kind == 'scrcpy':
        print(f"View the display by pointing scrcpy at {address}")
    else:
        print(f"View the display ({kind}) at {address}")


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

    print_viewer_info(device)
    subprocess.run(['ssh', '-t', hostname, '--', remote_cmd], check=True)


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

    print_viewer_info(device)

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

    logcat_proc = subprocess.Popen(logcat_cmd)
    try:
        logcat_proc.wait()
    except KeyboardInterrupt:
        pass
    finally:
        logcat_proc.terminate()
        logcat_proc.wait()
        subprocess.run(['adb', '-s', hostname, 'shell', 'am', 'force-stop', package])


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
