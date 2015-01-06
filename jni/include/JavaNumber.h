#ifndef _JAVA_NUMBER_H_
#define _JAVA_NUMBER_H_

//////////////////////////////////////////////////////////////////////////
// Number

#define JNumberSuper JObject

class JNumber : public JNumberSuper {
public:
	jbyte byteValue();

	jdouble doubleValue();

	jfloat floatValue();

	jint intValue();

	jlong longValue();

	jshort shortValue();
};

//////////////////////////////////////////////////////////////////////////
// Integer

#define JIntegerSuper JNumber

class JInteger : public JIntegerSuper {
public:
	void New(jint value);

	void New(JString& string);
};

#endif