#ifndef OPTPARSE_H
#define OPTPARSE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct optparse {
    int argc;
    char **argv;
    int optind;
    char *optarg;
    const char *errmsg;
} optparse_t;

enum optparse_argtype {
    OPTPARSE_NONE,
    OPTPARSE_REQUIRED,
};

struct optparse_long {
    const char *longname;
    int shortname;
    enum optparse_argtype argtype;
};

static inline void optparse_init(optparse_t *options, char **argv) {
    int i = 0;
    while (argv[i] != 0) i++;
    options->argc = i;
    options->argv = argv;
    options->optind = 1;
    options->optarg = 0;
    options->errmsg = 0;
}

static inline int optparse_long(optparse_t *options, const struct optparse_long *longopts, void *unused) {
    (void)unused;
    options->optarg = 0;
    options->errmsg = 0;
    if (options->optind >= options->argc) return -1;
    char *arg = options->argv[options->optind];
    if (arg[0] != '-') return -1;
    options->optind++;
    if (arg[1] == '-') {
        const char *name = arg + 2;
        for (int i = 0; longopts && longopts[i].longname; i++) {
            if (strcmp(name, longopts[i].longname) == 0) {
                if (longopts[i].argtype == OPTPARSE_REQUIRED) {
                    if (options->optind < options->argc) {
                        options->optarg = options->argv[options->optind++];
                    } else {
                        options->errmsg = "requires an argument";
                        return '?';
                    }
                }
                return longopts[i].shortname ? longopts[i].shortname : 0;
            }
        }
        options->errmsg = "unknown option";
        return '?';
    } else {
        int ch = (unsigned char)arg[1];
        // single short option with optional next arg
        if (arg[2] != 0) {
            options->optarg = &arg[2];
        }
        return ch;
    }
}

static inline char* optparse_arg(optparse_t *options) {
    if (options->optind >= options->argc) return 0;
    return options->argv[options->optind++];
}

#ifdef __cplusplus
}
#endif

#endif

