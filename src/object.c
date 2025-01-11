#include "object.h"
#include "ast.h"
#include "stdlib.h"
#include "string.h"
#include "custom_string.h"

void inspect_object(object_t object, char *buff_out){
    switch(object.type){
        case OBJECT_INTEGER:
            snprintf(buff_out, BUFSIZ, "%d\n", object.integer);
            break;
        case OBJECT_BOOLEAN:
            snprintf(buff_out, BUFSIZ, "%s\n", object.boolean ? "true" : "false");
            break;
        case OBJECT_NULL:
            snprintf(buff_out, BUFSIZ, "NULL\n");
            break;
    case OBJECT_FUNCTION:{
       string_t *params = string_new();
       for (int i = 0; i < object.function.parameters->count; i++) {
           identifier_t *param = object.function.parameters->data[i];
           string_append(params, param->value);
           if (i < object.function.parameters->count - 1) {
               string_append(params, ", ");
           }
       }

       string_t *body_str = string_new();
       format_block_statement(body_str, object.function.body);

       snprintf(buff_out, BUFSIZ, "fn(%s) %s", params->data, body_str->data);

       string_free(params);
       string_free(body_str);
    }
   break;
        default:
            snprintf(buff_out, BUFSIZ, "Unknown object type\n");
            break;
    }
}

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
