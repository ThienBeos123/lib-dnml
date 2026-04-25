// Functions to compare
#include "../../../adynamol/big_numbers/bigInt_func.h"
// Utilities
#include "../../../sconfigs/memory/_scratch.h"
#include "../../../util/util.h"
#include "../_ioconv.h"
// Utility Components
#include "../iobuf.h"
#include "../_ioconv.h"

// Testing framework
#include "../../../test_ui/_strui.h"
#include "bi_indef.h"
#include "bi_exec_func.h"
#include "bi_eval_fn.h"
#include "bi_util_func.h"
// STDLIB utilities
#include <stdint.h>
#include <stdio.h>


int main(int argc, char **argv) {
    // ----------------- PRE-TEST SETUP ----------------- //
    // Parse terminal args + Setup env constants
    u8 suite_count = 8;
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode init_omode; if (argc >= 2) {
        uint8_t sesh_count = _stou64(argv[2], strlen(argv[2]));
        init_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else init_omode = DNML_VOUT;
    // Buffer Setup
    return 0;
}