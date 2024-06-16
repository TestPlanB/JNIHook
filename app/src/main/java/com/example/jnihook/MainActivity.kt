package com.example.jnihook

import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.jnihook.databinding.ActivityMainBinding
import com.example.jnitest.NativeLib
import com.pika.jnihook.JniHook
import java.lang.reflect.Method

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        // 初始化hook
        JniHook.jniHookInit()

        val hookMethod = NativeLib::class.java.getDeclaredMethod("testJNI")
        val nativeTest = NativeLib()

        binding.originalCall.setOnClickListener {
            // 正常调用函数
            nativeTest.initSo()
            nativeTest.testJNI()
        }
        binding.jnihook.setOnClickListener {
            hooktest(hookMethod)
        }

        binding.unhook.setOnClickListener {
            unhooktest(hookMethod)
        }

        binding.registerNativeCall.setOnClickListener {
            testRegisterNative()
        }

        binding.entry.setOnClickListener {
            startActivity(Intent(this, DispatchTableHook::class.java))
        }

    }

    // 需要hook的函数
    companion object {
        init {
            System.loadLibrary("jnihooktest")
        }
    }

    // 替换的函数
    external fun hooktest(method: Method)
    external fun unhooktest(method: Method)

    external fun testRegisterNative()


}