# pytest-calc

The companion sample for the **pytest on Zephyr** guide
([Part 2 — pytest on Zephyr](https://huunghiaspkt.github.io/docs/zephyr-training/testing/pytest)).

The same `add()` as `ztest_calc`, but tested a different way: instead of `zassert`
in C, the firmware exposes an `add` **shell command** and a **pytest** test on the
host drives it and checks the output. Runs on `native_sim` — no hardware required.

## ztest vs. pytest harness

| | `ztest_calc` | `pytest_calc` (this) |
|---|---|---|
| Test lives in | C (`zassert_*`) | Python (`assert`) |
| Runs | inside the firmware | on the host, talking to the firmware |
| Good for | unit-testing C functions | interaction tests: shell, serial, OTA, networking |

## Layout

```
pytest_calc/
├── include/calc.h          # function under test (header)
├── src/calc.c              # function under test (impl)
└── tests/
    ├── CMakeLists.txt      # builds main.c + calc.c
    ├── prj.conf            # CONFIG_SHELL=y + bind UART to stdin/stdout
    ├── testcase.yaml       # harness: pytest
    ├── main.c              # registers the `add` shell command
    └── pytest/
        └── test_calc.py    # the actual tests (Python)
```

## Run it

From this directory, with a Zephyr environment activated (`ZEPHYR_BASE` set) and
`pytest` available:

```bash
west twister -p native_sim -v -n -T tests/
```

Expected:

```text
INFO - 1/1 native_sim/native    calc.testing.pytest    PASSED (native 0.42s)
INFO - 2 of 2 executed test cases passed (100.00%) ...
```

## Where pytest's own report lives

Twister wraps pytest, so there are two report layers. Twister's roll-up is
`twister-out/twister.xml`; pytest's own artifacts are deeper in the build tree:

```bash
find twister-out -name twister_harness.log   # pytest console (test session starts … N passed)
find twister-out -name report.xml            # pytest junit-xml report
```

## Try a failure

In `tests/pytest/test_calc.py`, change the expected value and re-run — pytest
reports the failing assertion with the exact line and the captured shell output:

```python
def test_add_positives(shell: Shell):
    output = "\n".join(shell.exec_command("add 2 3"))
    assert "6" in output   # it's 5 — this fails
```

Revert it to go green again.
