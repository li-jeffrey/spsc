# Shared memory single-producer single-consumer queue

[![C/C++ CI](https://github.com/li-jeffrey/spsc/actions/workflows/c-cpp.yml/badge.svg?branch=master)](https://github.com/li-jeffrey/spsc/actions/workflows/c-cpp.yml)
[![Generate docs](https://github.com/li-jeffrey/spsc/actions/workflows/doxygen.yml/badge.svg)](https://github.com/li-jeffrey/spsc/actions/workflows/doxygen.yml)

Lockless implementation of a shared memory ring buffer, intended to be used as a single-producer single-consumer (SPSC) queue. Allows variable-length message sizes.

Full API documentation can be found [here](https://li-jeffrey.github.io/spsc/).

## Building

Run the following command to build a static libary.

```
make
```

Publisher and subscriber examples can be found under [examples](examples/). To build them, run the following command:

```
make examples
```

## Testing

```
make test
```

This creates a test binary at `./lib/spsc_test`.

## Performance tests
```
make perf
```

### ./lib/perf_throughput

The throughput test times how long it takes to publish/subscribe 500 million messages in separate processes. During this test, the producer will sleep for 1 micro if the queue is full.

### ./lib/perf_latency

The latency test sets up two ring buffers and measures the round trip time for 500,000 messages. To make sure the queue does not get saturated, there is a sleep time of 1 microsecond between the writes.

## Results

The following results are recorded on an Intel i5-8265U CPU running WSL2 Debian, taking the best of 3 runs.

|Test type|Result|
|-|-|
|Throughput|29,794,176 ops/s|
|Latency (p50)|466 ns|
|Latency (p75)|487 ns|
|Latency (p90)|520 ns|
|Latency (p95)|547 ns|
|Latency (p99)|836 ns|

