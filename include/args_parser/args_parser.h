//
// Created by dshatalin on 10/11/26.
//

#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Compiler version guard
#if !defined(__cplusplus) && (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L)
  #error "args_parser requires C23 or newer"
#endif

// Usage: define any of the lists below BEFORE including this header, e.g.
//
//   #define POSITIONAL_ARGS   POSITIONAL_STRING_ARG(input, 0, "input file")
//   #define REQUIRED_FLAGS    REQUIRED_INT32_FLAG(port, "port", "port to listen on")
//   #define OPTIONAL_FLAGS    OPTIONAL_STRING_FLAG(host, "host", "localhost", "host name")
//   #define BOOLEAN_FLAGS     BOOLEAN_FLAG(verbose, "verbose", "verbose output")   // --verbose
//   #define SHORT_FLAGS       SHORT_FLAG(quiet, 'q', "be quiet")                   // -q, clusters as -qx
//
// Fields in args_t are named <name>_pos_arg / _required_arg / _optional_arg / _bool_arg / _short_arg.
// parse_args() returns false (after printing an error to stderr) on any failure.

#define POSITIONAL_ARG(type, name, position, description, parser) POSITIONAL_ARG_IMPL(type, name, position, description, parser)
#define REQUIRED_FLAG(type, name, flag, description, parser) REQUIRED_FLAG_IMPL(type, name, flag, description, parser)
#define OPTIONAL_FLAG(type, name, flag, default_val, description, parser) OPTIONAL_FLAG_IMPL(type, name, flag, default_val, description, parser)
#define BOOLEAN_FLAG(name, flag, description) BOOLEAN_FLAG_IMPL(name, flag, description)
#define SHORT_FLAG(name, flag, description) SHORT_FLAG_IMPL(name, flag, description)

#define POSITIONAL_STRING_ARG(name, position, description) POSITIONAL_ARG(char*, name, position, description, args_parse_str)
#define POSITIONAL_CHAR_ARG(name, position, description) POSITIONAL_ARG(char, name, position, description, args_parse_char)
#define POSITIONAL_INT16_ARG(name, position, description) POSITIONAL_ARG(int16_t, name, position, description, args_parse_int16)
#define POSITIONAL_UINT16_ARG(name, position, description) POSITIONAL_ARG(uint16_t, name, position, description, args_parse_uint16)
#define POSITIONAL_INT32_ARG(name, position, description) POSITIONAL_ARG(int32_t, name, position, description, args_parse_int32)
#define POSITIONAL_UINT32_ARG(name, position, description) POSITIONAL_ARG(uint32_t, name, position, description, args_parse_uint32)
#define POSITIONAL_INT64_ARG(name, position, description) POSITIONAL_ARG(int64_t, name, position, description, args_parse_int64)
#define POSITIONAL_UINT64_ARG(name, position, description) POSITIONAL_ARG(uint64_t, name, position, description, args_parse_uint64)
#define POSITIONAL_SIZE_ARG(name, position, description) POSITIONAL_ARG(size_t, name, position, description, args_parse_size)
#define POSITIONAL_FLOAT_ARG(name, position, description) POSITIONAL_ARG(float, name, position, description, args_parse_float)
#define POSITIONAL_DOUBLE_ARG(name, position, description) POSITIONAL_ARG(double, name, position, description, args_parse_double)

