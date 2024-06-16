#include <jni.h>



typedef void (*register_native_call)(JNIEnv *, jclass, const JNINativeMethod *, jint);

int hook_jni(JNIEnv *env, jobject method, void *new_entrance, void **origin_entrance);

void unhook_jni(JNIEnv *env, jobject method, void *origin_entrance);

void set_register_natives_call(register_native_call call);


