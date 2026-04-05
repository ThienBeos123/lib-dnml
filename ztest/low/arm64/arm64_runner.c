// Functions to compare
#include "../../../intrinsics/arm64/_arm64_conn.h"
#include "../../../intrinsics/zvanillc/_vanillc_conn.h"
// Utilities
#include "../../atest_ui.h"
#include "../../../sconfigs/arena.h"
// STDLIB utilities
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv) {
    _dnml_output_mode arm64_out = (argc > 1 && strcmp(argv[1], "--compact") == 0) ? DNML_COUT : DNML_VOUT;
    

    return 0;
}