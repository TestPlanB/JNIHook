
#include <android/log.h>
#include "include/jni_hook.h"

static int jni_entrance_index = -1;

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

int hook_jni(JNIEnv *env, jobject method, void *new_entrance, void **origin_entrance) {
    if (jni_entrance_index == -1) {
        return -1;
    }
    void **target_art_method = get_art_method(env, method);
    if (target_art_method[jni_entrance_index] == new_entrance) {
        return 0;
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


JNIEXPORT void JNICALL
Java_com_pika_jnihook_JniHook_jniPlaceHolder(JNIEnv *env, jclass clazz) {
}

JNIEXPORT void JNICALL
Java_com_pika_jnihook_JniHook_jniHookInitByHolder(JNIEnv *env, jobject thiz,
                                                  jobject native_place_holder) {
    init_jni_hook(env, native_place_holder,
                  (void *) Java_com_pika_jnihook_JniHook_jniPlaceHolder);
}





