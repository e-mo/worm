#ifndef LAYER_COMPAT_GIT_H
#define LAYER_COMPAT_GIT_H

#include <git2.h>
// Anonymous type never meant to be instanced.
// Allows for a nicer, more modern syntax (IMO).
// I will have to explore how well the compiler does inlining these functions.
// Hopefully the const suggestion helps. It feels like something c23 constexpr should be
// able to do, but I have yet to make that work correctly.
// TODO: Document error handling interface right here first. V important.
 
// Typedef some things for style consistency and decoupling.
typedef git_repository Git_Repository; 
typedef git_clone_options Git_CloneOpts;

typedef struct {

  // GLOBAL
  // Initialize libgit2. Returns the count of initializations.
  int (*const init)(void);
  // Shutdown libgit2. Returns number of initializations remaining to shut down. Will free
  // all resournces when all initializations have been shut down.
  // Accepts boolean argument 'all' and if true, calls itself repeatedly until all initializations
  // have been shut down or an error occurs. If false, bahaves like git_libgit2_shutdown, returning
  // the number of remaining initializations to be shut down.
  int (*const shutdown)(bool all);

  // REPOSITORY
  struct {
    void (*free)(Git_Repository *repo);
  } Repository;

  // CLONE
  int (*const clone)(Git_Repository **repo, const char *remote, const char *local, const Git_CloneOpts *opts);
  // CloneOpts
  struct {
    const Git_CloneOpts init;  
  } CloneOpts;

} _git_interface;
// Access to the interface.
extern const _git_interface Git;

// CLEANUP helper functions.
void repository_free(Git_Repository **repo);

#endif // LAYER_COMPAT_GIT_H
