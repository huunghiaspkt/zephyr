# ztest-calc

The companion sample for the **ZTest for Beginners** guide
([The Mental Model](https://huunghiaspkt.github.io/docs/zephyr-training/testing/ztest/concepts) ·
[Your First Test](https://huunghiaspkt.github.io/docs/zephyr-training/testing/ztest/first-test)).

A tiny `add()` function with a ztest suite, built and run on `native_sim` — no
hardware required.

## Layout

```
ztest_calc/
├── include/calc.h          # function under test (header)
├── src/calc.c              # function under test (impl)
└── tests/
    ├── CMakeLists.txt      # tells Zephyr what to build
    ├── prj.conf            # turns ztest on (CONFIG_ZTEST=y)
    ├── testcase.yaml       # tells Twister where & how to run
    └── test_calc.c         # the actual tests
```

## Run it

From this directory, with a Zephyr environment activated (`ZEPHYR_BASE` set):

```bash
west twister -v -n -T tests/
```

Expected:

```text
INFO - 1/1 native_sim    calc.testing.ztest    PASSED (native 0.005s)
INFO - 1 of 1 test configurations passed (100.00%), 0 failed ...
```

Want the raw ztest console (`START` / `PASS` lines)? Run the built binary:

```bash
# the exact path depends on your workspace, so just locate and run it:
$(find twister-out -name zephyr.exe)
```

List tests without running them:

```bash
west twister --list-tests -T tests/
```

## Try a failure

Add this to `tests/test_calc.c` and re-run — Twister reports `FAILED rc=1`
and the binary prints the exact `file:line` of the bad assertion:

```c
ZTEST(calc_tests, test_broken)
{
	zassert_equal(6, add(2, 3), "pretend 2+3==6");  /* it's 5 — this fails */
}
```

Delete it to go green again.
