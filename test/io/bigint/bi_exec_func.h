#ifndef _BIGINT_EXEC_FUNC_H
#define _BIGINT_EXEC_FUNC_H


#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "../../../test_ui/_strui.h"
#include "../../../adynamol/big_numbers/bigNums.h"
#include "../../../adynamol/big_numbers/bigInt_func.h"
#include "bi_indef.h"



//* ========================= BITOS EXECUTION WRAPPERS ======================= *//
// Truncative Conversions - tto_str
static inline void exec_bitos_tto_str(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_tto_str(out->str, in->x, &out->data.len);
}
static inline void exec_bitos_tto_strb(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_tto_strb(out->str, in->x, in->base, &out->data.len);
}
static inline void exec_bitos_tto_strn(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_tto_strn(out->str, in->len, in->x, &out->data.len);
}
static inline void exec_bitos_tto_strnb(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_tto_strnb(
        out->str, in->len, 
        in->x, in->base, 
        &out->data.len
    );
}
static inline void exec_bitos_tto_strf(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_tto_strf(out->str, 
        in->len, in->x, 
        in->base, in->uppercase,
        &out->data.len
    );
}
// Default / Strict Conversions - to_str
static inline void exec_bitos_to_str(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_to_str(out->str, in->x, &out->data.len);
}
static inline void exec_bitos_to_strb(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_to_strb(out->str, in->x, in->base, &out->data.len);
}
static inline void exec_bitos_to_strn(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_to_strn(out->str, in->len, in->x, &out->data.len);
}
static inline void exec_bitos_to_strnb(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_to_strnb(
        out->str, in->len, 
        in->x, in->base,
        &out->data.len
    );
}
static inline void exec_bitos_to_strf(const void *vin, str_res *out, void *ctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_to_strf(out->str, 
        in->len, in->x, 
        in->base, in->uppercase, 
        &out->data.len
    );
}
// Instant / Streamed Printing - fput
static inline void exec_bitos_fput(const void *vin, str_res *out, void *ctx) {
    bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_fput(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) { size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; out->status = STR_SUCCESS;
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_fputb(const void *vin, str_res *out, void *ctx) {
    bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_fputb(tmp, in->x, in->base);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; out->status = STR_SUCCESS;
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_fputf(const void *vin, str_res *out, void *ctx) {
    bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_fputf(tmp, in->x, in->base, in->uppercase);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; out->status = STR_SUCCESS;
    out->data.len = len; fclose(tmp);
}
// Buffered Printing - sfput
static inline void exec_bitos_sfput(const void *vin, str_res *out, void *ctx) {
    bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_sfput(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; out->status = STR_SUCCESS;
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_sfputb(const void *vin, str_res *out, void *ctx) {
    bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_sfputb(tmp, in->x, in->base);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; out->status = STR_SUCCESS;
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_sfputf(const void *vin, str_res *out, void *ctx) {
    bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_sfputf(tmp, in->x, in->base, in->uppercase);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; out->status = STR_SUCCESS;
    out->data.len = len; fclose(tmp);
}
// Stream-based Raw Output - fwrite
static inline void exec_bitos_fwrite(const void *vin, str_res *out, void *ctx) {
    bitos_fwrite_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_fwrite(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; 
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_serialize(const void *vin, str_res *out, void *ctx) {
    const bitos_serialize_in *in = vin;
    out->type = STRING;
    out->status = bigInt_serialize(out->str, in->len, in->x);
}
// Utilities
static inline void exec_bitos_ldump(const void *vin, str_res *out, void *ctx) {
    bitos_util_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_limb_dump(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; 
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_hdump(const void *vin, str_res *out, void *ctx) {
    bitos_util_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_hexdump(tmp, in->x, in->uppercase);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; 
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_bdump(const void *vin, str_res *out, void *ctx) {
    bitos_util_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_bindump(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; 
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_info(const void *vin, str_res *out, void *ctx) {
    bitos_util_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_info(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(
            out->str + len, 
            sizeof(char), 256, tmp
        ); len += n;
        if (feof(tmp)) break;
    } out->str[len] = '\0'; 
    out->data.len = len; fclose(tmp);
}



//* ========================= STOBI EXECUTION WRAPPERS ======================= *//
// Initialization - strinint
static inline void exec_stobi_strinit(const void *vin, str_res *out, void *ctx) {
    const stobi_init_in *in = vin;
    out->type = BIGINT;
    out->status = bigInt_strinit(&out->data.bi, in->str);
}
static inline void exec_stobi_strbinit(const void *vin, str_res *out, void *ctx) {
    const stobi_init_in *in = vin;
    out->type = BIGINT;
    out->status = bigInt_strbinit(&out->data.bi, in->str, in->base);
}
static inline void exec_stobi_strninit(const void *vin, str_res *out, void *ctx) {
    const stobi_init_in *in = vin;
    out->type = BIGINT;
    out->status = bigInt_strninit(&out->data.bi, in->str, in->len);
}
static inline void exec_stobi_strnbinit(const void *vin, str_res *out, void *ctx) {
    const stobi_init_in *in = vin;
    out->type = BIGINT;
    out->status = bigInt_strnbinit(&out->data.bi, in->str, in->len, in->base);
}
// Conversions - from_str
static inline void exec_stobi_from_str(const void *vin, str_res *out, void *ctx) {
    const stobi_conv_in *in = vin;
    out->type = BIGINT;
    out->data.bi = bigInt_from_str(in->str, &out->status);
}
static inline void exec_stobi_from_strb(const void *vin, str_res *out, void *ctx) {
    const stobi_conv_in *in = vin;
    out->type = BIGINT;
    out->data.bi = bigInt_from_strb(in->str, in->base, &out->status);
}
static inline void exec_stobi_from_strn(const void *vin, str_res *out, void *ctx) {
    const stobi_conv_in *in = vin;
    out->type = BIGINT;
    out->data.bi = bigInt_from_strn(in->str, in->len, &out->status);
}
static inline void exec_stobi_from_strnb(const void *vin, str_res *out, void *ctx) {
    const stobi_conv_in *in = vin;
    out->type = BIGINT;
    out->data.bi = bigInt_from_strnb(in->str, in->len, in->base, &out->status);
}
// Default Assignments - get_str - GROWS
static inline void exec_stobi_get_str(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_get_str(&out->data.bi, in->str);
}
static inline void exec_stobi_get_strb(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_get_strb(&out->data.bi, in->str, in->base);
}
static inline void exec_stobi_get_strn(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_get_strn(&out->data.bi, in->str, in->len);
}
static inline void exec_stobi_get_strnb(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_get_strnb(&out->data.bi, in->str, in->len, in->base);
}
// Truncative Assignments - tget_str - TRUNCATE
static inline void exec_stobi_tget_str(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_tget_str(&out->data.bi, in->str);
}
static inline void exec_stobi_tget_strb(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_tget_strb(&out->data.bi, in->str, in->base);
}
static inline void exec_stobi_tget_strn(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_tget_strn(&out->data.bi, in->str, in->len);
}
static inline void exec_stobi_tget_strnb(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_tget_strnb(&out->data.bi, in->str, in->len, in->base);
}
// Safe / Strict Assignments - sget_str - RETURNS AN ERROR
static inline void exec_stobi_sget_str(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_sget_str(&out->data.bi, in->str);
}
static inline void exec_stobi_sget_strb(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_sget_strb(&out->data.bi, in->str, in->base);
}
static inline void exec_stobi_sget_strn(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_sget_strn(&out->data.bi, in->str, in->len);
}
static inline void exec_stobi_sget_strnb(const void *vin, str_res *out, void *ctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_sget_strnb(&out->data.bi, in->str, in->len, in->base);
}
// Stream-based Input Scanning - fscan
static inline void exec_stobi_fscan(const void *vin, str_res *out, void *ctx) {
    stobi_scan_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_fscan(in->stream, &out->data.bi);
}
static inline void exec_stobi_fscanb(const void *vin, str_res *out, void *ctx) {
    stobi_scan_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_fscanb(in->stream, &out->data.bi, in->base);
}
static inline void exec_stobi_fsscan(const void *vin, str_res *out, void *ctx) {
    stobi_scan_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_fsscan(in->stream, &out->data.bi);
}
static inline void exec_stobi_fsscanb(const void *vin, str_res *out, void *ctx) {
    stobi_scan_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_fsscanb(in->stream, &out->data.bi, in->base);
}
static inline void exec_stobi_ftscan(const void *vin, str_res *out, void *ctx) {
    stobi_scan_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_ftscan(in->stream, &out->data.bi);
}
static inline void exec_stobi_ftscanb(const void *vin, str_res *out, void *ctx) {
    stobi_scan_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_ftscanb(in->stream, &out->data.bi, in->base);
}
// Stream-based Raw Input - fread
static inline void exec_stobi_fread(const void *vin, str_res *out, void *ctx) {
    stobi_scan_in *in = vin;
    out->type = BIGINT; bigInt_linit(&out->data.bi, in->bi_size);
    out->status = bigInt_fread(in->stream, &out->data.bi);
}
static inline void exec_stobi_deserialize(const void *vin, str_res *out, void *ctx) {
    stobi_deserialize_in *in = vin;
    out->type = BIGINT;
    out->data.bi = bigInt_deserialize(in->str, in->len, &out->status);
}


#endif