#ifndef CUSTOM_STRING_H
#define CUSTOM_STRING_H

#include <stdio.h>
#include <stdbool.h>
/*Could have used the Vector class but the void * type would have precluded us from*/
/*conveniently using alot of convenient string things - currently using this just for printing, may extend to hold more char * things like token literals*/

typedef struct String {
    char *data;
    size_t len;
    size_t cap;
} string_t;

// Constructor/destructor
string_t *string_new(void);
string_t *string_from(const char *str);
void string_free(string_t *str);

// Core operations
void string_append(string_t *str, const char *append);
void string_append_char(string_t *str, char ch);
void string_clear(string_t *str);
string_t *string_concat(string_t *str1, string_t *str2);
char *string_get_data(string_t *str); //Transferring ownership

// Utility methods
size_t string_len(const string_t *str);
bool string_equals(const string_t *a, const string_t *b);
string_t *string_clone(const string_t *str);
void string_print(const string_t *str, FILE *out);

#endif
