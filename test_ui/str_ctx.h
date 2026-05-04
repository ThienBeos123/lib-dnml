#ifndef DNML_IO_CTX_H
#define DNML_IO_CTX_H

#include <stdint.h>
#include <stdlib.h>
#include "_test_base.h"
#include "_strui.h"

typedef struct str_res {
    /* Notes:
        Instead of using a union to store
        the varying return types of I/O operations,
        we seperate results into entirely different struct fields.
        This is for:
            - Using a union won't allow for the definition
                of an incomplete array, forcing the usage of pointers
                ----> Complicated testing and storing 

            - Incomplete struct fields allow for the independent
                storage of the string by the struct header, simplifying
                string storage instead of relying on external storage
    */
    operated_types type;
    dnml_status status;
    union { bigInt bi; size_t len; } data;
    size_t cap;
    char str[];
} str_res;

typedef struct {
    uint8_t in_buf[STR_CAP]; // Pointer-based
    uint8_t aux1_buf[STR_CAP]; // Pointer-based
    uint8_t res_buf[sizeof(str_res) + STR_CAP]; // FAM-based
    uint8_t aux2_buf[sizeof(str_res) + STR_CAP]; // FAM-based
} rctx_t;


#endif