#define REQUIRED_STRING_FLAG(name, flag, description) REQUIRED_FLAG(char*, name, flag, description, args_parse_str)
#define REQUIRED_CHAR_FLAG(name, flag, description) REQUIRED_FLAG(char, name, flag, description, args_parse_char)
#define REQUIRED_INT16_FLAG(name, flag, description) REQUIRED_FLAG(int16_t, name, flag, description, args_parse_int16)
#define REQUIRED_UINT16_FLAG(name, flag, description) REQUIRED_FLAG(uint16_t, name, flag, description, args_parse_uint16)
#define REQUIRED_INT32_FLAG(name, flag, description) REQUIRED_FLAG(int32_t, name, flag, description, args_parse_int32)
#define REQUIRED_UINT32_FLAG(name, flag, description) REQUIRED_FLAG(uint32_t, name, flag, description, args_parse_uint32)
#define REQUIRED_INT64_FLAG(name, flag, description) REQUIRED_FLAG(int64_t, name, flag, description, args_parse_int64)
#define REQUIRED_UINT64_FLAG(name, flag, description) REQUIRED_FLAG(uint64_t, name, flag, description, args_parse_uint64)
#define REQUIRED_SIZE_FLAG(name, flag, description) REQUIRED_FLAG(size_t, name, flag, description, args_parse_size)
#define REQUIRED_FLOAT_FLAG(name, flag, description) REQUIRED_FLAG(float, name, flag, description, args_parse_float)
#define REQUIRED_DOUBLE_FLAG(name, flag, description) REQUIRED_FLAG(double, name, flag, description, args_parse_double)

#define OPTIONAL_STRING_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(char*, name, flag, default_val, description, args_parse_str)
#define OPTIONAL_CHAR_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(char, name, flag, default_val, description, args_parse_char)
#define OPTIONAL_INT16_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(int16_t, name, flag, default_val, description, args_parse_int16)
#define OPTIONAL_UINT16_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(uint16_t, name, flag, default_val, description, args_parse_uint16)
#define OPTIONAL_INT32_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(int32_t, name, flag, default_val, description, args_parse_int32)
#define OPTIONAL_UINT32_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(uint32_t, name, flag, default_val, description, args_parse_uint32)
#define OPTIONAL_INT64_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(int64_t, name, flag, default_val, description, args_parse_int64)
#define OPTIONAL_UINT64_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(uint64_t, name, flag, default_val, description, args_parse_uint64)
#define OPTIONAL_SIZE_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(size_t, name, flag, default_val, description, args_parse_size)
#define OPTIONAL_FLOAT_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(float, name, flag, default_val, description, args_parse_float)
#define OPTIONAL_DOUBLE_FLAG(name, flag, default_val, description) OPTIONAL_FLAG(double, name, flag, default_val, description, args_parse_double)

// HELPERS

#define MERGE_TWO(prefix, suffix) prefix##suffix

static inline char *args_skip_spaces(char *str) {
    if (!str) {
        return nullptr;
    }
    while (isspace((unsigned char)*str)) str++;
    return str;
}

// PARSERS

#define PARSE_NUMERIC(convert, wide_type, ret_type, in_range) \
    char* end; \
    errno = 0; \
    const wide_type val = convert; \
    if (*end != '\0') { \
        fprintf(stderr, "Error: '%s' invalid integer value for '%s'\n", text, name); \
        return (ret_type)0; \
    } \
    if (errno == ERANGE || !(in_range)) { \
        fprintf(stderr, "Error: '%s' is out of range for '%s'\n", text, name); \
        return (ret_type)0; \
    } \
    *ok = 1; \
    return (ret_type)val;

#define DEFINE_SIGNED_INT_PARSER(func_name, ret_type, max_val, min_val) \
    static inline ret_type func_name(char *name, char *text, int *ok) { \
        *ok = 0; \
        if (!text) { \
            fprintf(stderr, "Error: null input for '%s'\n", name); \
            return (ret_type)0; \
        } \
        text = args_skip_spaces(text); \
        if (text[0] == '\0') { \
            fprintf(stderr, "Error: empty value not allowed for '%s'\n", name); \
            return (ret_type)0; \
        } \
        PARSE_NUMERIC(strtoll(text, &end, 10), long long, ret_type, val <= max_val && val >= min_val) \
    }

