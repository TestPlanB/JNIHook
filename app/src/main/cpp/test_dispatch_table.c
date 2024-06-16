#include <jni.h>
#include <android/log.h>
#include <dispatchtable_hook.h>


static void *origin_calloc;

static void *my_calloc(size_t __item_count, size_t __item_size) {
    __android_log_print(ANDROID_LOG_ERROR, "hello", "calloc hook %zu %zu", __item_count,
                        __item_size);
    MallocCalloc c = (MallocCalloc) origin_calloc;
    return c(__item_count, __item_size);
}

JNIEXPORT void JNICALL
Java_com_example_jnihook_DispatchTableHook_testDispatchTableHook(JNIEnv *env, jobject thiz) {
    init_dispatch_table();
    dispatch_table_hook(CALLOC, my_calloc, &origin_calloc);
    calloc(1,4);
}
