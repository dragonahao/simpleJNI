#ifndef _ANDROID_CONTENT_H_
#define _ANDROID_CONTENT_H_

#include "JavaIO.h"

//////////////////////////////////////////////////////////////////////////
// AssetManager

#define JAssetManagerSuper JObject

class JAssetManager : public JAssetManagerSuper {
public:
	JInputStream* open(JString& fileName);
};

//////////////////////////////////////////////////////////////////////////
// Context

#define JContextSuper JObject

class JContext : public JContextSuper {
public:
	virtual JAssetManager* getAssets();

	/**
	 * 获得包名。
	 * @return 返回包名。
	 */
	virtual JString* getPackageName();
};

//////////////////////////////////////////////////////////////////////////
// ContextWrapper

#define JContextWrapperSuper JContext

class JContextWrapper : public JContextWrapperSuper {
public:
	//virtual JAssetManager* getAssets();
};

#endif