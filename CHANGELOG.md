- 1.4.4
    - Update of test dependencies, not code or functional changes.

- 1.4.3
    - Native code clean-up and optimizations, slightly improved hashid encoding and unique ID generation performance.
    - Fix bug where hashidEncode would cause segfault if non-array argument was passed.

- 1.4.0
    - No API changes. Changed dev dependency to use bn.js instead of bignum. confirmed compatibility with node 0.11, 0.12, iojs 2.5, 3

- 1.3.3
    - No API changes. Added benchmarking code and made it run as part of CI. Minor improvements to README.md file.

- 1.3.2
    - No impact on actual functionality, use steady_clock vs system_clock, cleanup, native code improvements, added one more test, example code improvements

- 1.2.4
    - No impact on actual functionality, improved C++ code and updated README with two additional API calls

- 1.2.2
    - No impact on actual functionality, added examples and reworked unit tests

- 1.2.0
    - A lot of fixes and test additions, also API breaking change: custom_epoch is expecting milliseconds instead of seconds

- 1.1.0
    - Initial public release
