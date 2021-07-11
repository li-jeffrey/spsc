# Shared memory single-producer single-consumer queue

Lockless implementation of a shared memory ring buffer, intended to be used as a single-producer single-consumer (SPSC) queue. Allows variable-length message sizes.

# Building
Publisher and subscriber examples can be found under [examples](examples/).

```
make
```

# Testing

```
make test
```

A test binary will be built in the lib/ folder.
