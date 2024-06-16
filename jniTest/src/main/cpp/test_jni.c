#include <jni.h>
#include <android/log.h>

void test_jni(JNIEnv *jniEnv, jobject obj) {
    __android_log_print(ANDROID_LOG_ERROR, "hello", "%s", "I am test_jni");
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jclass cls;
    jclass binderCls;
    if (NULL == vm) return -1;
    if (JNI_OK != (*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6)) return -1;
    if (NULL == (cls = (*env)->FindClass(env, "com/example/jnitest/NativeLib"))) return -1;
    JNINativeMethod jniNativeMethods[] = {
            {"testJNI", "()V", (void *) (test_jni)}
    };
    if ((*env)->RegisterNatives(env, cls, jniNativeMethods,
                                sizeof(jniNativeMethods) / sizeof((jniNativeMethods)[0])) < 0) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}