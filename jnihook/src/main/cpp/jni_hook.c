
#include <android/log.h>
#include <xdl.h>
#include <sys/mman.h>
#include <sys/user.h>
#include "include/jni_hook.h"
#include "dl_symbol_search.h"

static int jni_entrance_index = -1;
static void *jni_stub = NULL;
struct JNINativeInterface *original_functions;
static void *backup;
#define PAGE_START(addr) ((addr) & (uintptr_t)PAGE_MASK)
static register_native_call register_natives_call = NULL;

static void **get_art_method(JNIEnv *env, jobject foo) {
    void **fooArtMethod;
    if (android_get_device_api_level() >= 30) {
        jclass Executable = (*env)->FindClass(env, "java/lang/reflect/Executable");
        jfieldID artMethodField = (*env)->GetFieldID(env, Executable, "artMethod", "J");
        fooArtMethod = (void **) (*env)->GetLongField(env, foo, artMethodField);
    } else {
        fooArtMethod = (void **) (*env)->FromReflectedMethod(env, foo);
    }
    return fooArtMethod;
}

static void init_jni_hook(JNIEnv *env, jobject foo, void *fooJNI) {
    void **fooArtMethod = get_art_method(env, foo);
    for (int i = 0; i < 50; ++i) {
        if (fooArtMethod[i] == fooJNI) {
            jni_entrance_index = i;
            break;
        }
    }
}

static jint
hook_jni_RegisterNatives(JNIEnv *env, jclass c, const JNINativeMethod *methods, jint nMethods) {
    jint ret = ((jint (*)(JNIEnv *, jclass, const JNINativeMethod *, jint)) backup)(env, c, methods,
                                                                                    nMethods);
    __android_log_print(ANDROID_LOG_ERROR, "jnihook", "hook_jni_RegisterNatives %p",
                        (*methods).fnPtr);
    if (register_natives_call != NULL) {
        register_natives_call(env, c, methods, nMethods);
    }
    return ret;
}

int hook_jni(JNIEnv *env, jobject method, void *new_entrance, void **origin_entrance) {
    if (jni_entrance_index == -1) {
        return -1;
    }
    void **target_art_method = get_art_method(env, method);
    if (target_art_method[jni_entrance_index] == new_entrance) {
        return 0;
    }
    if (target_art_method[jni_entrance_index] == jni_stub ||
        target_art_method[jni_entrance_index] == NULL) {
        // 当前jni函数还未加载，可以注册RegisterNative监听
        return -2;
    }
    *origin_entrance = target_art_method[jni_entrance_index];
    target_art_method[jni_entrance_index] = new_entrance;
    return 1;
}

void unhook_jni(JNIEnv *env, jobject method, void *origin_entrance) {
    void **target_art_method = get_art_method(env, method);
    if (target_art_method[jni_entrance_index] == origin_entrance) {
        return;
    }
    target_art_method[jni_entrance_index] = origin_entrance;
}

void set_register_natives_call(register_native_call call) {
    register_natives_call = call;
}


JNIEXPORT void JNICALL
Java_com_pika_jnihook_JniHook_jniPlaceHolder(JNIEnv *env, jclass clazz) {
}

JNIEXPORT void JNICALL
Java_com_pika_jnihook_JniHook_jniHookInitByHolder(JNIEnv *env, jobject thiz,
                                                  jobject native_place_holder) {
    init_jni_hook(env, native_place_holder,
                  (void *) Java_com_pika_jnihook_JniHook_jniPlaceHolder);
    void *handle = xdl_open("libart.so", XDL_DEFAULT);
    jni_stub = find_symbol(handle, "art_jni_dlsym_lookup_stub");
    original_functions = *env;
    int offset = offsetof(struct JNINativeInterface, RegisterNatives);
    void **target = (void **) (((char *) original_functions) + offset);
    uintptr_t start_addr = PAGE_START((uintptr_t) (target));
    uintptr_t end_addr = PAGE_START((uintptr_t) target + sizeof(uintptr_t) - 1) + PAGE_SIZE;
    size_t size = end_addr - start_addr;

    if (mprotect((void *) start_addr, size, PROT_WRITE | PROT_READ) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "jnihook", "%s", "mprotect fail");
    }
    backup = *target;
    *target = hook_jni_RegisterNatives;
    if (mprotect((void *) start_addr, size, PROT_READ) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "jnihook", "%s", "mprotect fail");
    }
}