#define DEFINE_UNSIGNED_INT_PARSER(func_name, ret_type, max_val) \
    static inline ret_type func_name(char *name, char *text, int *ok) { \
        *ok = 0; \
        if (!text) { \
            fprintf(stderr, "Error: null input for '%s'\n", name); \
            return (ret_type)0; \
        } \
        text = args_skip_spaces(text); \
        if (text[0] == '\0') { \
            fprintf(stderr, "Error: empty value not allowed for '%s'\n", name); \
            return (ret_type)0; \
        } \
        if (text[0] == '-') { \
            fprintf(stderr, "Error: invalid unsigned integer value for '%s'\n", name); \
            return (ret_type)0; \
        } \
        PARSE_NUMERIC(strtoull(text, &end, 10), unsigned long long, ret_type, val <= max_val) \
    }

#define DEFINE_FLOATING_PARSER(func_name, converter, ret_type, max_val, min_val) \
    static inline ret_type func_name(char *name, char *text, int *ok) { \
        *ok = 0; \
        if (!text) { \
            fprintf(stderr, "Error: null input for '%s'\n", name); \
            return (ret_type)0; \
        } \
        text = args_skip_spaces(text); \
        if (text[0] == '\0') { \
            fprintf(stderr, "Error: empty value not allowed for '%s'\n", name); \
            return (ret_type)0; \
        } \
        PARSE_NUMERIC(converter(text, &end), ret_type, ret_type, val <= max_val && val >= min_val) \
    }

static inline char *args_parse_str(char *name, char *text, int *ok) {
    *ok = 0;
    if (!text) {
        fprintf(stderr, "Error: null input for '%s'\n", name);
        return nullptr;
    }
    *ok = 1;
    return text;
}

static inline char args_parse_char(char *name, char *text, int *ok) {
    *ok = 0;
    if (!text) {
        fprintf(stderr, "Error: null input for '%s'\n", name);
        return 0;
    }
    if (text[0] == '\0' || text[1] != '\0') {
        fprintf(stderr, "Error: '%s' is not valid char for '%s'\n", text, name);
        return 0;
    }
    *ok = 1;
    return text[0];
}

DEFINE_SIGNED_INT_PARSER(args_parse_int16, int16_t, INT16_MAX, INT16_MIN);
DEFINE_SIGNED_INT_PARSER(args_parse_int32, int32_t, INT32_MAX, INT32_MIN);
DEFINE_SIGNED_INT_PARSER(args_parse_int64, int64_t, INT64_MAX, INT64_MIN);

DEFINE_UNSIGNED_INT_PARSER(args_parse_uint16, uint16_t, UINT16_MAX);
DEFINE_UNSIGNED_INT_PARSER(args_parse_uint32, uint32_t, UINT32_MAX);
DEFINE_UNSIGNED_INT_PARSER(args_parse_uint64, uint64_t, UINT64_MAX);
DEFINE_UNSIGNED_INT_PARSER(args_parse_size, size_t, SIZE_MAX);

DEFINE_FLOATING_PARSER(args_parse_float, strtof, float, FLT_MAX, -FLT_MAX);
DEFINE_FLOATING_PARSER(args_parse_double, strtod, double, DBL_MAX, -DBL_MAX);

#undef DEFINE_FLOATING_PARSER
#undef DEFINE_UNSIGNED_INT_PARSER
#undef DEFINE_SIGNED_INT_PARSER
#undef PARSE_NUMERIC

// COUNTS

#ifdef POSITIONAL_ARGS
#define POSITIONAL_ARG_IMPL(...) +1
static constexpr int POSITIONAL_ARG_COUNT = 0 POSITIONAL_ARGS;
#undef POSITIONAL_ARG_IMPL
#else
static constexpr int POSITIONAL_ARG_COUNT = 0;
#endif

#ifdef REQUIRED_FLAGS
#define REQUIRED_FLAG_IMPL(...) +1
static constexpr int REQUIRED_FLAG_COUNT = 0 REQUIRED_FLAGS;
#undef REQUIRED_FLAG_IMPL

