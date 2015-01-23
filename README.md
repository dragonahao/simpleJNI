simpleJNI
=========

介绍
=========
这个项目是为了简化在native层的编程，方便在C++代码调用Java方法。
这个项目是用于安全方面，所以设计之初并未在兼容性方面多做考虑，这个项目的代码只保证在Android4.X上运行良好。

使用
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
