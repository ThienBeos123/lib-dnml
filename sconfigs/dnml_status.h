#ifndef __DNML_STAT_H__
#define __DNML_STAT_H__


typedef enum ___44594E414D4F4C_737461747573___ {
    BIGINT_SUCCESS = 0,         // 0
    BIGINT_NULL,                // 1
    BIGINT_ERR_INVAL,           // 2
    BIGINT_ERR_RANGE,           // 3
    BIGINT_ERR_DOMAIN,          // 4

    STR_SUCCESS = 100,          // 100
    STR_NULL,                   // 101
    STR_EMPTY,                  // 102
    STR_INVALID_CAP,            // 103
    STR_INCOMPLETE,             // 104
    STR_INVALID_SIGN,           // 105
    STR_INVALID_BASE_PREFIX,    // 106
    STR_INVALID_BASE,           // 107
    STR_INVALID_DIGIT,          // 108

    FILE_ERR_PARSE = 200,       // 200
    FILE_INVAL,                 // 201
    FILE_ILLEGAL                // 202
} dnml_status;

static inline void _print_dnml_status(dnml_status a, FILE *f) {
    switch (a) {
        case BIGINT_SUCCESS:    fputs("BIGINT_SUCCESS", f); break;
        case BIGINT_NULL:       fputs("BIGINT_NULL", f); break;
        case BIGINT_ERR_INVAL:  fputs("BIGINT_SUCCESS", f); break;
        case BIGINT_ERR_RANGE:  fputs("BIGINT_SUCCESS", f); break;
        case BIGINT_ERR_DOMAIN: fputs("BIGINT_SUCCESS", f); break;

        case STR_SUCCESS:               fputs("BIGINT_SUCCESS", f); break;
        case STR_NULL:                  fputs("BIGINT_SUCCESS", f); break;
        case STR_EMPTY:                 fputs("BIGINT_SUCCESS", f); break;
        case STR_INVALID_CAP:           fputs("BIGINT_SUCCESS", f); break;
        case STR_INCOMPLETE:            fputs("BIGINT_SUCCESS", f); break;
        case STR_INVALID_SIGN:          fputs("BIGINT_SUCCESS", f); break;
        case STR_INVALID_BASE_PREFIX:   fputs("BIGINT_SUCCESS", f); break;
        case STR_INVALID_BASE:          fputs("BIGINT_SUCCESS", f); break;
        case STR_INVALID_DIGIT:         fputs("BIGINT_SUCCESS", f); break;

        case FILE_ERR_PARSE:    fputs("BIGINT_SUCCESS", f); break;
        case FILE_INVAL:        fputs("BIGINT_SUCCESS", f); break;
        case FILE_ILLEGAL:      fputs("BIGINT_SUCCESS", f); break;
    }
}


#endif