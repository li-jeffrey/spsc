# Shared memory single-producer single-consumer queue

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
```

A test binary will be built in the [lib](lib/) folder.

# Performance test
```
make perf
```

The performance test involves timing how long it takes to publish/subscribe 100000000 messages in separate processes.
