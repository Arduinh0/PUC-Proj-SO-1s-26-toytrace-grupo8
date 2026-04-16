import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def run_toytrace(args):
    proc = subprocess.run(
        [str(ROOT / "toytrace"), *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert proc.returncode == 0, proc.stderr
    return proc.stdout


def test_trace_hello_write_contains_write():
    out = run_toytrace(["trace", "--", "./tests/targets/hello_write"])
    assert "write(" in out
    assert "= " in out


def test_raw_events_show_entry_and_exit():
    out = run_toytrace(["trace", "--raw-events", "--", "./tests/targets/hello_write"])
    assert "entrada" in out
    assert "saida" in out


def test_trace_exec_echo_mentions_execve():
    out = run_toytrace(["trace", "--", "./tests/targets/exec_echo"])
    assert "execve(" in out
