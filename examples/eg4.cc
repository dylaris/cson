/// deserialize

#define CSON_IMPLEMENTATION
#include "cson.h"

int main() {
    cson_node_t root = cson_load_file("test.json");
    cson_write(&root, stdout);
    cson_free(&root);
    return 0;
}
