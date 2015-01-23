simpleJNI
=========

介绍
=========
这个项目是为了简化在native层的编程：方便C++代码调用Java方法、Hack代码。  
这个项目是用于安全方面，所以设计之初并未在兼容性方面多做考虑，但保证在一定范围内保持稳定。  
这个项目的代码只保证在Android4.X上运行良好。

功能
=========
libJNIJava
---------
+ 获得JNIEvn。
+ C++的代码，以Java的习惯调用Java系统方法。

libHack
---------
+ native层对Java的Hook。 
+ native层对C/C++的Hook。

目录
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
