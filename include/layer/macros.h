#ifndef EMO_MACROS_COMMON_H
#define EMO_MACROS_COMMON_H

#define CLEANUP(F) __attribute__((cleanup(F)))

// TODO: Create central location for ERR_FD setting that is thread safe.
// TODO: Remove all ERR_FD uses.
#define ERR_FD stderr

#endif // EMO_MACROS_COMMON_H
