# args_parser

Header-only, allocation-free command-line argument parser for C23. Declare your
arguments once with macros and get a typed struct, validation, and help text.

## Features

- Single header (`include/args_parser/args_parser.h`), no dependencies beyond the C standard library
- Positional arguments, required flags, optional flags with defaults, boolean flags, and short-flag clusters (`-abc`)
- Repeatable flags (`--tag a --tag b`) and a positional rest list (`app a b c`) with fixed capacities
- `--flag=value` and `--flag value` syntax, plus `--` to end flag parsing
- Typed values (`int16`..`uint64`, `size_t`, `float`, `double`, `char`, string) with range checking
- Parsing and validation are separate, so `--help` can skip required-argument checks

## Requirements

- A C23 compiler (uses `nullptr`, `constexpr`, `bool`, `[[maybe_unused]]`)
- CMake 4.2+, if you use the CMake target

## Installation

**Copy the header:** drop `include/args_parser/args_parser.h` into your project.

**CMake:**

```cmake
add_subdirectory(args_parser)
target_link_libraries(my_app PRIVATE args_parser::args_parser)
```

## Quick start

```c++
#define POSITIONAL_ARGS \
    POSITIONAL_STRING_ARG(input, 0, "input file")
#define REQUIRED_FLAGS \
    REQUIRED_INT32_FLAG(port, "port", "port to listen on")
#define OPTIONAL_FLAGS \
    OPTIONAL_STRING_FLAG(host, "host", "localhost", "host name")
#define BOOLEAN_FLAGS \
    BOOLEAN_FLAG(help, "help", "show this help") \
    BOOLEAN_FLAG(verbose, "verbose", "verbose output")
#define SHORT_FLAGS \
    SHORT_FLAG(quiet, 'q', "be quiet") \
    SHORT_FLAG(help, 'h', "show this help")
#define LIST_FLAGS \
    REPEATED_STRING_FLAG(tag, "tag", 8, "tag")
#define POSITIONAL_REST \
    REST_STRING_ARG(extra, 0, 16, "extra input files")

#include "args_parser/args_parser.h"

int main(int argc, char *argv[]) {
    args_t args;
    if (!parse_args(argc, (const char **) argv, &args)) {
        return 1;
    }
    if (args.help_bool_arg || args.help_short_arg) {
        print_help(argv[0]);
        return 0;
    }
    if (!validate_args(&args)) {
        return 1;
    }

    printf("%s:%d\n", args.host_optional_arg, args.port_required_arg);
    for (size_t i = 0; i < args.tag_list_arg.count; ++i) {
        printf("tag: %s\n", args.tag_list_arg.items[i]);
    }
}
```

```bash
$ ./app data.txt --port=8080 --tag=a --tag=b extra1 extra2
```

## Argument kinds

| Macro list        | Declaration                                            | Struct field          | Syntax                    |
|-------------------|--------------------------------------------------------|-----------------------|---------------------------|
| `POSITIONAL_ARGS` | `POSITIONAL_<TYPE>_ARG(name, position, desc)`          | `<name>_pos_arg`      | `value`                   |
| `REQUIRED_FLAGS`  | `REQUIRED_<TYPE>_FLAG(name, flag, desc)`               | `<name>_required_arg` | `--flag=value`            |
| `OPTIONAL_FLAGS`  | `OPTIONAL_<TYPE>_FLAG(name, flag, default, desc)`      | `<name>_optional_arg` | `--flag=value`            |
| `BOOLEAN_FLAGS`   | `BOOLEAN_FLAG(name, flag, desc)`                       | `<name>_bool_arg`     | `--flag`                  |
| `SHORT_FLAGS`     | `SHORT_FLAG(name, 'c', desc)`                          | `<name>_short_arg`    | `-c`, `-abc`              |
| `LIST_FLAGS`      | `REPEATED_<TYPE>_FLAG(name, flag, capacity, desc)`     | `<name>_list_arg`     | `--flag a --flag b`       |
| `LIST_FLAGS`      | `REQUIRED_REPEATED_<TYPE>_FLAG(...)`                   | `<name>_list_arg`     | `--flag a --flag b`       |
| `POSITIONAL_REST` | `REST_<TYPE>_ARG(name, min_count, capacity, desc)`     | `<name>_rest_arg`     | `app a b c`               |

`<TYPE>` is one of `STRING`, `CHAR`, `INT16`, `UINT16`, `INT32`, `UINT32`, `INT64`, `UINT64`,
`SIZE`, `FLOAT`, `DOUBLE`.

## List arguments

Both list kinds are fixed-capacity arrays in `args_t` — no allocation, and the capacity is part
of the declaration. Each field has `items` and `count`:

```c
#define LIST_FLAGS \
    REPEATED_STRING_FLAG(tag, "tag", 4, "tag") \
    REQUIRED_REPEATED_INT32_FLAG(id, "id", 8, "id")
#define POSITIONAL_REST \
    REST_STRING_ARG(files, 1, 16, "input files")

// ... parse_args / validate_args ...

for (size_t i = 0; i < args.tag_list_arg.count; ++i) {
    puts(args.tag_list_arg.items[i]);
}
```

- A repeatable flag appends one value per occurrence, in both `--tag=a` and `--tag a` forms.
  Exceeding the capacity is a parse error; values are never silently dropped.
- `REQUIRED_REPEATED_*` means at least one value must be present; `validate_args()` checks it.
- The rest list takes every position after the declared positional arguments. It must be the
  only `POSITIONAL_REST` declaration, and it only receives values once all declared positions
  are filled. `min_count` is checked by `validate_args()`.
- Use `--` when a rest value starts with `-`. Scalar flags keep their old overwrite behavior.

## API

| Function                                           | Description                                                           |
|----------------------------------------------------|-----------------------------------------------------------------------|
| `parse_args(argc, argv, &args)`                    | Fills `args`. Returns `false` and prints to stderr on malformed input |
| `validate_args(&args)`                             | Checks required flags, required lists, and positional counts          |
| `print_usage(exec_name)` / `print_help(exec_name)` | Print to stdout                                                       |

## Error handling

All errors are printed to stderr. `parse_args` and `validate_args` return `false`; it's up
to the caller to choose the exit code.

## Limitations

- Lists must be defined before `#include`
- Short flags are boolean only
- Scalar flags overwrite earlier values; only `LIST_FLAGS` accumulates
- List capacities are fixed at compile time; overflow is reported as an error
- At most one `POSITIONAL_REST` list, and it cannot stop consuming trailing values

## Building

```bash
make all        # header-only library: nothing to compile by default
make example    # builds and runs the example, including list arguments
```