// index of each required flag in the "seen" array
enum {
#define REQUIRED_FLAG_IMPL(type, name, ...) MERGE_TWO(args_idx_, name),
    REQUIRED_FLAGS
#undef REQUIRED_FLAG_IMPL
};
#else
static constexpr int REQUIRED_FLAG_COUNT = 0;
#endif

// ARG_T STRUCT
#define POSITIONAL_ARG_IMPL(type, name, ...) type MERGE_TWO(name, _pos_arg);
#define REQUIRED_FLAG_IMPL(type, name, ...) type MERGE_TWO(name, _required_arg);
#define OPTIONAL_FLAG_IMPL(type, name, ...) type MERGE_TWO(name, _optional_arg);
#define BOOLEAN_FLAG_IMPL(name, ...) bool MERGE_TWO(name, _bool_arg);
#define SHORT_FLAG_IMPL(name, ...) bool MERGE_TWO(name, _short_arg);

typedef struct {
#ifdef POSITIONAL_ARGS
    POSITIONAL_ARGS
#endif
#ifdef REQUIRED_FLAGS
    REQUIRED_FLAGS
#endif
#ifdef OPTIONAL_FLAGS
    OPTIONAL_FLAGS
#endif
#ifdef BOOLEAN_FLAGS
    BOOLEAN_FLAGS
#endif
#ifdef SHORT_FLAGS
    SHORT_FLAGS
#endif
    int pos_count;
    bool seen[REQUIRED_FLAG_COUNT + 1];
} args_t;

#undef SHORT_FLAG_IMPL
#undef BOOLEAN_FLAG_IMPL
#undef OPTIONAL_FLAG_IMPL
#undef REQUIRED_FLAG_IMPL
#undef POSITIONAL_ARG_IMPL

[[maybe_unused]] static inline args_t make_default_args() {
    const args_t args = {
#define POSITIONAL_ARG_IMPL(type, name, ...) .MERGE_TWO(name, _pos_arg) = (type) 0,
#define REQUIRED_FLAG_IMPL(type, name, ...) .MERGE_TWO(name, _required_arg) = (type) 0,
#define OPTIONAL_FLAG_IMPL(type, name, flag, default_val, ...) .MERGE_TWO(name, _optional_arg) = (type) default_val,
#define BOOLEAN_FLAG_IMPL(name, ...) .MERGE_TWO(name, _bool_arg) = false,
#define SHORT_FLAG_IMPL(name, ...) .MERGE_TWO(name, _short_arg) = false,

#ifdef POSITIONAL_ARGS
        POSITIONAL_ARGS
#endif
#ifdef REQUIRED_FLAGS
        REQUIRED_FLAGS
#endif
#ifdef OPTIONAL_FLAGS
        OPTIONAL_FLAGS
#endif
#ifdef BOOLEAN_FLAGS
        BOOLEAN_FLAGS
#endif
#ifdef SHORT_FLAGS
        SHORT_FLAGS
#endif

#undef SHORT_FLAG_IMPL
#undef BOOLEAN_FLAG_IMPL
#undef OPTIONAL_FLAG_IMPL
#undef REQUIRED_FLAG_IMPL
#undef POSITIONAL_ARG_IMPL
        .pos_count = 0,
        .seen = {false},
    };
    return args;
}

// FLAG PARSING

