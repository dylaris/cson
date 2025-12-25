/// deserialize

#define CSON_IMPLEMENTATION
#include "cson.h"

static const char *json_str = "{\n"
"  \"name\": \"Jane Smith\",\n"
"  \"age\": 32,\n"
"  \"occupation\": \"Software Engineer\",\n"
"  \"married\": true,\n"
"  \"skills\": [\"JavaScript\", \"Python\", \"C++\"]\n"
"}";

int main(void) {
    cson_node_t root = cson_load_buffer(json_str);

    cson_node_t *age_node = cson_query(&root, "age");
    printf("age: %d\n", (int) cson_to_number(age_node));

    cson_node_t *skills_node = cson_query(&root, "skills");
    cson_nodes_t skills = cson_to_array(skills_node);
    for (size_t i = 0; i < skills.len; i++) {
        printf("skill[%zu]: %s\n", i, cson_to_string(&skills.items[i]));
    }

    cson_free(&root);
    return 0;
}
