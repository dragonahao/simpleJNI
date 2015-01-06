#include "../stdafx.h"
#include "./JavaContainer.h"

// namespace java_util {
// 
// JArray::JArray() : JArraySuper(), mArray(NULL) {}
// 
// JArray::~JArray() {
// 	if (NULL != mArray) {
// 		mJNIEnvGetter.get()->Dele
// 	}
// }
// 
// // 获得数组长度。
// int JArray::GetFlength(){
// 	return mJNIEnvGetter.get()->GetArrayLength((jarray)mJObject);
// }
// 
// void JArray::InitJObject(IN jarray obj, IN char type) {
// 	JNIEnv* env = mJNIEnvGetter.get();
// 
// 	switch (type) {
// 	case 'Z':
// 		env->GetBooleanArrayElements(obj);
// 		break;
// 	}
// }
// }
// 
// }
