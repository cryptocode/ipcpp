#pragma once

#include <cstddef>

void * ipcpp_allocate (const char * name, size_t size);
int ipcpp_destroy (const char * name);