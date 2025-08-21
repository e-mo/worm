#include <stdio.h>
#include <libgen.h>
#include <limits.h>

#include "layer.h"

enum worm_errors {
  WORM_OK = 0,
  WORM_ERROR_NO_CFG, 
  WORM_ERROR_CFG_OVERFLOW,
  WORM_ERROR_CFG_PARSE,
  WORM_ERROR_CFG_MODULE_PARSE,
  WORM_ERROR_GIT,
};

void print_git_error(int error) {
  const git_error *e = git_error_last();
  fprintf(stderr, "libgit2: error\n\t%d/%d\n\t%s\n", error, e->klass, e->message);
}

void file_cleanup(FILE **fp) {
  if (*fp != nullptr)
    fclose(*fp);
} 

int credentials_cb(git_credential **cred, const char *url, const char *username, unsigned int at, void *payload) {
  (void)url;
  (void)at;


  const char *ssh_key_path = payload;

  // Construct public key path (assuming .pub extension)
  char public_key_path[NAME_MAX];
  snprintf(public_key_path, sizeof(public_key_path), "%s.pub", ssh_key_path);
  
  return git_credential_ssh_key_new(cred, username, public_key_path, ssh_key_path, NULL);
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  // Open worm.toml

  FILE *worm_fd DEFER(file_cleanup) = fopen("worm.toml", "r");
  if (!worm_fd) return WORM_ERROR_NO_CFG; 

  constexpr size CFG_BUF_SIZE = 4096;
  char worm_cfg[CFG_BUF_SIZE] = {0};

  size buf_remaining = CFG_BUF_SIZE;
  char *cfg_p = worm_cfg;
  while (!feof(worm_fd) || buf_remaining <= 0) {
    usize bytes = fread(cfg_p, sizeof(*cfg_p), buf_remaining, worm_fd);
    cfg_p += bytes;
    buf_remaining -= bytes;
  } 

  if (!feof(worm_fd)) return WORM_ERROR_CFG_OVERFLOW;

  // File opened!

  char toml_err[200];
  toml_table_t *tbl = toml_parse(worm_cfg, toml_err, sizeof(toml_err));
  if (!tbl) {
    fprintf(stderr, "toml-c: error > %s\n", toml_err);
    return WORM_ERROR_CFG_PARSE; 
  }

  int git_err;
  if ((git_err = Git.init()) < 0) {
    print_git_error(git_err);
    return WORM_ERROR_GIT;
  }

  toml_array_t *module_array = toml_table_array(tbl, "modules");
  if (!module_array) return WORM_ERROR_CFG_PARSE;
  int num_modules = toml_array_len(module_array);  
  for (int i = 0; i < num_modules; i++) {
    toml_table_t *module = toml_array_table(module_array, i);
    if (!module) return WORM_ERROR_CFG_PARSE;

    toml_value_t name = toml_table_string(module, "name");
    if (!name.ok) return WORM_ERROR_CFG_MODULE_PARSE;
    toml_value_t remote = toml_table_string(module, "remote");
    if (!remote.ok) return WORM_ERROR_CFG_MODULE_PARSE;
    toml_value_t local = toml_table_string(module, "local");
    if (!local.ok) return WORM_ERROR_CFG_MODULE_PARSE;
    toml_value_t ssh = toml_table_string(module, "ssh");

    printf("name: %s\nremote: %s\nlocal: %s\n", name.u.s, remote.u.s, local.u.s);

    Git_CloneOpts c_opts = Git.CloneOpts.init;
    Git_Repository *repo DEFER(repository_free) = nullptr;

    while ((git_err = Git.clone(&repo, remote.u.s, local.u.s, &c_opts)) < 0) {
      const git_error *last = git_error_last();

      // Remote is asking for SSH auth.
      if (git_err == GIT_EAUTH && last->klass == GIT_ERROR_SSH) {
        c_opts.fetch_opts.callbacks.credentials = credentials_cb;
        c_opts.fetch_opts.callbacks.payload = ssh.ok ? ssh.u.s : nullptr;

        print_git_error(git_err);
        continue;
      }
      
      print_git_error(git_err);
      break;
    }
  }

  // true  = shut down all inits.
  // false = shut down 1 init (classic libgit2 behavior).
  if ((git_err = Git.shutdown(true)) < 0) {
    print_git_error(git_err);
    return WORM_ERROR_GIT;
  }

  return 0;
}
