#include "stdafx.h"
#include <JavaContainer.h>
#include <JavaString.h>
#include <JavaIO.h>

//////////////////////////////////////////////////////////////////////////
// File

void JFile::New(IN JString& path) {
	CallConstructorMethod("java/io/File", "(Ljava/lang/String;)V", path.GetJObject());
}

jboolean JFile::mkdir() {
	return CallBooleanMethod("mkdir", "()Z");;
}

jboolean JFile::mkdirs() {
	return CallBooleanMethod("mkdirs", "()Z");;
}

jboolean JFile::exists() {
	return CallBooleanMethod("exists", "()Z");;
}

JString* JFile::getAbsolutePath() {
	JString* obj = new JString;
	CallObjectMethod(obj, "getAbsolutePath", "()Ljava/lang/String;");
	return obj;
}

JString* JFile::getParent() {
	JString* obj = new JString;
	CallObjectMethod(obj, "getParent", "()Ljava/lang/String;");
	return obj;
}

//////////////////////////////////////////////////////////////////////////
// InputStream

int JInputStream::read() {
	return CallIntMethod("read", "()I");
}

int JInputStream::read(JArray<jbyte>& buffer) {
	return CallIntMethod("read", "([B)I", buffer.GetJObject());
}

int JInputStream::read(JArray<jbyte>& buffer, jint byteOffset, jint byteCount) {
	return CallIntMethod("read", "([BII)I", buffer.GetJObject(), byteOffset, byteCount);
}

void JInputStream::close() {
	CallVoidMethod("close", "()V");
}

long JInputStream::skip(jlong byteCount) {
	return CallLongMethod("skip", "(J)J", byteCount);
}

//////////////////////////////////////////////////////////////////////////
// FileInputStream

void JFileInputStream::New(JFile& file) {
	CallConstructorMethod("java/io/FileInputStream", "(Ljava/io/File;)V", file.GetJObject());
}

void JFileInputStream::New(JString& path) {
	CallConstructorMethod("java/io/FileInputStream", "(Ljava/lang/String;)V", path.GetJObject());
}

//////////////////////////////////////////////////////////////////////////
// OutputStream

void JOutputStream::write(JArray<jbyte>& buffer) {
	CallVoidMethod("write", "([B)V", buffer.GetJObject());
}

void JOutputStream::write(JArray<jbyte>& buffer, int offset, int count) {
	CallVoidMethod("write", "([BII)V", buffer.GetJObject(), offset, count);
}

void JOutputStream::write(jint oneByte) {
	CallVoidMethod("write", "(I)V", oneByte);
}

void JOutputStream::close() {
	CallVoidMethod("close", "()V");
}

//////////////////////////////////////////////////////////////////////////
// FileOutputStream

void JFileOutputStream::New(JFile& file) {
	CallConstructorMethod("java/io/FileOutputStream", "(Ljava/io/File;)V", file.GetJObject());
}

void JFileOutputStream::New(JFile& file, jboolean append) {
	CallConstructorMethod("java/io/FileOutputStream", "(Ljava/io/File;Z)V", file.GetJObject(), append);
}

void JFileOutputStream::New(JString& path) {
	CallConstructorMethod("java/io/FileOutputStream", "(Ljava/lang/String;)V", path.GetJObject());
}

void JFileOutputStream::New(JString& path, jboolean append) {
	CallConstructorMethod("java/io/FileOutputStream", "(Ljava/lang/String;Z)V", path.GetJObject(), append);
}
