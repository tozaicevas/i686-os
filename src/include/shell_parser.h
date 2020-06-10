#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include <stdint.h>

namespace shell_parser {

    enum PARSED_COMMAND {
        UNKNOWN,
        LS,
        CAT,
        HELP,
        ECHO
    };

    char *build_kernel_output(char *input);

}

#endif
