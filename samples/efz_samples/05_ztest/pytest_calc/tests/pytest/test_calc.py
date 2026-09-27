"""pytest-harness tests for the `add` shell command.

Twister builds + launches the firmware, then runs these tests. The `shell`
fixture (from twister_harness) sends commands to the device and returns the
output lines, so pass/fail is decided here in Python — not by ztest in C.
"""
from twister_harness import Shell


def test_add_positives(shell: Shell):
    output = "\n".join(shell.exec_command("add 2 3"))
    assert "5" in output, f"2 + 3 should be 5, got: {output!r}"


def test_add_negatives(shell: Shell):
    output = "\n".join(shell.exec_command("add -3 -4"))
    assert "-7" in output, f"-3 + -4 should be -7, got: {output!r}"
