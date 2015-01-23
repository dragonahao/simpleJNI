#ifndef _JAVA_IO_H_
#define _JAVA_IO_H_

//////////////////////////////////////////////////////////////////////////
// File

#define JFileSuper JObject

class JFile : public JFileSuper
{
public:
	void New(IN JString& path);

	jboolean mkdir();

	jboolean mkdirs();

	jboolean exists();

	JString* getAbsolutePath();

	JString* getParent();
};

//////////////////////////////////////////////////////////////////////////
// InputStream

#define JInputStreamSuper JObject

class JInputStream : public JInputStreamSuper {
public:
	virtual int read();
	
	virtual int read(JArray<jbyte>& buffer);

	virtual int read(JArray<jbyte>& buffer, jint byteOffset, jint byteCount);

	virtual void close();

	virtual long skip(jlong byteCount);
};

//////////////////////////////////////////////////////////////////////////
// FileInputStream

#define JFileInputStreamSuper JInputStream

class JFileInputStream : public JFileInputStreamSuper {
public:
	void New(JFile& file);
	void New(JString& path);
};

//////////////////////////////////////////////////////////////////////////
// OutputStream

#define JOutputStreamSuper JObject

class JOutputStream : public JOutputStreamSuper {
public:
	virtual void write(JArray<jbyte>& buffer);

	virtual void write(JArray<jbyte>& buffer, int offset, int count);

	void write(jint oneByte);

	virtual void close();
};

//////////////////////////////////////////////////////////////////////////
// FileOutputStream

#define JFileOutputStreamSuper JOutputStream

class JFileOutputStream : public JFileOutputStreamSuper {
public:
	void New(JFile& file);
	void New(JFile& file, jboolean append);
	void New(JString& path);
	void New(JString& path, jboolean append);
};

#endif