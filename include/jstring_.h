#pragma once

#include <JavaBase.h>

class jstring_;

class jstring_ {
public:
	/**
	 * 构造函数。
	 * @note 构造函数会进行初始化。可以通过IsInitSuccess()函数确定构造函数初始化是否成功。
	 */
	jstring_();

	/**
	 * 构造函数。
	 * @param[in] str 字符串常量，utf编码。
	 * @note 构造函数会进行初始化。可以通过IsInitSuccess()函数确定构造函数初始化是否成功。
	 * 		   构造失败，将会抛出异常。
	 */
	jstring_(const char* strUtf);

	/**
	 * 构造函数。
	 * @param[in] jstr jstring类型的对象。
	 * @note 构造函数会进行初始化。可以通过IsInitSuccess()函数确定构造函数初始化是否成功。
	 */
	//jstring_(const jstring jstr);

	virtual ~jstring_();

	/**
	 * 重载赋值运算符。
	 * @note 不会改变类当前的初始化状态，也不会改变类的JNIEnv。
	 * 		   如果初始化获得JNIEnv失败，那么这个赋值操作将会出错。
	 * 		   如果char*转换jstring失败，将抛出异常。
	 */
	jstring_& operator=(const char* s);

	/**
	 * 重载赋值运算符。
	 * @note 不会改变类当前的初始化状态，也不会改变类的JNIEnv。
	 * 		   如果初始化获得JNIEnv失败，那么这个赋值操作将会出错。
	 */
	//jstring_& operator=(const jstring s);

	//jstring_& operator=(const jstring_& s_);

	/**
	 * 初始化是否成功。
	 * @return true: 初始化成功。
	 */
	bool IsInitSuccess();

	/**
	 * 获得jstring类型的字符串。
	 * @return 获得成功，返回jstring类型的字符串。获得失败，返回NULL。如果从未赋值，返回NULL。如果初始化失败，返回NULL。
	 */
	const jstring GetJString();

	/**
	 * 获得char类型的字符串。
	 * @return 获得成功，返回char类型的字符串。获得失败，返回NULL。如果初始化失败，也会返回NULL。
	 */
	const char* GetString();

	/**
	 * 获得字符串的字符个数。
	 * @return 返回字符串的字符个数。
	 */
	int Length();

	void SetString(const jstring jstr);

protected:
	bool mIsInitSuc;
	JNIEnvGetter mJNIEnvGetter;
	struct {
		jstring jstr;
		bool isAlloc;
	} mJString;
	struct {
		const char* charsUtf;
		const jchar* chars; //std::string stringUtf;
	} mChars;

private:
	/**
	 * 初始化。
	 */
	void init();

	/**
	 * 重置。
	 */
	void reset();

	/**
	 * 获得jstring类型的字符串。
	 * @return true: 获得成功。false：获得失败。
	 */
	bool GetJString(const char* s);

	/**
	 * 设置字符串。
	 */
	bool SetJString(jstring jstr);

	/**
	 * 构造函数。
	 * @param[in] jstr_ jstring_类型的对象。
	 * @note 构造函数会进行初始化。可以通过IsInitSuccess()函数确定构造函数初始化是否成功。
	 */
	//jstring_(jstring_& jstr_) {}
};
