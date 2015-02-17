#pragma once

//////////////////////////////////////////////////////////////////////////
// ClassLoader

#define JClassLoaderSuper JObject

class JClassLoader : public JClassLoaderSuper {
public:
	static JClassLoader* getSystemClassLoader();
};
