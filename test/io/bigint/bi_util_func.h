#ifndef _BIGINT_UTIL_FUNC_H
#define _BIGINT_UTIL_FUNC_H


#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "../../../test_ui/_strui.h"
#include "../../../adynamol/big_numbers/bigNums.h"


//* ========================= COMPARISONS WRAPPERS ======================= *//
// Invsere Comparator

// Evaluation Comparator
static inline bool cmp_eval_bitos(const str_res *exp, const str_res *res) {
    // Property Check
    if (res->type != exp->type) return false;
    if (res->status != exp->status) return false;
    if (res->status != STR_SUCCESS || res->status != STR_TRUNC_SUCCESS) return true;
    // Main Check
    if (res->data.len == exp->data.len) return strcmp(res->str, exp->str) == 0;
    else if (res->data.len > exp->data.len) return false;
    else return strncmp(res->str, exp->str, res->data.len) == 0;
}
static inline bool cmp_eval_stobi(const str_res *exp, const str_res *res) {
    if (res->status != exp->status) return false;
    else if (res->status == STR_SUCCESS || res->status == STR_TRUNC_SUCCESS) {
        return (__BIGINT_INTERNAL_COMP__(&res->data.bi, &exp->data.bi));
    } else return true;
}




#endif