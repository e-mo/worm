#ifndef WORM_REPO_UTILITY_H
#define WORM_REPO_UTILITY_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include <git2.h>

static inline git_repository * get_current_repo(void) {
    git_repository *repo = nullptr;

    int git_err = git_repository_open_ext(&repo, ".", 0, nullptr); 

    if (git_err != 0)
        return nullptr;

    return repo;
}

// Get relative path from 'from' to 'to'
// Returns nullptr on failure, caller must free result
static inline char* get_relative_path(const char *from, const char *to) {
    char real_from[PATH_MAX];
    char real_to[PATH_MAX];
    
    // Assure both paths are absolute.
    if (!realpath(from, real_from) || !realpath(to, real_to)) {
        return nullptr;
    }
    
    // Check if 'to' is inside 'from'
    size_t from_len = strlen(real_from);
    if (strncmp(real_to, real_from, from_len) != 0) {
        return nullptr;  // Not a subdirectory
    }
    
    // Skip past the base path and leading slash
    const char *relative = real_to + from_len;
    while (*relative == '/') relative++;
    
    if (*relative == '\0') {
        return nullptr;  // Same directory
    }
    
    return strdup(relative);
}

// Check if pattern already exists in .gitignore
static inline bool gitignore_contains(const char *gitignore_path, const char *pattern) {
    FILE *f = fopen(gitignore_path, "r");
    if (!f) {
        return false;
    }
    
    char line[4096];
    bool found = false;
    
    while (fgets(line, sizeof(line), f)) {
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }
        
        // Trim trailing whitespace
        len = strlen(line);
        while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t')) {
            line[--len] = '\0';
        }
        
        if (strcmp(line, pattern) == 0) {
            found = true;
            break;
        }
    }
    
    fclose(f);
    return found;
}

// Add submodule path to repository's .gitignore
// submodule_path must be absolute path to the submodule
// Returns 0 on success, -1 on error
static inline int add_submodule_to_gitignore(git_repository *repo, const char *submodule_path) {
    assert(repo != nullptr && "repo is nullptr");
    assert(submodule_path != nullptr && "submodule_path is nullptr");
    
    // Get repository root directory
    const char *repo_workdir = git_repository_workdir(repo);
    if (!repo_workdir) {
        fprintf(stderr, "Repository has no working directory (bare repo?)\n");
        return -1;
    }
    
    // Get relative path from repo root to submodule
    char *relative_path = get_relative_path(repo_workdir, submodule_path);
    if (!relative_path) {
        fprintf(stderr, "Submodule is not inside repository\n");
        return -1;
    }
    
    // Build .gitignore path
    char gitignore_path[PATH_MAX];
    snprintf(gitignore_path, sizeof(gitignore_path), "%s/.gitignore", repo_workdir);
    
    // Check if already in .gitignore
    if (gitignore_contains(gitignore_path, relative_path)) {
        free(relative_path);
        return 0;
    }
    
    // Open .gitignore for appending
    FILE *f = fopen(gitignore_path, "a+");
    if (!f) {
        fprintf(stderr, "Failed to open .gitignore: %s\n", strerror(errno));
        free(relative_path);
        return -1;
    }
    
    // Check if file ends with newline
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    
    if (size > 0) {
        fseek(f, -1, SEEK_END);
        char last_char;
        if (fread(&last_char, 1, 1, f) == 1 && last_char != '\n') {
            fprintf(f, "\n");
        }
    }
    
    // Add the pattern
    fprintf(f, "%s", relative_path);
    printf("Added to .gitignore: %s\n", relative_path);

    fclose(f);
    free(relative_path);
    return 0;
}

#endif // WORM_REPO_UTILITY_H
