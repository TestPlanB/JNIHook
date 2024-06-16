
#include <sys/mman.h>
#include <sys/user.h>
#include <stdbool.h>
#include "test_jni_hook.h"
#include "jni_hook.h"


// 定义原始函数
static void (*test_jni_original)(JNIEnv *, jobject);

static void test_jni_hook_proxy(JNIEnv *env, jobject java_this) {
    // 先走到代理函数，然后走到原函数
    __android_log_print(ANDROID_LOG_ERROR, "hello", "%s", "test_jni_hook_proxy");

    test_jni_original(env, java_this);
}


JNIEXPORT void JNICALL
Java_com_example_jnihook_MainActivity_hooktest(JNIEnv *env, jobject thiz, jobject method) {
    int result = hook_jni(env, method, (void *) test_jni_hook_proxy, (void **) &test_jni_original);
    __android_log_print(ANDROID_LOG_ERROR, "hello", "jni hook result %d", result);
}

JNIEXPORT void JNICALL
Java_com_example_jnihook_MainActivity_unhooktest(JNIEnv *env, jobject thiz, jobject method) {
    unhook_jni(env, method, test_jni_original);

}


static void
test_register_native(JNIEnv *env, jclass c, const JNINativeMethod *methods, jint nMethods) {
    __android_log_print(ANDROID_LOG_ERROR, "hello", "当前so进行了jni方法注册 %p", (*methods).fnPtr);
}

JNIEXPORT void JNICALL
Java_com_example_jnihook_MainActivity_testRegisterNative(JNIEnv *env, jobject thiz) {
    set_register_natives_call(test_register_native);
}