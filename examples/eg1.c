/// serialize

#define CSON_IMPLEMENTATION
#include "cson.h"

int main(void) {
    cson_node_t root = cson_create_object(NULL);

    cson_append(&root, cson_create_string("name", "John Doe"));
    cson_append(&root, cson_create_number("age", 28));
    cson_append(&root, cson_create_string("gender", "male"));
    cson_append(&root, cson_create_boolean("married", false));
    cson_append(&root, cson_create_null("email"));
    {
        cson_node_t address = cson_create_object("address");
        cson_append(&address, cson_create_string("city", "New York"));
        cson_append(&address, cson_create_string("street", "123 Main Street"));
        cson_append(&root, address);
    }
    {
        cson_node_t hobbies = cson_create_array("hobbies");
        cson_append(&hobbies, cson_create_string(NULL, "reading"));
        cson_append(&hobbies, cson_create_string(NULL, "coding"));
        cson_append(&root, hobbies);
    }

    cson_write(&root, stdout);
    cson_free(&root);
    return 0;
}
