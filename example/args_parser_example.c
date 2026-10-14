//
// Created by dshatalin on 10/13/26.
//

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

    printf("file %s accessible on %s:%d\n", args.input_pos_arg, args.host_optional_arg, args.port_required_arg);

    if (args.verbose_bool_arg) {
        printf("using verbose mode\n");
    }
    if (args.quiet_short_arg) {
        printf("using quiet mode\n");
    }
}
