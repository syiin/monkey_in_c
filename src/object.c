#include "object.h"
#include "stdlib.h"
#include "string.h"
#include "custom_string.h"

object_t *object_heap_copy(const object_t *source) {
    object_t *new_obj = malloc(sizeof(object_t));
    if (!new_obj) return NULL;

    memcpy(new_obj, source, sizeof(object_t));

    if (source->type == OBJECT_ERROR && source->error_message) {
        new_obj->error_message = string_clone(source->error_message);
        if (!new_obj->error_message) {
            free(new_obj);
            return NULL;
        }
    }

    return new_obj;
}
