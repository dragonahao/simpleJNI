#include "stdafx.h"
#include "JavaNumber.h"

//////////////////////////////////////////////////////////////////////////
// Number

jbyte JNumber::byteValue() {
	return CallByteMethod("byteValue", "()B");
}

jdouble JNumber::doubleValue() {
	return CallDoubleMethod("doubleValue", "()D");
}

jfloat JNumber::floatValue() {
	return CallFloatMethod("floatValue", "()F");
}

jint JNumber::intValue() {
	return CallIntMethod("intValue", "()I");
}

jlong JNumber::longValue() {
	return CallLongMethod("longValue", "()J");
}

jshort JNumber::shortValue() {
	return CallShortMethod("shortValue", "()S");
}

//////////////////////////////////////////////////////////////////////////
// Integer

void JInteger::New(jint value) {
	CallConstructorMethod("java/lang/Integer", "(I)V", value);
}

void JInteger::New(JString& string) {
	CallConstructorMethod("java/lang/Integer", "(Ljava/lang/String;)V", string.GetJObject());
}
