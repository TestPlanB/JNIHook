#include <xdl.h>
#include "dl_symbol_search.h"

void *find_symbol(void *handle, const char *sym_name) {
    void *addr = xdl_sym(handle, sym_name, NULL);
    if (NULL == addr) {
        addr = xdl_dsym(handle, sym_name, NULL);
    }
    return addr;
}
