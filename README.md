simpleJNI
=========

介绍
=========
为了简化native层的编程：方便C++代码调用Java方法、Hack代码。  
这个项目目的是用于安全方面，所以设计之初并未在兼容性方面多做考虑，但保证在一定范围内保持稳定。  
这个项目的代码只保证在Android4.X上运行良好。  
  
本项目将源码编译成了工具（so、a、jar），看过使用说明后可以直接使用。

功能
=========
libJNIJava
---------
+ 获得JNIEvn。
+ C++代码封装JNI调用，在native层以Java的习惯调用Java系统方法。如C++类"JFile"，对应Java层File类的用法。

libHack
---------
+ native层对Java的Hook。 
+ native层对C/C++的Hook。

目录介绍
=========
include: 头文件。  
lib: 动态库、静态库、jar包。  
samples: 例子。  
src: C/C++源码、Java源码。

工具的使用
=========
如果无法链接libJNIJava.a库，那么Application.mk请写成如下内容：
```
APP_STL := gnustl_static
NDK_TOOLCHAIN_VERSION=4.6
APP_CPPFLAGS := -frtti -fexceptions -std=gnu++11
```

libJNIJava.a
---------
需要在Android.mk中链接动态库libandroid_runtime_arm。

libHack.a
---------
需要链接动态库libdvm_arm。
