# About

ipcpp is an IPC library written in and for C++. It supports shared memory and message queuing and can be built on Linux, macOS and Windows.

An STL compliant allocator is provided, allowing strings, vectors, maps and sets to be shared between processes.

## Raison d'être

Portable and robust IPC is *hard* and macOS is especially terrible. The posix implementation is not posix compliant, and openradar has numerous longstanding bugs related to poll, interprocess semaphores, etc. Windows is a world of its own.

While ipcpp is less feature rich than, say, Boost IPC, the goal is provide a useful set of features for IPC that works well on all three major platforms.

ipcpp offers precise out-of-memory handling. Any allocation exhausting the alloted shared memory segment will cause a std::bac_alloc to be thrown.

## ipcmalloc

ipcmalloc is a heap allocator made specifically for ipcpp. It is based on the well renowed dlmalloc, which is extended to deal with multiple shared memory segments.

Compared to dlmalloc, ipcmalloc:

* Allows multiple concurrent instances of the allocator. Each instance if configured with a `sbrk` callback. This is important to allow malloc'ing from different shared memory segments.
* Uses the sbrk/morecore mechanism even on Windows, which lowers to Win API mechanisms for getting anonymous shared memory mappings.
* Removes unnecessary functions.

# Testing

GTest from Google is used for unit tests. It is automatically downloaded and built when configuring ipcpp with CMake.

After building, run the test suites with ./tests or tests.exe

# Tools of the trade

## Linux

### Shared memory settings

```
ipcs -lm
```

### View and change max shared memory segment size

```
cat /proc/sys/kernel/shmmax

# Change shmmax without needing a reboot
echo 2147483648 > /proc/sys/kernel/shmmax

# Or with sysctl (setting does not survive reboot, see below)
sysctl -w kernel.shmmax=214748364

# Change permanently with sysctl
echo "kernel.shmmax=2147483648" >> /etc/sysctl.conf

```

### Show the contents of mapped memory
This only works if the library uses non-anonymous mappings through set_persistence(1)

```
ls -al /dev/shm
hexdump /dev/shm/<name>
```

### Memory info

```
vmstat -s
cat /proc/meminfo
sudo dmidecode -t 17
```