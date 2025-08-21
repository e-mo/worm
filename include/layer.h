#ifndef LAYER_COMPAT_LIB_H
#define LAYER_COMPAT_LIB_H

#define DEFER(F) __attribute__((cleanup(F)))

#include "layer/types.h"
#include "layer/git.h"
#include "layer/toml.h"

#endif // LAYER_COMPAT_LIB_H

