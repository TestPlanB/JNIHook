
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
Java_com_example_jnihook_MainActivity_testJniHook(JNIEnv *env, jobject thiz) {
    __android_log_print(ANDROID_LOG_ERROR, "hello", "%s", "test_jni_hook");
}

JNIEXPORT void JNICALL
Java_com_example_jnihook_MainActivity_hooktest(JNIEnv *env, jobject thiz, jobject method) {
    hook_jni(env, method, (void *) test_jni_hook_proxy, (void **) &test_jni_original);
}

JNIEXPORT void JNICALL
Java_com_example_jnihook_MainActivity_unhooktest(JNIEnv *env, jobject thiz, jobject method) {
    unhook_jni(env,method,test_jni_original);
}