#include "include/dispatchtable_hook.h"
#include "include/jni_hook.h"
#include "dl_symbol_search.h"
#include <sys/mman.h>
#include <malloc.h>
#include <xdl.h>
#include <android/log.h>

static struct MallocDispatch *dynamic;
static const struct MallocDispatch *pika_dispatch_table;


void init_dispatch_table() {
    void *handle = xdl_open("libc.so", XDL_DEFAULT);
    struct MallocDispatch *c_dispatcher = ((struct MallocDispatch *(*)()) find_symbol(
            handle, "_Z23NativeAllocatorDispatchv"))();
    if (c_dispatcher == NULL) {
        return;
    }
    pika_dispatch_table = c_dispatcher;
    dynamic = malloc(sizeof(struct MallocDispatch));
    dynamic->calloc = pika_dispatch_table->calloc;
    dynamic->free = pika_dispatch_table->free;
    dynamic->mallinfo = pika_dispatch_table->mallinfo;
    dynamic->malloc = pika_dispatch_table->malloc;
    dynamic->malloc_usable_size = pika_dispatch_table->malloc_usable_size;
    dynamic->memalign = pika_dispatch_table->memalign;
    dynamic->posix_memalign = pika_dispatch_table->posix_memalign;
#if defined(HAVE_DEPRECATED_MALLOC_FUNCS)
    dynamic->pvalloc = predispatcher->pvalloc;
#endif
    dynamic->realloc = pika_dispatch_table->realloc;
#if defined(HAVE_DEPRECATED_MALLOC_FUNCS)
    dynamic->valloc = predispatcher->valloc;
#endif
    dynamic->malloc_iterate = pika_dispatch_table->malloc_iterate;
    dynamic->malloc_disable = pika_dispatch_table->malloc_disable;
    dynamic->malloc_enable = pika_dispatch_table->malloc_enable;
    dynamic->mallopt = pika_dispatch_table->mallopt;
    dynamic->aligned_alloc = pika_dispatch_table->aligned_alloc;
    dynamic->malloc_info = pika_dispatch_table->malloc_info;
}


int dispatch_table_hook(enum dispatch_table_type type, void *hook_func, void **callee) {
    void *handle = xdl_open("libc.so", XDL_DEFAULT);
    struct libc_globals *c_global = (struct libc_globals *) find_symbol(handle,
                                                                        "__libc_globals");
    if (mprotect(c_global, PAGE_SIZE, PROT_WRITE | PROT_READ) == -1) {
        return 0;
    }

    switch (type) {
        case MALLOC: {
            *callee = pika_dispatch_table->malloc;
            dynamic->malloc = hook_func;
            break;
        }
        case CALLOC: {
            *callee = pika_dispatch_table->calloc;
            dynamic->calloc = hook_func;
            break;
        }
        case FREE: {
            *callee = pika_dispatch_table->free;
            dynamic->free = hook_func;
            break;
        }
            // You can add any function in here which should at the dispatch table
        default: {
            return 0;
        }
    }

    c_global->malloc_dispatch_table = *dynamic;
    atomic_store(&c_global->default_dispatch_table, dynamic);

    if (c_global->current_dispatch_table == NULL) {
        atomic_store(&c_global->current_dispatch_table,
                     dynamic);
    }

    if (mprotect(c_global, PAGE_SIZE, PROT_READ) == -1) {
        return 0;
    }
    return 1;

}
