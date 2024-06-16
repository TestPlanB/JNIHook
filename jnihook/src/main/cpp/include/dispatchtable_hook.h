#include <android/log.h>
#include <stdio.h>
#include <malloc.h>
#include <mm_malloc.h>
#include <stdatomic.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <dlfcn.h>

enum dispatch_table_type {
    MALLOC,
    CALLOC,
    FREE
};

void init_dispatch_table();

int dispatch_table_hook(enum dispatch_table_type type, void *hook_func, void **callee);

#ifndef JNIHOOK_DISPATCHTABLE_HOOK_H
#define JNIHOOK_DISPATCHTABLE_HOOK_H

typedef void *(*MallocCalloc)(size_t, size_t);

typedef void (*MallocFree)(void *);

typedef struct mallinfo (*MallocMallinfo)();

typedef void *(*MallocMalloc)(size_t);

typedef int (*MallocMallocInfo)(int, FILE *);

typedef size_t (*MallocMallocUsableSize)(const void *);

typedef void *(*MallocMemalign)(size_t, size_t);

typedef int (*MallocPosixMemalign)(void **, size_t, size_t);

typedef void *(*MallocRealloc)(void *, size_t);

typedef int (*MallocIterate)(uintptr_t, size_t, void (*)(uintptr_t, size_t, void *), void *);

typedef void (*MallocMallocDisable)();

typedef void (*MallocMallocEnable)();

typedef int (*MallocMallopt)(int, int);

typedef void *(*MallocAlignedAlloc)(size_t, size_t);

#if defined(HAVE_DEPRECATED_MALLOC_FUNCS)
typedef void* (*MallocPvalloc)(size_t);
typedef void* (*MallocValloc)(size_t);
#endif

struct MallocDispatch {
    MallocCalloc calloc;
    MallocFree free;
    MallocMallinfo mallinfo;
    MallocMalloc malloc;
    MallocMallocUsableSize malloc_usable_size;
    MallocMemalign memalign;
    MallocPosixMemalign posix_memalign;
#if defined(HAVE_DEPRECATED_MALLOC_FUNCS)
    MallocPvalloc pvalloc;
#endif
    MallocRealloc realloc;
#if defined(HAVE_DEPRECATED_MALLOC_FUNCS)
    MallocValloc valloc;
#endif
    MallocIterate malloc_iterate;
    MallocMallocDisable malloc_disable;
    MallocMallocEnable malloc_enable;
    MallocMallopt mallopt;
    MallocAlignedAlloc aligned_alloc;
    MallocMallocInfo malloc_info;
} __attribute__((aligned(32)));

struct vdso_entry {
    const char *name;
    void *fn;
};
enum {
    VDSO_CLOCK_GETTIME = 0,
    VDSO_CLOCK_GETRES,
    VDSO_GETTIMEOFDAY,
#if defined(VDSO_TIME_SYMBOL)
    VDSO_TIME,
#endif
#if defined(VDSO_RISCV_HWPROBE_SYMBOL)
    VDSO_RISCV_HWPROBE,
#endif
    VDSO_END
};

struct libc_globals {
    struct vdso_entry vdso[VDSO_END];
    long setjmp_cookie;
    uintptr_t heap_pointer_tag;
    _Atomic (bool) decay_time_enabled;
    _Atomic (bool) memtag;

    // In order to allow a complete switch between dispatch tables without
    // the need for copying each function by function in the structure,
    // use a single atomic pointer to switch.
    // The current_dispatch_table pointer can only ever be set to a complete
    // table. Any dispatch table that is pointed to by current_dispatch_table
    // cannot be modified after that. If the pointer changes in the future,
    // the old pointer must always stay valid.
    // The malloc_dispatch_table is modified by malloc debug, malloc hooks,
    // and heaprofd. Only one of these modes can be active at any given time.
    _Atomic (const struct MallocDispatch *) current_dispatch_table;
    // This pointer is only used by the allocation limit code when both a
    // limit is enabled and some other hook is enabled at the same time.
    _Atomic (const struct MallocDispatch *) default_dispatch_table;
    struct MallocDispatch malloc_dispatch_table;
};


#endif //JNIHOOK_DISPATCHTABLE_HOOK_H
