#pragma once

#include <betr/atomic.hpp>

extern betr::AtomicFlag running;

extern int load_context();
extern void unload_context();
extern void swap_buffers();
