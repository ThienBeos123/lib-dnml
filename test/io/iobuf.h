#ifndef _BIGINT_IOBUF_H
#define _BIGINT_IOBUF_H


#include "../../../sconfigs/memory/_scratch.h"
#include "../_ioconv.h"
#include <stdlib.h>


static bool iobuf_init = false; 
static local_thread dnml_dratch _BIGINT_IOBUF;
static inline dnml_dratch* _use_iobuf(void) {
    // Support 512 limbs (the gold standard)
    if (!iobuf_init || _BIGINT_IOBUF.base == NULL) { 
        init_dratch(&_BIGINT_IOBUF, 1024);
    } return &_BIGINT_IOBUF;
}

static inline void* iobuf_alloc(size_t block_count) { 
    dnml_dratch* iobuf_p = _use_iobuf();
    return dratch_alloc(iobuf_p, block_count); 
}



#endif