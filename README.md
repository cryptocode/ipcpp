# About

ipcpp is an IPC library written in and for C++. It supports shared memory and message queuing and can be built on Linux, macOS and Windows.

An STL compliant allocator is provided, allowing strings, vectors, maps and sets to be shared between processes.

## ipcmalloc

srmalloc is a heap allocator based on dlmalloc. The changes are as follows:

* It supports multiple `sbrk` callbacks. This is important to allow malloc'ing from different shared memory segments.
* It removes unnecessary functions.

# Testing

GTest from Google is used for unit tests. It is automatically downloaded and built when configuring nanocate with CMake.

After building, run the test suites with ./tests or tests.exe

# Tools of the trade

## Linux

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
```