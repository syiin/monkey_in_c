#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "custom_string.h"

#define INITIAL_CAPACITY 16

static void string_grow(string_t *str, size_t min_cap) {
    size_t new_cap = str->cap;
    while (new_cap < min_cap) {
        new_cap *= 2;
    }
    str->data = realloc(str->data, new_cap);
    str->cap = new_cap;
}

string_t *string_new(void) {
    string_t *str = malloc(sizeof(string_t));
    str->data = malloc(INITIAL_CAPACITY);
    str->len = 0;
    str->cap = INITIAL_CAPACITY;
    str->data[0] = '\0';
    return str;
}

string_t *string_from(const char *init) {
    size_t len = strlen(init);
    size_t cap = len + 1;
    if (cap < INITIAL_CAPACITY) {
        cap = INITIAL_CAPACITY;
    }
    string_t *str = malloc(sizeof(string_t));
    str->data = malloc(cap);
    str->len = len;
    str->cap = cap;
    strcpy(str->data, init);
    return str;
}

void string_free(string_t *str) {
    free(str->data);
    free(str);
}

void string_append(string_t *str, const char *append) {
    size_t append_len = strlen(append);
    size_t needed_cap = str->len + append_len + 1;
    if (needed_cap > str->cap) {
        string_grow(str, needed_cap);
    }
    strcpy(str->data + str->len, append);
    str->len += append_len;
}

string_t *string_concat(string_t *str1, string_t *str2){
    size_t concated_len = str1->len + str2->len + 1;
    string_t *concated = string_new();
    if (concated_len > concated->cap){
        string_grow(concated, concated_len);
    }
    strcpy(concated->data, str1->data);
    strcpy(concated->data + str1->len, str2->data);

    concated->len = concated_len - 1; //without NULL term
    return concated;
}

void string_append_char(string_t *str, char ch) {
    size_t needed_cap = str->len + 2; // +1 for char, +1 for null
    
    if (needed_cap > str->cap) {
        string_grow(str, needed_cap);
    }
    
    str->data[str->len] = ch;
    str->data[str->len + 1] = '\0';
    str->len++;
}

void string_clear(string_t *str) {
    str->len = 0;
    str->data[0] = '\0';
}

char *string_get_data(string_t *str) {
    char *data = str->data;
    free(str);
    return data;
}

size_t string_len(const string_t *str) {
    return str->len;
}

bool string_equals(const string_t *a, const string_t *b) {
    if (a->len != b->len) {
        return false;
    }
    return strcmp(a->data, b->data) == 0;
}

string_t *string_clone(const string_t *str) {
    string_t *clone = malloc(sizeof(string_t));
    clone->len = str->len;
    clone->cap = str->cap;
    clone->data = malloc(str->cap);
    strcpy(clone->data, str->data);
    return clone;
}

void string_print(const string_t *str, FILE *out) {
    fprintf(out, "%s", str->data);
}
