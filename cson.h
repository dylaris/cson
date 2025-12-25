/*
cson.h - v0.01 - Dylaris 2025
===================================================

BRIEF:
  A C library for serializing and deserializing json.

NOTE:
  Currently, this library assumes the JSON input is well-formed.
  Invalid input will cause the program to abort, and duplicate keys are not handled.

USAGE:
  In exactly one source file, define the implementation macro
  before including this header:
  ```
    #define CSON_IMPLEMENTATION
    #include "cson.h"
  ```
  In other files, just include the header without the macro.

EXAMPLE:
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

LICENSE:
  See the end of this file for further details.
*/

#ifndef CSON_H
#define CSON_H

#ifndef CSONDEF
#define CSONDEF
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cson_node cson_node_t;

typedef struct {
    cson_node_t *items;
    size_t len;
    size_t cap;
} cson_nodes_t;

typedef enum {
    CSON_NULL,
    CSON_BOOLEAN,
    CSON_NUMBER,
    CSON_STRING,
    CSON_ARRAY,
    CSON_OBJECT
} cson_node_kind_t;

struct cson_node {
    cson_node_kind_t kind;
    char *key;
    union {
        bool boolean;
        double number;
        char *string;
        cson_nodes_t container;
    } as;
};

typedef enum {
    CSON_TK_LCURLY, CSON_TK_RCURLY,
    CSON_TK_LSQUARE, CSON_TK_RSQUARE,
    CSON_TK_COMMA, CSON_TK_COLON,
    CSON_TK_TRUE, CSON_TK_FALSE,
    CSON_TK_STRING, CSON_TK_NUMBER,
    CSON_TK_NULL, CSON_TK_EOF
} cson_token_kind_t;

typedef struct {
    cson_token_kind_t kind;
    const char *start;
    size_t len;
} cson_token_t;

typedef struct {
    const char *start;
    const char *current;
} cson_lexer_t;

// cson_append - append item to node
// @node: must be object or array
// @item: new item
CSONDEF void cson_append(cson_node_t *node, cson_node_t item);

// cson_remove_with_key - remove item from node on key
// @node: must be object
// @key: removed node key (should not be NULL)
// Note: uses swap-and-pop deletion, which does not preserve element order
CSONDEF void cson_remove_with_key(cson_node_t *node, const char *key);

// cson_remove_with_idx - remove item from node on index
// @node: must be array
// @key: removed node index
// Note: uses swap-and-pop deletion, which does not preserve element order
CSONDEF void cson_remove_with_idx(cson_node_t *node, size_t idx);

// cson_remove_all - remove all item from node
// @node: must be object or array
CSONDEF void cson_remove_all(cson_node_t *node);

// cson_create_xxx - create a xxx node
CSONDEF cson_node_t cson_create_object(const char *key);
CSONDEF cson_node_t cson_create_array(const char *key);
CSONDEF cson_node_t cson_create_number(const char *key, double value);
CSONDEF cson_node_t cson_create_boolean(const char *key, bool value);
CSONDEF cson_node_t cson_create_string(const char *key, const char *value);
CSONDEF cson_node_t cson_create_null(const char *key);

// cson_load_buffer - load the json string from buffer
// @buffer: json string (should not be NULL)
// Note: the input json must be valid
// Return: root node (always valid)
CSONDEF cson_node_t cson_load_buffer(const char *buffer);

// cson_load_file - load the json file
// @path: json file path (should not be NULL)
// Note: the input json must be valid
// Return: root node (always valid)
CSONDEF cson_node_t cson_load_file(const char *path);

// cson_write - output the nodes tree into file pointer
// @root: root node (should be a object node)
// @f: output file pointer
CSONDEF void cson_write(const cson_node_t *root, FILE *f);

// cson_generate_file - output to a file (create if not exists)
// @root: root node (should be a object node)
// @path: file path
CSONDEF void cson_generate_file(const cson_node_t *root, const char *path);

// cson_free - free the memory
// @root: root node
CSONDEF void cson_free(cson_node_t *root);

// cson_query - get the node pointer with key
// @root: root node (should be object)
// @key: member key (should not be NULL)
// Note: only query one layer, and just return the first matched result
// Return: the node pointer, NULL if not exists
CSONDEF cson_node_t *cson_query(const cson_node_t *root, const char *key);

// cson_to_xxx - get the node value
CSONDEF cson_nodes_t cson_to_object(const cson_node_t *node);
CSONDEF cson_nodes_t cson_to_array(const cson_node_t *node);
CSONDEF double cson_to_number(const cson_node_t *node);
CSONDEF bool cson_to_boolean(const cson_node_t *node);
CSONDEF const char *cson_to_string(const cson_node_t *node);

