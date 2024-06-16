package com.example.jnitest

class NativeLib {

    // 设置为调用方法后延迟加载
    fun initSo(){
        System.loadLibrary("jnitest")
    }
    external fun testJNI()
}