// --flag=value or --flag value (value-less boolean flags: --flag)
[[maybe_unused]] static int parse_flag(const int idx, [[maybe_unused]] const int argc, char *argv[],
                                       [[maybe_unused]] args_t *args, [[maybe_unused]] bool *seen) {
    char *name = argv[idx] + 2;
    char *eq = strchr(name, '=');
    const size_t name_len = eq ? (size_t) (eq - name) : strlen(name);
    [[maybe_unused]] char *value = eq ? eq + 1 : nullptr;
    [[maybe_unused]] int next = idx;

#define ARGS_FLAG_MATCHES(flag) (name_len == strlen(flag) && strncmp(name, flag, name_len) == 0)

    // value comes from after '=', or else from the next argv element
#define ARGS_SET_VALUE(field, flag, parser, on_set) \
    if (ARGS_FLAG_MATCHES(flag)) { \
        if (!value) { \
            if (next + 1 >= argc) { \
                fprintf(stderr, "Error: missing value for '--%s'\n", flag); \
                return -1; \
            } \
            value = argv[++next]; \
        } \
        int ok = 0; \
        args->field = parser(flag, value, &ok); \
        if (!ok) { \
            return -1; \
        } \
        on_set; \
        return next; \
    }

#ifdef REQUIRED_FLAGS
#define REQUIRED_FLAG_IMPL(type, name, flag, description, parser) \
    ARGS_SET_VALUE(MERGE_TWO(name, _required_arg), flag, parser, seen[MERGE_TWO(args_idx_, name)] = true)
    REQUIRED_FLAGS
#undef REQUIRED_FLAG_IMPL
#endif
#ifdef OPTIONAL_FLAGS
#define OPTIONAL_FLAG_IMPL(type, name, flag, default_val, description, parser) \
    ARGS_SET_VALUE(MERGE_TWO(name, _optional_arg), flag, parser, (void) 0)
    OPTIONAL_FLAGS
#undef OPTIONAL_FLAG_IMPL
#endif
#ifdef BOOLEAN_FLAGS
#define BOOLEAN_FLAG_IMPL(name, flag, description) \
    if (ARGS_FLAG_MATCHES(flag)) { \
        if (value) { \
            fprintf(stderr, "Error: '--%s' does not take a value\n", flag); \
            return -1; \
        } \
        args->MERGE_TWO(name, _bool_arg) = true; \
        return next; \
    }
    BOOLEAN_FLAGS
#undef BOOLEAN_FLAG_IMPL
#endif

#undef ARGS_SET_VALUE
#undef ARGS_FLAG_MATCHES

    fprintf(stderr, "Error: unknown flag '%.*s'\n", (int) (name_len + 2), argv[idx]);
    return -1;
}

// -a, or a cluster of boolean short flags: -abc
[[maybe_unused]] static int parse_short_flag(const int idx, char *argv[], [[maybe_unused]] args_t *args) {
#ifdef SHORT_FLAGS
#define MAYBE_CONST
#else
#define MAYBE_CONST constexpr
#endif
    for (const char *c = argv[idx] + 1; *c; ++c) {
        MAYBE_CONST bool matched = false;
#ifdef SHORT_FLAGS
#define SHORT_FLAG_IMPL(name, flag, description) \
        if (*c == (flag)) { \
            args->MERGE_TWO(name, _short_arg) = true; \
            matched = true; \
        }
        SHORT_FLAGS
#undef SHORT_FLAG_IMPL
#endif
        if (!matched) {
            fprintf(stderr, "Error: unknown flag '-%c'\n", *c);
            return -1;
        }
    }
#undef MAYBE_CONST
    return idx;
}

// anything not starting with '-': assigned by its declared position
[[maybe_unused]] static int parse_pos_arg(const int idx, char *argv[], [[maybe_unused]] args_t *args,
                                          [[maybe_unused]] int *pos_idx) {
#ifdef POSITIONAL_ARGS
#define POSITIONAL_ARG_IMPL(type, name, position, description, parser) \
    if ((position) == *pos_idx) { \
        int ok = 0; \
        args->MERGE_TWO(name, _pos_arg) = parser(#name, argv[idx], &ok); \
        if (!ok) { \
            return -1; \
        } \
        ++*pos_idx; \
        return idx; \
    }
    POSITIONAL_ARGS
#undef POSITIONAL_ARG_IMPL
#endif
    fprintf(stderr, "Error: unexpected positional argument '%s'\n", argv[idx]);
    return -1;
}

[[maybe_unused]] static bool parse_args(const int argc, char *argv[], args_t *args) {
    if (!argc || !argv || !args) {
        fprintf(stderr, "Internal Error: null argc, argv or args\n");
        return false;
    }

    *args = make_default_args();
    bool only_positional = false; // set after a bare "--"

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        int last;
        if (!only_positional && strcmp(arg, "--") == 0) {
            only_positional = true;
            continue;
        }
        if (!only_positional && arg[0] == '-' && arg[1] == '-') {
            last = parse_flag(i, argc, argv, args, args->seen);
        } else if (!only_positional && arg[0] == '-' && arg[1] != '\0') {
            last = parse_short_flag(i, argv, args);
        } else {
            last = parse_pos_arg(i, argv, args, &args->pos_count);
        }
        if (last < 0) {
            return false;
        }
        i = last;
    }

    return true;
}