#ifdef __cplusplus
}
#endif

#endif // CSON_H

#ifdef CSON_IMPLEMENTATION

#define cson__fatal(fmt, ...)                                                 \
    do {                                                                      \
        fprintf(stderr, "%s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        abort();                                                              \
    } while (0)

CSONDEF void cson_append(cson_node_t *node, cson_node_t item) {
    if (node->kind != CSON_OBJECT && node->kind != CSON_ARRAY) {
        cson__fatal("node should be object or array type");
    }
    cson_nodes_t *da = &node->as.container;
    if (da->len + 1 > da->cap) {
        da->cap = da->cap < 16 ? 16 : 2*da->cap;
        da->items = (cson_node_t *) realloc(da->items, sizeof(cson_node_t)*da->cap);
        if (!da->items) cson__fatal("out of memory");
    }
    da->items[da->len++] = item;
}

// read_file - read the file content
// @path: file path (should not be null)
// Return: dynamically allocated buffer of file content
static char *cson__read_file(const char *path) {
    FILE *f = NULL;
    char *buffer = NULL;
    long size;

    f = fopen(path, "r");
    if (!f) goto fail;

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);
    if (size <= 0) goto fail;

    buffer = (char *) malloc(size);
    if (!buffer) goto fail;
    if (fread(buffer, size, 1, f) != 1) goto fail;

    fclose(f);
    return buffer;
fail:
    if (f) fclose(f);
    if (buffer) free(buffer);
    return NULL;
}

// cson__make_punc - make a punctuation token
// @lex: pointer to lexer
// @kind: token type
// Return: a punctuation (length = 1) token
static cson_token_t cson__make_punc(cson_lexer_t *lex, cson_token_kind_t kind) {
    return (cson_token_t) {
        .kind = kind,
        .start = lex->current++,
        .len = 1
    };
}

// cson__make_string - make string token
// @lex: pointer to lexer
// Note: assume the string has the balanced '"'
// Return: a string (exclude '"') token
static cson_token_t cson__make_string(cson_lexer_t *lex) {
    lex->current++; // skip open '"'
    while (*lex->current != '"') lex->current++;
    lex->current++; // skip closed '"'
    return (cson_token_t) {
        .kind = CSON_TK_STRING,
        .start = lex->start + 1,
        .len = (size_t) (lex->current - lex->start - 2)
    };
}

// cson__make_number - make number token
// @lex: pointer to lexer
// Return: a number (include '.') token
static cson_token_t cson__make_number(cson_lexer_t *lex) {
    while (isdigit(*lex->current)) lex->current++;
    if (*lex->current != '.') goto exit;
    lex->current++; // skip '.'
    while (isdigit(*lex->current)) lex->current++;
exit:
    return (cson_token_t) {
        .kind = CSON_TK_NUMBER,
        .start = lex->start,
        .len = (size_t) (lex->current - lex->start)
    };
}

// cson__make_literal - make literal token
// @lex: pointer to lexer
// Note: only support null/true/false
// Return: a literal token
static cson_token_t cson__make_literal(cson_lexer_t *lex) {
    cson_token_t token;
    switch (*lex->start) {
    case 'f': token.kind = CSON_TK_FALSE; lex->current += 5; break;
    case 'n': token.kind = CSON_TK_NULL;  lex->current += 4; break;
    case 't': token.kind = CSON_TK_TRUE;  lex->current += 4; break;
    }
    token.start = lex->start;
    token.len = lex->current - lex->start;
    return token;
}

// cson__make_eof - make a eof token
// @lex: pointer to lexer
// Return: a eof token
static cson_token_t cson__make_eof(cson_lexer_t *lex) {
    (void) lex;
    return (cson_token_t) {
        .kind = CSON_TK_EOF,
        .start = lex->start,
        .len = 1
    };
}

static cson_token_t cson__get_next_token(cson_lexer_t *lex) {
    // skip whitespace
    while (isspace(*lex->current)) lex->current++;
    lex->start = lex->current;

    char ch = *lex->start;
    if (ch == '\0')  return cson__make_eof(lex);
    if (isdigit(ch)) return cson__make_number(lex);
    if (ch == '"')   return cson__make_string(lex);
    if (isalpha(ch)) return cson__make_literal(lex);
    switch (ch) {
    case '{': return cson__make_punc(lex, CSON_TK_LCURLY);
    case '}': return cson__make_punc(lex, CSON_TK_RCURLY);
    case '[': return cson__make_punc(lex, CSON_TK_LSQUARE);
    case ']': return cson__make_punc(lex, CSON_TK_RSQUARE);
    case ':': return cson__make_punc(lex, CSON_TK_COLON);
    case ',': return cson__make_punc(lex, CSON_TK_COMMA);
    case '"': return cson__make_string(lex);
    default: cson__fatal("unknown character: %c", ch);
    }
}

// cson__match - get a token and check the kind
// @lex: pointer to lexer
// @kind: expected kind
// Note: if it is unexpected, just abort
// Return: the expected token
static cson_token_t cson__match(cson_lexer_t *lex, cson_token_kind_t kind) {
    cson_token_t token = cson__get_next_token(lex);
    if (token.kind != kind) cson__fatal("expect %d, but got %d at '%.*s'",
                                       kind, token.kind, (int) token.len, token.start);
    return token;
}

// cson__peek - peek current token
// @lex: pointer to lexer
// Note: not consume current token
// Return current token
static cson_token_t cson__peek(cson_lexer_t *lex) {
    cson_lexer_t lex_copy = *lex;
    cson_token_t token = cson__get_next_token(&lex_copy);
    return token;
}

// cson__strndup - strndup
// @s: original string (Nullable)
// @len: duplicate length
// Note: C99 not support strndup
// Return: duplicate string
static char *cson__strndup(const char *s, size_t len) {
    if (!s) return NULL;
    char *p = (char *) malloc(len + 1);
    if (!p) cson__fatal("out of memeory");
    memcpy(p, s, len);
    p[len] = '\0';
    return p;
}

static cson_node_t cson__parse_object(cson_lexer_t *lex);
static cson_node_t cson__parse_array(cson_lexer_t *lex);

// cson__parse_value - parse json value
// @lex: pointer to lexer
// Note: abort if failed to parse value, and the result will be append to container,
//       so it do not need to its pointer, otherwise there would be duplicate nodes
// Return: value node without key yet
static cson_node_t cson__parse_value(cson_lexer_t *lex) {
    cson_node_t node;
    memset(&node, 0, sizeof(node));
    cson_token_t token = cson__get_next_token(lex);

    switch (token.kind) {
    case CSON_TK_NULL:
        node.kind = CSON_NULL;
        break;
    case CSON_TK_NUMBER: {
        node.kind = CSON_NUMBER;
        char buffer[token.len + 1];
        memcpy(buffer, token.start, token.len);
        buffer[token.len] = '\0';
        node.as.number = strtod(buffer, NULL);
    } break;
    case CSON_TK_TRUE:
        node.kind = CSON_BOOLEAN;
        node.as.boolean = true;
        break;
    case CSON_TK_FALSE:
        node.kind = CSON_BOOLEAN;
        node.as.boolean = false;
        break;
    case CSON_TK_STRING:
        node.kind = CSON_STRING;
        node.as.string = cson__strndup(token.start, token.len);
        break;
    case CSON_TK_LCURLY:
        node = cson__parse_object(lex);
        break;
    case CSON_TK_LSQUARE:
        node = cson__parse_array(lex);
        break;
    default: cson__fatal("unreachable");
    }

    return node;
}

// cson__parse_pair - parse json pair
// @lex: pointer to lexer
// Note: abort if failed to parse pair, and the result will be append to container,
//       so it do not need to its pointer, otherwise there would be duplicate nodes
// Return: pair node
static cson_node_t cson__parse_pair(cson_lexer_t *lex) {
    cson_token_t key = cson__match(lex, CSON_TK_STRING);
    cson__match(lex, CSON_TK_COLON);
    cson_node_t node = cson__parse_value(lex);
    node.key = cson__strndup(key.start, key.len);
    return node;
}

// cson__parse_obejct - parse json array
// @lex: pointer to lexer
// Return: array node
static cson_node_t cson__parse_array(cson_lexer_t *lex) {
    cson_node_t node = cson_create_array(NULL);
    while (1) {
        cson_token_t token = cson__peek(lex);
        if (token.kind == CSON_TK_RSQUARE) {
            break;
        } else if (token.kind == CSON_TK_COMMA) {
            cson__get_next_token(lex);
            continue;
        }
        cson_node_t value_node = cson__parse_value(lex);
        cson_append(&node, value_node);
    }
    cson__match(lex, CSON_TK_RSQUARE);
    return node;
}

// cson__parse_obejct - parse json object
// @lex: pointer to lexer
// Return: object node
static cson_node_t cson__parse_object(cson_lexer_t *lex) {
    cson_node_t node = cson_create_object(NULL);
    while (1) {
        cson_token_t token = cson__peek(lex);
        if (token.kind == CSON_TK_RCURLY) {
            break;
        } else if (token.kind == CSON_TK_COMMA) {
            cson__get_next_token(lex);
            continue;
        }
        cson_node_t pair_node = cson__parse_pair(lex);
        cson_append(&node, pair_node);
    }
    cson__match(lex, CSON_TK_RCURLY);
    return node;
}

// cson__create_container - create a container node
// @kind: node type
// @key: member key (Nullable)
// Return: object or array node (always valid)
static cson_node_t cson_create_container(cson_node_kind_t kind, const char *key) {
    cson_node_t node;
    memset(&node, 0, sizeof(node));
    node.kind = kind;
    node.key = key ? cson__strndup(key, strlen(key)) : NULL;
    return node;
}

CSONDEF cson_node_t cson_create_object(const char *key) {
    return cson_create_container(CSON_OBJECT, key);
}

CSONDEF cson_node_t cson_create_array(const char *key) {
    return cson_create_container(CSON_ARRAY, key);
}

CSONDEF cson_node_t cson_create_number(const char *key, double value) {
    cson_node_t node;
    node.kind = CSON_NUMBER;
    node.key = key ? cson__strndup(key, strlen(key)) : NULL;
    node.as.number = value;
    return node;
}

CSONDEF cson_node_t cson_create_boolean(const char *key, bool value) {
    cson_node_t node;
    node.kind = CSON_BOOLEAN;
    node.key = key ? cson__strndup(key, strlen(key)) : NULL;
    node.as.boolean = value;
    return node;
}

CSONDEF cson_node_t cson_create_string(const char *key, const char *value) {
    cson_node_t node;
    node.kind = CSON_STRING;
    node.key = key ? cson__strndup(key, strlen(key)) : NULL;
    node.as.string = value ? cson__strndup(value, strlen(value)) : NULL;
    return node;
}

CSONDEF cson_node_t cson_create_null(const char *key) {
    cson_node_t node;
    node.kind = CSON_NULL;
    node.key = key ? cson__strndup(key, strlen(key)) : NULL;
    return node;
}

CSONDEF cson_node_t cson_load_buffer(const char *buffer) {
    cson_lexer_t lex = { .start = buffer, .current = buffer };
    // while (1) {
    //     cson_token_t token = cson__get_next_token(&lex);
    //     if (token.kind == CSON_TK_EOF) break;
    //     printf("token: %d, value: %.*s\n", token.kind, (int) token.len, token.start);
    // }
    // return NULL;
    cson__match(&lex, CSON_TK_LCURLY);
    return cson__parse_object(&lex);
}

CSONDEF cson_node_t cson_load_file(const char *path) {
    char *buffer = cson__read_file(path);
    if (!buffer) cson__fatal("empty file or error occurs when reading file");
    cson_node_t root = cson_load_buffer(buffer);
    free(buffer);
    return root;
}

static void cson__dump_value(const cson_node_t *node, FILE *f, size_t level, bool indent);
static void cson__dump_indent(FILE *f, size_t level);

// cson__dump_pair - output pair (key and value)
// @node: current node
// @f: output file pointer
// @level: indent level
// @comma: write comma or not
static void cson__dump_pair(const cson_node_t *node, FILE *f, size_t level, bool comma) {
    cson__dump_indent(f, level);
    fprintf(f, "\"%s\": ", node->key);
    cson__dump_value(node, f, level, false);
    if (comma) fprintf(f, ",\n");
    else fprintf(f, "\n");
}

// cson__dump_value - output value
// @node: current node
// @f: output file pointer
// @indent: write indent or not
static void cson__dump_value(const cson_node_t *node, FILE *f, size_t level, bool indent) {
    if (indent) cson__dump_indent(f, level);

    switch (node->kind) {
    case CSON_OBJECT:
        fprintf(f, "{\n");
        for (size_t i = 0; i < node->as.container.len; i++) {
            cson_node_t *subnode = &node->as.container.items[i];
            bool comma = true;
            if (i == node->as.container.len - 1) comma = false;
            cson__dump_pair(subnode, f, level + 1, comma);
        }
        cson__dump_indent(f, level);
        fprintf(f, "}");
        break;

    case CSON_ARRAY:
        fprintf(f, "[\n");
        for (size_t i = 0; i < node->as.container.len; i++) {
            cson_node_t *subnode = &node->as.container.items[i];
            cson__dump_value(subnode, f, level + 1, true);
            if (i == node->as.container.len - 1) fprintf(f, "\n");
            else fprintf(f, ",\n");
        }
        cson__dump_indent(f, level);
        fprintf(f, "]");
        break;

    case CSON_STRING:
        fprintf(f, "\"");
        fprintf(f, node->as.string);
        fprintf(f, "\"");
        break;

    case CSON_NUMBER:
        fprintf(f, "%.15g", node->as.number);
        break;

    case CSON_BOOLEAN:
        if (node->as.boolean) fprintf(f, "true");
        else fprintf(f, "false");
        break;

    case CSON_NULL:
        fprintf(f, "null");
        break;

    default:
        break;
    }
}

// cson__dump_indent - output indent with level
// @f: output file pointer
// @level: current level
static void cson__dump_indent(FILE *f, size_t level) {
    const char *indent = "    ";
    for (size_t i = 0; i < level; i++) {
        fprintf(f, indent);
    }
}

CSONDEF void cson_write(const cson_node_t *root, FILE *f) {
    cson__dump_value(root, f, 0, true);
}

CSONDEF void cson_generate_file(const cson_node_t *root, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) cson__fatal("failed to open file '%s'", path);
    cson_write(root, f);
    fclose(f);
}

