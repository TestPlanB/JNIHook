#include <jni.h>

int hook_jni(JNIEnv *env, jobject method, void *newEntrance, void **origin_entrance);

void unhook_jni(JNIEnv *env, jobject method, void *origin_entrance);