"""Logging, process and log-polling helpers shared by every test."""

import contextlib
import os
import re
import signal
import socket
import subprocess
import sys
import time
from pathlib import Path

SRC_DIR = Path(__file__).resolve().parents[3]
TESTS_DIR = SRC_DIR / ".github" / "tests"
NET_DIR = TESTS_DIR / "net"
WEB_DIR = TESTS_DIR / "web"

IN_CI = os.environ.get("GITHUB_ACTIONS") == "true"


class TestFailure(Exception):
    """A test verdict, printed as FAIL and turned into exit code 1."""


class SetupError(Exception):
    """The test could not run at all; exit code 2."""


def log(msg=""):
    print(msg, flush=True)


@contextlib.contextmanager
def group(title):
    log(f"::group::{title}" if IN_CI else f"--- {title}")
    try:
        yield
    finally:
        log("::endgroup::" if IN_CI else "---")


def read_text(path):
    try:
        return Path(path).read_text(errors="replace")
    except OSError:
        return ""


def dump(title, path, tail=None):
    lines = read_text(path).splitlines()
    if tail:
        lines = lines[-tail:]
    with group(title):
        for line in lines:
            log(line)


def dump_matching(title, path, pattern):
    rx = re.compile(pattern)
    with group(title):
        for line in read_text(path).splitlines():
            if rx.search(line):
                log(line)


def log_has(path, needle):
    return needle in read_text(path)


class Proc:
    """A child in its own process group, with a wall-clock deadline.

    The group is the whole tree: `cb run` interposes a python process that a
    plain kill would stop while leaving the engine behind it holding a port.
    """

    TIMED_OUT = 124
    live = []

    def __init__(self, name, argv, log_path, env=None, timeout=None, cwd=None):
        self.name = name
        self.log_path = Path(log_path)
        self.log_path.parent.mkdir(parents=True, exist_ok=True)
        self.deadline = time.monotonic() + timeout if timeout else None
        self.timed_out = False
        with open(self.log_path, "wb") as out:
            self.popen = subprocess.Popen(
                [str(a) for a in argv],
                stdout=out,
                stderr=subprocess.STDOUT,
                stdin=subprocess.DEVNULL,
                env=env,
                cwd=cwd,
                start_new_session=True,
            )
        Proc.live.append(self)

    @property
    def pid(self):
        return self.popen.pid

    def alive(self):
        self._enforce_deadline()
        return self.popen.poll() is None

    def _enforce_deadline(self):
        if (
            self.deadline
            and self.popen.poll() is None
            and time.monotonic() > self.deadline
        ):
            self.timed_out = True
            self.kill()

    def wait(self):
        while self.alive():
            time.sleep(0.25)
        return Proc.TIMED_OUT if self.timed_out else self.popen.returncode

    def kill(self):
        if self.popen.poll() is not None:
            return
        try:
            os.killpg(self.popen.pid, signal.SIGTERM)
            self.popen.wait(5)
        except ProcessLookupError:
            return
        except subprocess.TimeoutExpired:
            with contextlib.suppress(ProcessLookupError):
                os.killpg(self.popen.pid, signal.SIGKILL)
            self.popen.wait()

    @classmethod
    def kill_all(cls):
        for proc in reversed(cls.live):
            proc.kill()
        cls.live.clear()


def wait_until(check, timeout, proc, what):
    """Polls check() once a second; fails if proc dies or time runs out."""
    end = time.monotonic() + timeout
    while time.monotonic() < end:
        if check():
            return
        if not proc.alive():
            raise TestFailure(f"{proc.name} exited before {what}")
        time.sleep(1)
    raise TestFailure(f"{what} did not happen within {timeout}s")


def wait_for_log(proc, needle, timeout, what=None):
    wait_until(
        lambda: log_has(proc.log_path, needle), timeout, proc, what or repr(needle)
    )


def wait_for_port(proc, port, timeout):
    def connectable():
        with socket.socket() as s:
            s.settimeout(1)
            return s.connect_ex(("127.0.0.1", port)) == 0

    wait_until(connectable, timeout, proc, f"{proc.name} opened port {port}")


def run_checked(argv, what, **kwargs):
    res = subprocess.run([str(a) for a in argv], **kwargs)
    if res.returncode != 0:
        raise SetupError(f"{what} failed (exit {res.returncode})")
    return res


def install_signal_cleanup():
    def on_signal(signum, _frame):
        Proc.kill_all()
        sys.exit(128 + signum)

    signal.signal(signal.SIGTERM, on_signal)
    signal.signal(signal.SIGINT, on_signal)