[[maybe_unused]] static bool validate_args([[maybe_unused]] const args_t *args) {
#if defined(REQUIRED_FLAGS) || defined(POSITIONAL_ARGS)
#define MAYBE_CONST
#else
#define MAYBE_CONST constexpr
#endif
    MAYBE_CONST bool ok = true;
#ifdef REQUIRED_FLAGS
#define REQUIRED_FLAG_IMPL(type, name, flag, ...) \
    if (!args->seen[MERGE_TWO(args_idx_, name)]) { \
        fprintf(stderr, "Error: missing required flag '--%s'\n", flag); \
        ok = false; \
    }
    REQUIRED_FLAGS
#endif
#undef REQUIRED_FLAG_IMPL
#ifdef POSITIONAL_ARGS
    if (args->pos_count < POSITIONAL_ARG_COUNT) {
        fprintf(stderr, "Error: expected %d positional argument(s), got %d\n", POSITIONAL_ARG_COUNT, args->pos_count);
        ok = false;
    }
#endif
#undef MAYBE_CONST
    return ok;
}

[[maybe_unused]] static void args_print_usage(const char *exec_name) {
    fprintf(stdout, "Usage: %s [options]", exec_name);
#ifdef POSITIONAL_ARGS
#define POSITIONAL_ARG_IMPL(type, name, ...) fprintf(stdout, " <%s>", #name);
    POSITIONAL_ARGS
#undef POSITIONAL_ARG_IMPL
#endif
    fprintf(stdout, "\n");
}

[[maybe_unused]] static void print_help(const char *exec_name) {
    args_print_usage(exec_name);
#ifdef POSITIONAL_ARGS
    fprintf(stdout, "\nArguments:\n");
#define POSITIONAL_ARG_IMPL(type, name, position, description, parser) \
    fprintf(stdout, "  %-24s %s\n", #name, description);
    POSITIONAL_ARGS
#undef POSITIONAL_ARG_IMPL
#endif
    fprintf(stdout, "\nOptions:\n");
#ifdef REQUIRED_FLAGS
#define REQUIRED_FLAG_IMPL(type, name, flag, description, parser) \
    fprintf(stdout, "  --%-22s %s (required)\n", flag, description);
    REQUIRED_FLAGS
#undef REQUIRED_FLAG_IMPL
#endif
#ifdef OPTIONAL_FLAGS
#define OPTIONAL_FLAG_IMPL(type, name, flag, default_val, description, parser) \
    fprintf(stdout, "  --%-22s %s (default: %s)\n", flag, description, #default_val);
    OPTIONAL_FLAGS
#undef OPTIONAL_FLAG_IMPL
#endif
#ifdef BOOLEAN_FLAGS
#define BOOLEAN_FLAG_IMPL(name, flag, description) \
    fprintf(stdout, "  --%-22s %s\n", flag, description);
    BOOLEAN_FLAGS
#undef BOOLEAN_FLAG_IMPL
#endif
#ifdef SHORT_FLAGS
#define SHORT_FLAG_IMPL(name, flag, description) \
    fprintf(stdout, "  -%-23c %s\n", flag, description);
    SHORT_FLAGS
#undef SHORT_FLAG_IMPL
#endif
}

#undef MERGE_TWO

#endif //ARGS_PARSER_H
