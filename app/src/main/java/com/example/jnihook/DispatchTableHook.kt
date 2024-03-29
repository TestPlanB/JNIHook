package com.example.jnihook

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.jnihook.databinding.ActivityDispatchTableHookBinding

class DispatchTableHook : AppCompatActivity() {
    private lateinit var binding: ActivityDispatchTableHookBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityDispatchTableHookBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.btnTest.setOnClickListener {
            testDispatchTableHook()
        }
    }

    external fun testDispatchTableHook()
}