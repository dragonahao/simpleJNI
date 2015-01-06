#include "stdafx.h"
#include <JavaIO.h>
#include <AndroidContent.h>

//////////////////////////////////////////////////////////////////////////
// AssetManager

JInputStream* JAssetManager::open(JString& fileName) {
	JInputStream* obj = new JInputStream;
	CallObjectMethod(obj, "open", "(Ljava/lang/String;)Ljava/io/InputStream;", fileName.GetJObject());
	return obj;
}

//////////////////////////////////////////////////////////////////////////
// Context

JAssetManager* JContext::getAssets() {
	JAssetManager* obj = new JAssetManager;
	CallObjectMethod(obj, "getAssets", "()Landroid/content/res/AssetManager;");
	return obj;
}

// 获得包名。
JString* JContext::getPackageName() {
	JString* obj = new JString;
	CallObjectMethod(obj, "getPackageName", "()Ljava/lang/String;");
	return obj;
}

//////////////////////////////////////////////////////////////////////////
// ContextWrapper

// JAssetManager* JContextWrapper::getAssets() {
// 	JAssetManager *obj;
// 	CallObjectMethod(&obj, "getAssets", "()Landroid/content/res/AssetManager;");
// 	return obj;
// }
