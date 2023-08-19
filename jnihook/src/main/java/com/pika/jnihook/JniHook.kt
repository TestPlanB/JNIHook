package com.pika.jnihook

import java.lang.reflect.Method

object JniHook {

    fun jniHookInit() {
        jniPlaceHolder()
        val placeHolder = JniHook::class.java.getDeclaredMethod("jniPlaceHolder")
        jniHookInitByHolder(placeHolder)
    }

    private external fun jniHookInitByHolder(nativePlaceHolder: Method)


    private external fun jniPlaceHolder()


}