CSONDEF void cson_free(cson_node_t *root) {
    if (!root) return;
    if (root->key) free(root->key);
    switch (root->kind) {
    case CSON_NULL:
    case CSON_BOOLEAN:
    case CSON_NUMBER:
        break;
    case CSON_STRING:
        if (root->as.string) free(root->as.string);
        break;
    case CSON_ARRAY:
    case CSON_OBJECT:
        for (size_t i = 0; i < root->as.container.len; i++) {
            cson_node_t *node = &root->as.container.items[i];
            cson_free(node);
        }
        if (root->as.container.items) free(root->as.container.items);
        break;
    default: cson__fatal("unreachable");
    }
}

CSONDEF cson_node_t *cson_query(const cson_node_t *root, const char *key) {
    if (root->kind != CSON_OBJECT) cson__fatal("query node should be an object");
    if (!key) return NULL;
    for (size_t i = 0; i < root->as.container.len; i++) {
        cson_node_t *node = &root->as.container.items[i];
        if (strcmp(node->key, key) == 0) return node;
    }
    return NULL;
}

CSONDEF cson_nodes_t cson_to_object(const cson_node_t *node) {
    if (node->kind != CSON_OBJECT) cson__fatal("should be an object node");
    return node->as.container;
}

CSONDEF cson_nodes_t cson_to_array(const cson_node_t *node) {
    if (node->kind != CSON_ARRAY) cson__fatal("should be an array node");
    return node->as.container;
}

