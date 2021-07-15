# Shared memory single-producer single-consumer queue

[![C/C++ CI](https://github.com/li-jeffrey/spsc/actions/workflows/c-cpp.yml/badge.svg?branch=master)](https://github.com/li-jeffrey/spsc/actions/workflows/c-cpp.yml)

Lockless implementation of a shared memory ring buffer, intended to be used as a single-producer single-consumer (SPSC) queue. Allows variable-length message sizes.


# Building

Run the following command to build a static libary.

```
make
```

Publisher and subscriber examples can be found under [examples](examples/). To build them, run the following command:

```
make examples
```

# Testing

```
make test
./lib/spsc_test
```

# Performance test (throughput)
```
make perf
./lib/spsc_perf
```

The performance test involves timing how long it takes to publish/subscribe 500 million messages in separate processes. During this test, the producer will sleep for 1 micro if the queue is saturated.

## Results

Taking the best of 3 runs, roughly 29M ops/second have been recorded on an Intel i5-8265U CPU (WSL2 Debian).

