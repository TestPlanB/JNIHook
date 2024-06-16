# JniHook
一个简单的工具库，专门处理Jni 函数调用Hook

原理讲解：https://juejin.cn/post/7268894037464367140

# 使用指南
## 开启prefad
```
build.gradle 中开启prefab
    buildFeatures {
        prefab true
    }
```
## 初始化hook
```
JniHook.jniHookInit()
```

## hook jni函数
需要的时候调用hook_jni函数即可 

* env:jni 环境 
* method：java层中需要hook的jni函数，即Method
* new_entrance：native层的代理函数指针
* origin_entrance：原函数指针的指针（指针的地址）
```
int hook_jni(JNIEnv *env, jobject method, void *new_entrance, void **origin_entrance)
```
返回值：1（hook 成功） 0（当前已经hook） -1（hook失败） -2(当前jni函数还没有注册，可以通过注册RegisterNatives函数监听，见set_register_natives_call)
## unhook jni函数
如果需要解除hook，调用unhook_jni
* env:jni 环境 
* method：java层中hook的jni函数，即Method
* origin_entrance：原函数指针
```
void unhook_jni(JNIEnv *env, jobject method, void *origin_entrance)
```
## set_register_natives_call 
当jni 函数还未被加载时，此时hook_jni会返回无效状态-2，因此可以监听RegisterNatives 函数调用进行查看是否有需要的jni函数正在被注册，注册之后hook_jni才会返回1
```
void set_register_natives_call(register_native_call call)
```




## 项目层级介绍
* **app下是使用例子**
* **jnihook 是jnihook的核心实现**

## 环境准备
建议直接用最新的稳定版本Android Studio打开工程。目前项目已适配`Android Studio Arctic Fox | 2022.3.1`
### 

## 感谢
[btrace]([https://www.runoob.com](https://github.com/bytedance/btrace)https://github.com/bytedance/btrace)

