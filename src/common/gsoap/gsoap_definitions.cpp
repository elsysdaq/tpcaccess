#include <common/gsoap-gen/TransPC_Server.nsmap>

#include "gsoap-gen/soapH.h"

// Centrally define some symbols which are required for cleanly consuming gsoap

#ifdef _LINUX
__attribute__((used))
#endif
ns__Data::ns__Data() {
    __ptr   = NULL;
    __size  = 0;
    id      = NULL;
    type    = NULL;
    options = NULL;
}
