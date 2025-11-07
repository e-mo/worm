#ifndef EMO_LAYER_STRING_H
#define EMO_LAYER_STRING_H

// string_utils.h
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct str str_t;
struct str {
    const char *buf;
    int32_t len;
};

typedef struct string string_t;
struct string {
    char *buf;   
    int32_t len;
    int32_t size;
};

// begin and end both inclusive indices. If both are the same, a slice of len 1 is created.
static inline str_t string_str(string_t s, int32_t begin, int32_t end) {
    assert(s.buf != nullptr && "string_t.buf == nullptr");
    assert(s.len > 0 && "string_t.len <= 0");
    
    // Normalize negative indices. 
    if (begin < 0)
        begin = s.len + begin;
    if (end < 0)
        end = s.len + end;

    // Bounds check on normalized indices.
    assert(begin >= 0 && begin < s.len && "begin index OOB");
    assert(end >= 0 && end < s.len && end >= begin && "end index OOB");
    
    return (str_t){
        .buf = s.buf + begin,
        .len = end - begin + 1,
    };
}

// Equality
static inline bool str_eq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static inline bool str_eq_ignore_case(const char *a, const char *b) {
    return strcasecmp(a, b) == 0;
}

// Prefix/Suffix
static inline bool str_starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

static inline bool str_ends_with(const char *str, const char *suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) {
        return false;
    }
    
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

// Contains
static inline bool str_contains(const char *str, const char *substr) {
    return strstr(str, substr) != NULL;
}

// Empty/Blank checks
static inline bool str_is_empty(const char *str) {
    return str == NULL || str[0] == '\0';
}

static inline bool str_is_blank(const char *str) {
    if (str == NULL) return true;
    
    while (*str) {
        if (!isspace((unsigned char)*str)) {
            return false;
        }
        str++;
    }
    return true;
}

#endif // EMO_LAYER_STRING_H
