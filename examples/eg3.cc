/// serialize

#define CSON_IMPLEMENTATION
#include "cson.h"

int main() {
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
        {
            cson_node_t coord = cson_create_object("coordinates");
            cson_append(&coord, cson_create_number("lat", 37.7749));
            cson_append(&coord, cson_create_number("lng", -122.4194));
            cson_append(&address, coord);
        }
        cson_append(&root, address);
    }
    {
        cson_node_t skills = cson_create_array("skills");
        {
            cson_node_t skill1 = cson_create_object(NULL);
            cson_append(&skill1, cson_create_string("name", "C++"));
            cson_append(&skill1, cson_create_number("level", 5));
            cson_append(&skills, skill1);
        }
        {
            cson_node_t skill2 = cson_create_object(NULL);
            cson_append(&skill2, cson_create_string("name", "Python"));
            cson_append(&skill2, cson_create_number("level", 4));
            cson_append(&skills, skill2);
        }
        cson_append(&root, skills);
    }

    cson_generate_file(&root, "person.json");
    cson_free(&root);
    return 0;
}
