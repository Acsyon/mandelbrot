#include "getopt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

/**
 * Communication variable with the caller.
 * If the current option takes an argument, it points to that argument.
 */
char *optarg = NULL;

/**
 * Communication variable with the caller.
 * Index in ARGV currently being processed. 0 on first call.
 */
int optind = 1;

/**
 * Communication variable with the caller.
 * Boolean switch to toggle whether to print error messages or not.
 */
int opterr = 1;

/**
 * Communication variable with the caller.
 * Set to an option character which was unrecognized.
 */
int optopt = '?';

/**
 * The next char to be scanned in the current `argv` element. This is necessary
 * to resume for multiple short options in single element (which is allowed).
 *
 * If it points to '\0' or is NULL, we are done for the current `argv` element
 * and can advance to the next one.
 */
static char *nextchar;

int
getopt(int argc, char *const *argv, const char *optstring)
{
    return getopt_long(argc, argv, optstring, NULL, NULL);
}

int
getopt_long(
  int argc,
  char *const *argv,
  const char *shortopts,
  const struct option *longopts,
  int *indexptr
)
{
    optarg = NULL;

    /* argv[0] is program name not an argument */
    if (optind == 0) {
        optind = 1;
    }

    /* Analyse new `argv` element */
    if (nextchar == NULL || *nextchar == '\0') {
        /* Stop if we encounter `argv` element "--" */
        if (optind != argc && strcmp(argv[optind], "--") == 0) {
            return -1;
        }

        /* Stop if we have processed all entries of argv */
        if (optind == argc) {
            return -1;
        }

        /* Stop for a non-option */
        if (argv[optind][0] != '-' || argv[optind][1] == '\0') {
            return -1;
        }

        /* For valid element, skip initial dash(es) */
        nextchar = &argv[optind][1];
        if (longopts != NULL && argv[optind][1] == '-') {
            ++nextchar;
        }
    }

    /* Check if current element is a long option */
    if (longopts != NULL && argv[optind][1] == '-') {
        /* Search end of option */
        char *optend = nextchar;
        while (*optend != '\0' && *optend != '=') {
            ++optend;
        }

        /* Go through all long options */
        int found = 0;
        const struct option *opt;
        int idx;
        for (opt = longopts, idx = 0; opt->name; ++opt, ++idx) {
            if (strncmp(opt->name, nextchar, optend - nextchar) == 0) {
                found = 1;
                break;
            }
        }

        /* Process found option */
        if (found != 0) {
            optind++;
            if (*optend != '\0') {
                if (opt->has_arg != NO_ARGUMENT) {
                    optarg = optend + 1;
                } else {
                    if (opterr != 0) {
                        log_err(
                          "Option '--%s' doesn't allow an argument\n", opt->name
                        );

                        optopt = opt->val;
                        return '?';
                    }
                }
            } else if (opt->has_arg == REQUIRED_ARGUMENT) {
                if (optind < argc) {
                    optarg = argv[optind];
                    ++optind;
                } else {
                    if (opterr != 0) {
                        log_err(
                          "Option '%s' requires an argument\n", argv[optind - 1]
                        );
                    }
                    optopt = opt->val;
                    return (shortopts[0] == ':') ? ':' : '?';
                }
            }
            nextchar += strlen(nextchar);
            if (indexptr != NULL) {
                *indexptr = idx;
            }
            if (opt->flag != NULL) {
                *(opt->flag) = opt->val;
                return 0;
            }
            return opt->val;
        }

        /* Not a valid long option */
        if (opterr != 0) {
            char tmp = *optend;
            *optend = '\0';
            log_err("Unrecognized option '--%s'\n", nextchar);
            *optend = tmp;
        }
        ++optind;
        optopt = 0;
        return '?';
    }

    /* Handle short option */
    {
        /* Search character in `shortopts` */
        char c = *nextchar;
        char *tmp = strchr(shortopts, c);
        ++nextchar;

        /* Increment `optind` if we arrived at last character of current
         * `argv` element. */
        if (*nextchar == '\0') {
            ++optind;
        }

        /* Invalid option */
        if (tmp == NULL || c == ':') {
            if (opterr != 0) {
                log_err("Illegal option -- '%c'\n", c);
            }
            optopt = c;
            return '?';
        }

        if (tmp[1] == ':') {
            /* This option requires an argument. Therefore, the next
             * character cannot be in the same `argv` element. */
            if (*nextchar != '\0') {
                optarg = nextchar;
                ++optind;
            } else if (optind == argc) {
                if (opterr != 0) {
                    log_err("Option requires an argument -- '%c'\n", c);
                }
                optopt = c;
                c = (shortopts[0] == ':') ? ':' : '?';
            } else {
                /* Increment `optind` once more, if argument is next `argv`
                 * element */
                optarg = argv[optind];
                ++optind;
            }
            nextchar = NULL;
        }
        return c;
    }
}
