#include "cargparse.h"

#include <stdio.h>

void cargparse_print_help(const cargparse_t *const self) {
    int i;

    if (self->usages) {
        const char *const *usages_ptr = self->usages;
        printf("Usage: %s\n", *usages_ptr++);
        while (*usages_ptr) {
            printf("       %s\n", *usages_ptr++);
        }
        printf("\n");
    }
    if (self->description) {
        printf("%s\n", self->description);
        printf("\n");
    }
    if (self->n_options > 0) {
        for (i = 0; i < self->n_options; i++) {
            printf("  -%c --%s %s\n", self->options[i].short_name, self->options[i].long_name,
                   self->options[i].help);
        }
        printf("\n");
    }
    if (self->epilog) {
        printf("%s\n", self->epilog);
    }
}
