#define CSON_IMPLEMENTATION
#include "cson.h"

int main(void) {
    cson_node_t root = cson_load_file("test.json");
    cson_write(&root, stdout);
    cson_free(&root);
    // cson_node_t root = cson_create_object(NULL);
    // cson_append(&root, cson_create_number("id", 1));
    // cson_append(&root, cson_create_number("id", 1));
    // cson_append(&root, cson_create_number("id", 1));
    // cson_append(&root, cson_create_number("id", 1));
    // cson_write(&root, stdout);
    // cson_free(&root);
    return 0;
}
