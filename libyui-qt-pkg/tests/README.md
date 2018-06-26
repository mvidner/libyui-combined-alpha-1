
# Experimental Unit Tests

This is experimental proof of concept for unit tests.

### Running in Headless Mode

To run the unit tests in headless mode use this command:

```shell
xvfb-run --server-args="-screen 0 1024x768x24" make -C build test
```