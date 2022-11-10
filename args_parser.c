#include "args_parser.h"
#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void print_help()
{
    fputs("usage: simple-http [OPTIONS]\n\n"
          "--help, -h       show this message\n"
          "--dir,  -d [DIR] specify server ROOT_DIR. (CWD by default)\n"
          "",
          stdout);
}

void parse_args(int argc, char **argv)
{
    int c;
    while (1) {
        static struct option long_options[] = {
            { "help",    required_argument, 0, 'h'},
            { "dir",     required_argument, 0, 'd'},
            {0, 0, 0, 0}
        };

        int opt_index = 0;
        c = getopt_long(argc, argv, "hd:", long_options, &opt_index);

        if (c == -1)
            break;

        switch (c) {
        case 'h':
            print_help();
            exit(0);
        case 'd':
            if (!opendir(optarg)) {
                perror("[SERVER] opendir");
                exit(1);
            }
            ROOT_DIR = strdup(optarg);
            break;
        default:
            exit(1);
        }
    }
}
