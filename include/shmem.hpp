#pragma once

#include <cstddef>

void * nanocate_allocate (const char * name, size_t size);
int nanocate_destroy (const char * name);