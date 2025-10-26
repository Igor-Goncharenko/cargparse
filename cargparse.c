#include "cargparse.h"

#include <stdio.h>
#include <string.h>

void cargparse_print_help(const cargparse_t *const self) {
    int i;
    const char *ch_ptr_start, *ch_ptr_end;

    if (self->usages) {
        ch_ptr_start = self->usages;
        ch_ptr_end = strchr(ch_ptr_start, '\n');
        if (ch_ptr_end) {
            printf("Usages: %.*s\n", (int)(ch_ptr_end - ch_ptr_start), ch_ptr_start);
            ch_ptr_start = ch_ptr_end + 1;
            while ((ch_ptr_end = strchr(ch_ptr_start, '\n'))) {
                printf("        %.*s\n", (int)(ch_ptr_end - ch_ptr_start), ch_ptr_start);
                ch_ptr_start = ch_ptr_end + 1;
            }
            if (*ch_ptr_start != '\0') {
                printf("        %s\n", ch_ptr_start);
            }
        } else {
            printf("Usages: %s\n", ch_ptr_start);
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
