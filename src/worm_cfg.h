#ifndef WORM_CFG_H
#define WORM_CFG_H

#include <git2.h>

#include <stddef.h>
#include <limits.h>

#define CLEANUP(F) __attribute__((cleanup(F)))

constexpr const char WORM_CFG_FILENAME[] = "worm.toml";
constexpr ptrdiff_t WORM_CFG_STACK_MAX = 256;
constexpr ptrdiff_t WORM_CFG_NAME_MAX = 256;

constexpr int SSH_KEY_PASSPHRASE_MAX = 256;

typedef struct _ssh_cfg _ssh_cfg;
struct _ssh_cfg {
    char public[PATH_MAX];
    char private[PATH_MAX];
    char passphrase[SSH_KEY_PASSPHRASE_MAX];
}; 

typedef struct worm_cfg worm_cfg;
struct worm_cfg {
    // char name[WORM_CFG_NAME_MAX];    
    char remote[PATH_MAX];
    char local[PATH_MAX];
    char root[PATH_MAX];

    struct _ssh_cfg ssh;

    git_clone_options gco;
};

typedef struct worm_cfg_stack worm_cfg_stack;
struct worm_cfg_stack {
    worm_cfg buf[WORM_CFG_STACK_MAX];
    ptrdiff_t index;
};

int worm_cfg_ssh_auth_cb(git_credential **cred, const char *url, const char *username, unsigned int at, void *payload);

bool worm_cfg_stack_push(worm_cfg_stack *restrict stack, worm_cfg *restrict config);

worm_cfg worm_cfg_stack_pop(worm_cfg_stack *stack);

bool worm_cfg_stack_is_empty(worm_cfg_stack *stack);

int worm_cfg_file_parse(const char *root, worm_cfg_stack *stack);

#endif // WORM_CFG_H
