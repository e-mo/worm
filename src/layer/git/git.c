#include "layer/git.h"

static int _libgit2_shutdown(bool all);
static int _libgit2_shutdown_all(void);

const _git_interface Git = {
  // GLOBAL
  .init = git_libgit2_init,
  .shutdown = _libgit2_shutdown,

  // REPOSITORY
  .Repository = {
    .free = git_repository_free,
  },

  // CLONE
  .clone = git_clone,
  .CloneOpts = {
    .init = GIT_CLONE_OPTIONS_INIT,
  },
};

static int _libgit2_shutdown(bool all) {
  if (all)
    return _libgit2_shutdown_all();
  else
    return git_libgit2_shutdown();
}

static int _libgit2_shutdown_all(void) {
  int ret;
  while ((ret = git_libgit2_shutdown()) > 0);

  return ret;
}

// CLEANUP helper functions.
void repository_free(Git_Repository **repo) {
  Git.Repository.free(*repo);
  *repo = nullptr;
}
