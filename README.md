# cson

`cson.h` is a stb-style C library for serializing and deserializing JSON.

## Quick Start

```console
$ cd examples
$ make
```

- serialization
```c
cson_node_t root = cson_create_object(NULL);
cson_append(&root, cson_create_string("name", "John Doe"));
cson_append(&root, cson_create_number("age", 28));
cson_write(&root, stdout):
```

```console
output:
{
    "name": "John Doe",
    "age": 28
}
```

- deserialization
```c
static const char *json_str = "{\n"
"  \"name\": \"Jane Smith\",\n"
"  \"age\": 32,\n"
"  \"occupation\": \"Software Engineer\",\n"
"  \"married\": true,\n"
"  \"skills\": [\"JavaScript\", \"Python\", \"C++\"]\n"
"}";

cson_node_t root = cson_load_buffer(json_str);
cson_node_t *age_node = cson_query(&root, "age");
printf("age: %d\n", (int) cson_to_number(age_node));
```

```console
output:
age: 32
```

## Reference

- [tsoding/jim](https://github.com/tsoding/jim)