CSONDEF double cson_to_number(const cson_node_t *node) {
    if (node->kind != CSON_NUMBER) cson__fatal("should be an number node");
    return node->as.number;
}

CSONDEF bool cson_to_boolean(const cson_node_t *node) {
    if (node->kind != CSON_BOOLEAN) cson__fatal("should be an boolean node");
    return node->as.boolean;
}

CSONDEF const char *cson_to_string(const cson_node_t *node) {
    if (node->kind != CSON_STRING) cson__fatal("should be an string node");
    return node->as.string;
}

CSONDEF void cson_remove_with_key(cson_node_t *node, const char *key) {
    if (node->kind != CSON_OBJECT) cson__fatal("should be an object node");
    cson_node_t *removed_node = cson_query(node, key);
    if (!removed_node) return;
    *removed_node = node->as.container.items[node->as.container.len-1];
    node->as.container.len--;
}

CSONDEF void cson_remove_with_idx(cson_node_t *node, size_t idx) {
    if (node->kind != CSON_ARRAY) cson__fatal("should be an object or array node");
    if (idx >= node->as.container.len) cson__fatal("index out of range");
    node->as.container.items[idx] = node->as.container.items[node->as.container.len-1];
    node->as.container.len--;
}

CSONDEF void cson_remove_all(cson_node_t *node) {
    if (node->kind != CSON_OBJECT && node->kind != CSON_ARRAY) {
        cson__fatal("should be an object or array node");
    }
    node->as.container.len = 0;
}

#endif

/*
------------------------------------------------------------------------------
This software is available under MIT license.
------------------------------------------------------------------------------
Copyright (c) 2025 Dylaris
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
