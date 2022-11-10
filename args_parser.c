#include "args_parser.h"
#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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
        case 'd': {
            struct stat s;
            if (stat(optarg, &s) == 0) {
                if (S_ISDIR(s.st_mode)) {
                    ROOT_DIR = strdup(optarg);
                } else {
                    fprintf(stderr, "[SERVER] %s - not a directory", optarg);
                    exit(1);
                }
            } else {
                perror("[SERVER] stat");
                exit(1);
            }
            break;
        }
        default:
            exit(1);
        }
    }
}
