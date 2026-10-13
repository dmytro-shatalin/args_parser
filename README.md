# args_parser

Header-only, allocation-free command-line argument parser for C23. Declare your
arguments once with macros and get a typed struct, validation, and help text.

## Features

- Single header (`include/args_parser/args_parser.h`), no dependencies beyond the C standard library
- Positional arguments, required flags, optional flags with defaults, boolean flags, and short-flag clusters (`-abc`)
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

#include "args_parser/args_parser.h"

int main(int argc, char *argv[]) {
    args_t args;
    if (!parse_args(argc, argv, &args)) {
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
}
```

```bash
$ ./app data.txt --port=8080 --verbose
```

## Argument kinds

| Macro list        | Declaration                                       | Struct field          | Syntax         |                                                                                                                                                                                                                                                             
|-------------------|---------------------------------------------------|-----------------------|----------------|                                                                                                                                                                                                                                                                                                
| `POSITIONAL_ARGS` | `POSITIONAL_<TYPE>_ARG(name, position, desc)`     | `<name>_pos_arg`      | `value`        |                                                                                                                                                                                                               
| `REQUIRED_FLAGS`  | `REQUIRED_<TYPE>_FLAG(name, flag, desc)`          | `<name>_required_arg` | `--flag=value` |                                                                                                                                                                                                         
| `OPTIONAL_FLAGS`  | `OPTIONAL_<TYPE>_FLAG(name, flag, default, desc)` | `<name>_optional_arg` | `--flag=value` |                                                                                                                                                                                                
| `BOOLEAN_FLAGS`   | `BOOLEAN_FLAG(name, flag, desc)`                  | `<name>_bool_arg`     | `--flag`       |                                                                                                                                                                                                                            
| `SHORT_FLAGS`     | `SHORT_FLAG(name, 'c', desc)`                     | `<name>_short_arg`    | `-c`, `-abc`   |                                                                                                                                                                                                                            

`<TYPE>` is one of `STRING`, `CHAR`, `INT16`, `UINT16`, `INT32`, `UINT32`, `INT64`, `UINT64`,                                                                                                                                                                                                                    
`SIZE`, `FLOAT`, `DOUBLE`.

## API

| Function                                           | Description                                                           |                                                                                                                                                                                                                                                                                       
|----------------------------------------------------|-----------------------------------------------------------------------|                                                                                                                                                                                                                                                                                                        
| `parse_args(argc, argv, &args)`                    | Fills `args`. Returns `false` and prints to stderr on malformed input |                                                                                                                                                                                                      
| `validate_args(&args)`                             | Checks that required flags and positionals are present                |                                                                                                                                                                                                                              
| `print_usage(exec_name)` / `print_help(exec_name)` | Print to stdout                                                       |                                                                                                                                                                                                                                         

## Error handling

All errors are printed to stderr. `parse_args` and `validate_args` return `false`; it's up
to the caller to choose the exit code.

## Limitations

- Lists must be defined before `#include`
- Short flags are boolean only
- Repeated flags overwrite earlier values

## Building

```bash
make all
```