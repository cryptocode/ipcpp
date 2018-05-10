#include "platform.hpp"
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void * ipcpp_allocate (const char * name, size_t size)
{
	// Unlinking before shm_open is required on macOS to get ftruncate
	// working consistently (??? the other process)
	shm_unlink (name);
	auto shm_fd = shm_open (name, O_CREAT | O_RDWR, 0666);
	std::cerr << "SHMID:" << shm_fd << std::endl;

	/* configure the size of the shared memory segment */
	if (ftruncate (shm_fd, size) == -1)
	{
		std::cerr << "ftruncate failed" << std::endl;
		return (void *)0;
	}

	void * ptr = mmap (0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, shm_fd, 0);
	if (ptr == MAP_FAILED)
	{
		std::cerr << "mmap failed" << std::endl;
		return (void *)0;
	}

	std::cerr << "mmap:" << ptr << std::endl;
	return ptr;
}

int ipcpp_destroy (const char * name)
{
	return shm_unlink (name);
}