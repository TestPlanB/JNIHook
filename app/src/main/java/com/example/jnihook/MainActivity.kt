package com.example.jnihook

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.example.jnihook.databinding.ActivityMainBinding
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

        val hookMethod = MainActivity::class.java.getDeclaredMethod("testJniHook")

        binding.originalCall.setOnClickListener {
            // 正常调用函数
            testJniHook()
        }
        binding.jnihook.setOnClickListener {
            hooktest(hookMethod)
        }

        binding.unhook.setOnClickListener {
            unhooktest(hookMethod)
        }

    }

    companion object {
        init {
            System.loadLibrary("jnihooktest")
        }
    }

    // 需要hook的函数
    external fun testJniHook()

    // 替换的函数
    external fun hooktest(method: Method)
    external fun unhooktest(method: Method)
}