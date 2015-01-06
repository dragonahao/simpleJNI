/*
 * Copyright (c) 1997, 2006, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Object_4.h"
#include "JniUtil.h"

/* Due to a bug in the win32 C runtime library strings
 * such as "z:" need to be appended with a "." so we
 * must allocate at least 4 bytes to allow room for
 * this expansion. See 4235353 for details.
 */
#define MALLOC_MIN4(len) ((char *)malloc((len) + 1 < 4 ? 4 : (len) + 1))


static int utf16_utf8ByteLen(const u2* utf16Str, int len)
{
    int utf8Len = 0;

    while (len--) {
        unsigned int uic = *utf16Str++;

        /*
         * The most common case is (uic > 0 && uic <= 0x7f).
         */
        if (uic == 0 || uic > 0x7f) {
            if (uic > 0x07ff)
                utf8Len += 3;
            else /*(uic > 0x7f || uic == 0) */
                utf8Len += 2;
        } else
            utf8Len++;
    }
    return utf8Len;
}

static void convertUtf16ToUtf8(char* utf8Str, const u2* utf16Str, int len)
{
    assert(len >= 0);

    while (len--) {
        unsigned int uic = *utf16Str++;

        /*
         * The most common case is (uic > 0 && uic <= 0x7f).
         */
        if (uic == 0 || uic > 0x7f) {
            if (uic > 0x07ff) {
                *utf8Str++ = (uic >> 12) | 0xe0;
                *utf8Str++ = ((uic >> 6) & 0x3f) | 0x80;
                *utf8Str++ = (uic & 0x3f) | 0x80;
            } else /*(uic > 0x7f || uic == 0)*/ {
                *utf8Str++ = (uic >> 6) | 0xc0;
                *utf8Str++ = (uic & 0x3f) | 0x80;
            }
        } else {
            *utf8Str++ = uic;
        }
    }

    *utf8Str = '\0';
}

static char* dvmCreateCstrFromString(StringObject* jstr)
{
    char* newStr;
    ArrayObject* chars;
    int len, byteLen, offset;
    const u2* data;

    if (jstr == NULL)
        return NULL;

    len = dvmGetFieldInt((Object*) jstr, STRING_FIELDOFF_COUNT);
    offset = dvmGetFieldInt((Object*) jstr, STRING_FIELDOFF_OFFSET);
    chars = (ArrayObject*) dvmGetFieldObject((Object*) jstr,
                                STRING_FIELDOFF_VALUE);
    data = (const u2*) chars->contents + offset;
    assert(offset + len <= (int) chars->length);

    byteLen = utf16_utf8ByteLen(data, len);
    newStr = (char*) malloc(byteLen+1);
    if (newStr == NULL)
        return NULL;
    convertUtf16ToUtf8(newStr, data, len);

    return newStr;
}

/**
 * Throw a Java exception by name. Similar to SignalError.
 */
void JNICALL
JNU_ThrowByName(JNIEnv *env, const char *name, const char *msg)
{
    jclass cls = env->FindClass(name);

    if (cls != 0) /* Otherwise an exception has already been thrown */
        env->ThrowNew(cls, msg);
}

/* JNU_Throw common exceptions */

void JNICALL
JNU_ThrowNullPointerException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/NullPointerException", msg);
}

void JNICALL
JNU_ThrowArrayIndexOutOfBoundsException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/ArrayIndexOutOfBoundsException", msg);
}

void JNICALL
JNU_ThrowOutOfMemoryError(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/OutOfMemoryError", msg);
}

void JNICALL
JNU_ThrowIllegalArgumentException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/IllegalArgumentException", msg);
}

void JNICALL
JNU_ThrowIllegalAccessError(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/IllegalAccessError", msg);
}

void JNICALL
JNU_ThrowIllegalAccessException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/IllegalAccessException", msg);
}

void JNICALL
JNU_ThrowInternalError(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/InternalError", msg);
}

void JNICALL
JNU_ThrowNoSuchFieldException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/NoSuchFieldException", msg);
}

void JNICALL
JNU_ThrowNoSuchMethodException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/NoSuchMethodException", msg);
}

void JNICALL
JNU_ThrowClassNotFoundException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/ClassNotFoundException", msg);
}

void JNICALL
JNU_ThrowNumberFormatException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/NumberFormatException", msg);
}

void JNICALL
JNU_ThrowIOException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/io/IOException", msg);
}

void JNICALL
JNU_ThrowNoSuchFieldError(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/NoSuchFieldError", msg);
}

void JNICALL
JNU_ThrowNoSuchMethodError(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/NoSuchMethodError", msg);
}

void JNICALL
JNU_ThrowStringIndexOutOfBoundsException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/StringIndexOutOfBoundsException", msg);
}

void JNICALL
JNU_ThrowInstantiationException(JNIEnv *env, const char *msg)
{
    JNU_ThrowByName(env, "java/lang/InstantiationException", msg);
}


/* Throw an exception by name, using the string returned by
 * JVM_LastErrorString for the detail string.  If the last-error
 * string is NULL, use the given default detail string.
 */
void JNICALL
JNU_ThrowByNameWithLastError(JNIEnv *env, const char *name,
                             const char *defaultDetail)
{
    char buf[256];
    int n = 0; // JVM_GetLastErrorString(buf, sizeof(buf));

    if (n > 0) {
        jstring s = JNU_NewStringPlatform(env, buf);
        if (s != NULL) {
            jobject x = JNU_NewObjectByName(env, name,
                                            "(Ljava/lang/String;)V", s);
            if (x != NULL) {
                // env->Throw(x);
            }
        }
    }
    if (!env->ExceptionOccurred()) {
        JNU_ThrowByName(env, name, defaultDetail);
    }
}

/* Throw an IOException, using the last-error string for the detail
 * string.  If the last-error string is NULL, use the given default
 * detail string.
 */
void JNICALL
JNU_ThrowIOExceptionWithLastError(JNIEnv *env, const char *defaultDetail)
{
    JNU_ThrowByNameWithLastError(env, "java/io/IOException", defaultDetail);
}


jvalue JNICALL
JNU_CallStaticMethodByName(JNIEnv *env,
                           jboolean *hasException,
                           const char *class_name,
                           const char *name,
                           const char *signature,
                           ...)
{
    jclass clazz;
    jmethodID mid;
    va_list args;
    jvalue result;
    const char *p = signature;

    /* find out the return type */
    while (*p && *p != ')')
        p++;
    p++;

    result.i = 0;

    if (env->EnsureLocalCapacity(3) < 0)
        goto done2;

    clazz = env->FindClass(class_name);
    if (clazz == 0)
        goto done2;
    mid = env->GetStaticMethodID(clazz, name, signature);
    if (mid == 0)
        goto done1;
    va_start(args, signature);
    switch (*p) {
    case 'V':
        env->CallStaticVoidMethodV(clazz, mid, args);
        break;
    case '[':
    case 'L':
        result.l = env->CallStaticObjectMethodV(clazz, mid, args);
        break;
    case 'Z':
        result.z = env->CallStaticBooleanMethodV(clazz, mid, args);
        break;
    case 'B':
        result.b = env->CallStaticByteMethodV(clazz, mid, args);
        break;
    case 'C':
        result.c = env->CallStaticCharMethodV(clazz, mid, args);
        break;
    case 'S':
        result.s = env->CallStaticShortMethodV(clazz, mid, args);
        break;
    case 'I':
        result.i = env->CallStaticIntMethodV(clazz, mid, args);
        break;
    case 'J':
        result.j = env->CallStaticLongMethodV(clazz, mid, args);
        break;
    case 'F':
        result.f = env->CallStaticFloatMethodV(clazz, mid, args);
        break;
    case 'D':
        result.d = env->CallStaticDoubleMethodV(clazz, mid, args);
        break;
    default:
        env->FatalError("JNU_CallStaticMethodByName: illegal signature");
    }
    va_end(args);

 done1:
    env->DeleteLocalRef(clazz);
 done2:
    if (hasException) {
        *hasException = env->ExceptionCheck();
    }
    return result;
}

jvalue JNICALL
JNU_CallMethodByName(JNIEnv *env,
                     jboolean *hasException,
                     jobject obj,
                     const char *name,
                     const char *signature,
                     ...)
{
    jvalue result;
    va_list args;

    va_start(args, signature);
    result = JNU_CallMethodByNameV(env, hasException, obj, name, signature,
                                   args);
    va_end(args);

    return result;
}


jvalue JNICALL
JNU_CallMethodByNameV(JNIEnv *env,
                      jboolean *hasException,
                      jobject obj,
                      const char *name,
                      const char *signature,
                      va_list args)
{
    jclass clazz;
    jmethodID mid;
    jvalue result;
    const char *p = signature;

    /* find out the return type */
    while (*p && *p != ')')
        p++;
    p++;

    result.i = 0;

    if (env->EnsureLocalCapacity(3) < 0)
        goto done2;

    clazz = env->GetObjectClass(obj);
    mid = env->GetMethodID(clazz, name, signature);
    if (mid == 0)
        goto done1;

    switch (*p) {
    case 'V':
        env->CallVoidMethodV(obj, mid, args);
        break;
    case '[':
    case 'L':
        result.l = env->CallObjectMethodV(obj, mid, args);
        break;
    case 'Z':
        result.z = env->CallBooleanMethodV(obj, mid, args);
        break;
    case 'B':
        result.b = env->CallByteMethodV(obj, mid, args);
        break;
    case 'C':
        result.c = env->CallCharMethodV(obj, mid, args);
        break;
    case 'S':
        result.s = env->CallShortMethodV(obj, mid, args);
        break;
    case 'I':
        result.i = env->CallIntMethodV(obj, mid, args);
        break;
    case 'J':
        result.j = env->CallLongMethodV(obj, mid, args);
        break;
    case 'F':
        result.f = env->CallFloatMethodV(obj, mid, args);
        break;
    case 'D':
        result.d = env->CallDoubleMethodV(obj, mid, args);
        break;
    default:
        env->FatalError("JNU_CallMethodByNameV: illegal signature");
    }
 done1:
    env->DeleteLocalRef(clazz);
 done2:
    if (hasException) {
        *hasException = env->ExceptionCheck();
    }
    return result;
}

jobject JNICALL
JNU_NewObjectByName(JNIEnv *env, const char *class_name,
                    const char *constructor_sig, ...)
{
    jobject obj = NULL;

    jclass cls = 0;
    jmethodID cls_initMID;
    va_list args;

    if (env->EnsureLocalCapacity(2) < 0)
        goto done;

    cls = env->FindClass(class_name);
    if (cls == 0) {
        goto done;
    }
    cls_initMID  = env->GetMethodID(cls,
                                       "<init>", constructor_sig);
    if (cls_initMID == NULL) {
        goto done;
    }
    va_start(args, constructor_sig);
    obj = env->NewObjectV(cls, cls_initMID, args);
    va_end(args);

 done:
    env->DeleteLocalRef(cls);
    return obj;
}

/* Optimized for char set ISO_8559_1 */
static jstring
newString8859_1(JNIEnv *env, const char *str)
{
    int len = (int)strlen(str);
    jchar buf[512];
    jchar *str1;
    jstring result;
    int i;

    if (len > 512) {
        str1 = (jchar *)malloc(len * sizeof(jchar));
        if (str1 == 0) {
            JNU_ThrowOutOfMemoryError(env, 0);
            return 0;
        }
    } else
        str1 = buf;

    for (i=0;i<len;i++)
        str1[i] = (unsigned char)str[i];
    result = env->NewString(str1, len);
    if (str1 != buf)
        free(str1);
    return result;
}

static const char*
getString8859_1Chars(JNIEnv *env, jstring jstr)
{
    int i;
    char *result;
    jint len = env->GetStringLength(jstr);
    const jchar *str = env->GetStringCritical(jstr, 0);
    if (str == 0) {
        return 0;
    }

    result = MALLOC_MIN4(len);
    if (result == 0) {
        env->ReleaseStringCritical(jstr, str);
        JNU_ThrowOutOfMemoryError(env, 0);
        return 0;
    }

    for (i=0; i<len; i++) {
        jchar unicode = str[i];
        if (unicode <= 0x00ff)
            result[i] = unicode;
        else
            result[i] = '?';
    }

    result[len] = 0;
    env->ReleaseStringCritical(jstr, str);
    return result;
}


/* Optimized for char set ISO646-US (us-ascii) */
static jstring
newString646_US(JNIEnv *env, const char *str)
{
    int len = strlen(str);
    jchar buf[512];
    jchar *str1;
    jstring result;
    int i;

    if (len > 512) {
        str1 = (jchar *)malloc(len * sizeof(jchar));
        if (str1 == 0) {
            JNU_ThrowOutOfMemoryError(env, 0);
            return 0;
        }
    } else
        str1 = buf;

    for (i=0; i<len; i++) {
        unsigned char c = (unsigned char)str[i];
        if (c <= 0x7f)
            str1[i] = c;
        else
            str1[i] = '?';
    }

    result = env->NewString(str1, len);
    if (str1 != buf)
        free(str1);
    return result;
}

static const char*
getString646_USChars(JNIEnv *env, jstring jstr)
{
    int i;
    char *result;
    jint len = env->GetStringLength(jstr);
    const jchar *str = env->GetStringCritical(jstr, 0);
    if (str == 0) {
        return 0;
    }

    result = MALLOC_MIN4(len);
    if (result == 0) {
        env->ReleaseStringCritical(jstr, str);
        JNU_ThrowOutOfMemoryError(env, 0);
        return 0;
    }

    for (i=0; i<len; i++) {
        jchar unicode = str[i];
        if (unicode <= 0x007f )
            result[i] = unicode;
        else
            result[i] = '?';
    }

    result[len] = 0;
    env->ReleaseStringCritical(jstr, str);
    return result;
}

/* enumeration of c1 row from Cp1252 */
static int cp1252c1chars[32] = {
    0x20AC,0xFFFD,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,
    0x02C6,0x2030,0x0160,0x2039,0x0152,0xFFFD,0x017D,0xFFFD,
    0xFFFD,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
    0x02Dc,0x2122,0x0161,0x203A,0x0153,0xFFFD,0x017E,0x0178
};

/* Optimized for char set Cp1252 */
static jstring
newStringCp1252(JNIEnv *env, const char *str)
{
    int len = (int) strlen(str);
    jchar buf[512];
    jchar *str1;
    jstring result;
    int i;
    if (len > 512) {
        str1 = (jchar *)malloc(len * sizeof(jchar));
        if (str1 == 0) {
            JNU_ThrowOutOfMemoryError(env, 0);
            return 0;
        }
    } else
        str1 = buf;

    for (i=0; i<len; i++) {
        unsigned char c = (unsigned char)str[i];
        if ((c >= 0x80) && (c <= 0x9f))
            str1[i] = cp1252c1chars[c-128];
        else
            str1[i] = c;
    }

    result = env->NewString(str1, len);
    if (str1 != buf)
        free(str1);
    return result;
}

static const char*
getStringCp1252Chars(JNIEnv *env, jstring jstr)
{
    int i;
    char *result;
    jint len = env->GetStringLength(jstr);
    const jchar *str = env->GetStringCritical(jstr, 0);
    if (str == 0) {
        return 0;
    }

    result = MALLOC_MIN4(len);
    if (result == 0) {
        env->ReleaseStringCritical(jstr, str);
        JNU_ThrowOutOfMemoryError(env, 0);
        return 0;
    }

    for (i=0; i<len; i++) {
        jchar c = str[i];
        if (c < 256)
            result[i] = c;
        else switch(c) {
            case 0x20AC: result[i] = (char)0x80; break;
            case 0x201A: result[i] = (char)0x82; break;
            case 0x0192: result[i] = (char)0x83; break;
            case 0x201E: result[i] = (char)0x84; break;
            case 0x2026: result[i] = (char)0x85; break;
            case 0x2020: result[i] = (char)0x86; break;
            case 0x2021: result[i] = (char)0x87; break;
            case 0x02C6: result[i] = (char)0x88; break;
            case 0x2030: result[i] = (char)0x89; break;
            case 0x0160: result[i] = (char)0x8A; break;
            case 0x2039: result[i] = (char)0x8B; break;
            case 0x0152: result[i] = (char)0x8C; break;
            case 0x017D: result[i] = (char)0x8E; break;
            case 0x2018: result[i] = (char)0x91; break;
            case 0x2019: result[i] = (char)0x92; break;
            case 0x201C: result[i] = (char)0x93; break;
            case 0x201D: result[i] = (char)0x94; break;
            case 0x2022: result[i] = (char)0x95; break;
            case 0x2013: result[i] = (char)0x96; break;
            case 0x2014: result[i] = (char)0x97; break;
            case 0x02DC: result[i] = (char)0x98; break;
            case 0x2122: result[i] = (char)0x99; break;
            case 0x0161: result[i] = (char)0x9A; break;
            case 0x203A: result[i] = (char)0x9B; break;
            case 0x0153: result[i] = (char)0x9C; break;
            case 0x017E: result[i] = (char)0x9E; break;
            case 0x0178: result[i] = (char)0x9F; break;
            default:     result[i] = '?';  break;
        }
    }

    result[len] = 0;
    env->ReleaseStringCritical(jstr, str);
    return result;
}

static int fastEncoding = NO_ENCODING_YET;
static jstring jnuEncoding = NULL;

/* Cached method IDs */
static jmethodID String_init_ID;        /* String(byte[], enc) */
static jmethodID String_getBytes_ID;    /* String.getBytes(enc) */

int getFastEncoding() {
    return fastEncoding;
}

/* Initialize the fast encoding.  If the "sun.jnu.encoding" property
 * has not yet been set, we leave fastEncoding == NO_ENCODING_YET.
 */
void
initializeEncoding(JNIEnv *env)
{
    jstring propname = 0;
    jstring enc = 0;

    if (env->EnsureLocalCapacity(3) < 0)
        return;

    propname = env->NewStringUTF("sun.jnu.encoding");
    if (propname) {
        jboolean exc;
        enc = (jstring)JNU_CallStaticMethodByName
                       (env,
                        &exc,
                        "java/lang/System",
                        "getProperty",
                        "(Ljava/lang/String;)Ljava/lang/String;",
                        propname).l;
        if (!exc) {
            if (enc) {
                const char* encname = env->GetStringUTFChars(enc, 0);
                if (encname) {
           /*
            * On Solaris with nl_langinfo() called in GetJavaProperties():
            *
            *   locale undefined -> NULL -> hardcoded default
            *   "C" locale       -> "" -> hardcoded default     (on 2.6)
            *   "C" locale       -> "ISO646-US"                 (on Sol 7/8)
            *   "en_US" locale -> "ISO8859-1"
            *   "en_GB" locale -> "ISO8859-1"                   (on Sol 7/8)
            *   "en_UK" locale -> "ISO8859-1"                   (on 2.6)
            */
                    if ((strcmp(encname, "8859_1") == 0) ||
                        (strcmp(encname, "ISO8859-1") == 0) ||
                        (strcmp(encname, "ISO8859_1") == 0))
                        fastEncoding = FAST_8859_1;
                    else if (strcmp(encname, "ISO646-US") == 0)
                        fastEncoding = FAST_646_US;
                    else if (strcmp(encname, "Cp1252") == 0 ||
                             /* This is a temporary fix until we move */
                             /* to wide character versions of all Windows */
                             /* calls. */
                             strcmp(encname, "utf-16le") == 0)
                        fastEncoding = FAST_CP1252;
                    else {
                        fastEncoding = NO_FAST_ENCODING;
                        jnuEncoding = (jstring)env->NewGlobalRef(enc);
                    }
                    env->ReleaseStringUTFChars(enc, encname);
                }
            }
        } else {
            env->ExceptionClear();
        }
    } else {
        env->ExceptionClear();
    }
    env->DeleteLocalRef(propname);
    env->DeleteLocalRef(enc);

    /* Initialize method-id cache */
    String_getBytes_ID = env->GetMethodID(JNU_ClassString(env),
                                             "getBytes", "(Ljava/lang/String;)[B");
    String_init_ID = env->GetMethodID(JNU_ClassString(env),
                                         "<init>", "([BLjava/lang/String;)V");
}

static jboolean isJNUEncodingSupported = JNI_FALSE;
static jboolean jnuEncodingSupported(JNIEnv *env) {
    jboolean exe;
    if (isJNUEncodingSupported == JNI_TRUE) {
        return JNI_TRUE;
    }
    isJNUEncodingSupported = (jboolean) JNU_CallStaticMethodByName (
                                    env, &exe,
                                    "java/nio/charset/Charset",
                                    "isSupported",
                                    "(Ljava/lang/String;)Z",
                                    jnuEncoding).z;
    return isJNUEncodingSupported;
}


jstring
NewStringPlatform(JNIEnv *env, const char *str)
{
    return JNU_NewStringPlatform(env, str);
}

jstring nativeNewStringPlatform(JNIEnv *env, const char *str)
{
	env->NewStringUTF(str);
}

jstring JNICALL
JNU_NewStringPlatform(JNIEnv *env, const char *str)
{
    jstring result;
    result = nativeNewStringPlatform(env, str);
    if (result == NULL) {
        jbyteArray hab = 0;
        int len;

        if (fastEncoding == NO_ENCODING_YET)
            initializeEncoding(env);

        if ((fastEncoding == FAST_8859_1) || (fastEncoding == NO_ENCODING_YET))
            return newString8859_1(env, str);
        if (fastEncoding == FAST_646_US)
            return newString646_US(env, str);
        if (fastEncoding == FAST_CP1252)
            return newStringCp1252(env, str);

        if (env->EnsureLocalCapacity(2) < 0)
            return NULL;

        len = (int)strlen(str);
        hab = env->NewByteArray(len);
        if (hab != 0) {
            env->SetByteArrayRegion(hab, 0, len, (jbyte *)str);
            if (jnuEncodingSupported(env)) {
                result = (jstring)env->NewObject(JNU_ClassString(env),
                                           String_init_ID, hab, jnuEncoding);
            } else {
                /*If the encoding specified in sun.jnu.encoding is not endorsed
                  by "Charset.isSupported" we have to fall back to use String(byte[])
                  explicitly here without specifying the encoding name, in which the
                  StringCoding class will pickup the iso-8859-1 as the fallback
                  converter for us.
                 */
                jmethodID mid = (jmethodID)env->GetMethodID(JNU_ClassString(env),
                                                    "<init>", "([B)V");
                result = (jstring)env->NewObject(JNU_ClassString(env), mid, hab);
            }
            env->DeleteLocalRef(hab);
            return result;
        }
    }
    return NULL;
}

const char *
GetStringPlatformChars(JNIEnv *env, jstring jstr, jboolean *isCopy)
{
    return JNU_GetStringPlatformChars(env, jstr, isCopy);
}

char* nativeGetStringPlatformChars(JNIEnv *env, jstring jstr, jboolean *isCopy)
{
	return dvmCreateCstrFromString((StringObject*)jstr);
}

const char * JNICALL
JNU_GetStringPlatformChars(JNIEnv *env, jstring jstr, jboolean *isCopy)
{
    char *result = nativeGetStringPlatformChars(env, jstr, isCopy);
    if (result == NULL) {

        jbyteArray hab = 0;

        if (isCopy)
            *isCopy = JNI_TRUE;

        if (fastEncoding == NO_ENCODING_YET)
            initializeEncoding(env);

        if ((fastEncoding == FAST_8859_1) || (fastEncoding == NO_ENCODING_YET))
            return getString8859_1Chars(env, jstr);
        if (fastEncoding == FAST_646_US)
            return getString646_USChars(env, jstr);
        if (fastEncoding == FAST_CP1252)
            return getStringCp1252Chars(env, jstr);

        if (env->EnsureLocalCapacity(2) < 0)
            return 0;

        if (jnuEncodingSupported(env)) {
            hab = (jbyteArray)env->CallObjectMethod(jstr, String_getBytes_ID, jnuEncoding);
        } else {
            jmethodID mid = env->GetMethodID(JNU_ClassString(env),
                                                "getBytes", "()[B");
            hab = (jbyteArray)env->CallObjectMethod(jstr, mid);
        }

        if (!env->ExceptionCheck()) {
            jint len = env->GetArrayLength(hab);
            result = MALLOC_MIN4(len);
            if (result == 0) {
                JNU_ThrowOutOfMemoryError(env, 0);
                env->DeleteLocalRef(hab);
                return 0;
            }
            env->GetByteArrayRegion(hab, 0, len, (jbyte *)result);
            result[len] = 0; /* NULL-terminate */
        }

        env->DeleteLocalRef(hab);
    }
    return result;
}

void JNICALL
JNU_ReleaseStringPlatformChars(JNIEnv *env, jstring jstr, const char *str)
{
    free((void *)str);
}

/*
 * Export the platform dependent path canonicalization so that
 * VM can find it when loading system classes.
 *
 */
// extern int canonicalize(char *path, const char *out, int len);

// int
// Canonicalize(JNIEnv *env, char *orig, char *out, int len)
// {
//   /* canonicalize an already natived path */
//    return canonicalize(orig, out, len);
//}

jclass JNICALL
JNU_ClassString(JNIEnv *env)
{
    static jclass cls = 0;
    if (cls == 0) {
        jclass c;
        if (env->EnsureLocalCapacity(1) < 0)
            return 0;
        c = env->FindClass("java/lang/String");
        cls = (jclass)env->NewGlobalRef((jobject)c);
        env->DeleteLocalRef(c);
    }
    return cls;
}

jclass JNICALL
JNU_ClassClass(JNIEnv *env)
{
    static jclass cls = 0;
    if (cls == 0) {
        jclass c;
        if (env->EnsureLocalCapacity(1) < 0)
            return 0;
        c = env->FindClass("java/lang/Class");
        cls = (jclass)env->NewGlobalRef((jobject)c);
        env->DeleteLocalRef(c);
    }
    return cls;
}

jclass JNICALL
JNU_ClassObject(JNIEnv *env)
{
    static jclass cls = 0;
    if (cls == 0) {
        jclass c;
        if (env->EnsureLocalCapacity(1) < 0)
            return 0;
        c = env->FindClass("java/lang/Object");
        cls = (jclass)env->NewGlobalRef((jobject)c);
        env->DeleteLocalRef(c);
    }
    return cls;
}

jclass JNICALL
JNU_ClassThrowable(JNIEnv *env)
{
    static jclass cls = 0;
    if (cls == 0) {
        jclass c;
        if (env->EnsureLocalCapacity(1) < 0)
            return 0;
        c = env->FindClass("java/lang/Throwable");
        cls = (jclass)env->NewGlobalRef((jobject)c);
        env->DeleteLocalRef(c);
    }
    return cls;
}

jint JNICALL
JNU_CopyObjectArray(JNIEnv *env, jobjectArray dst, jobjectArray src,
                         jint count)
{
    int i;
    if (env->EnsureLocalCapacity(1) < 0)
        return -1;
    for (i=0; i<count; i++) {
        jstring p = (jstring)env->GetObjectArrayElement(src, i);
        env->SetObjectArrayElement(dst, i, p);
        env->DeleteLocalRef(p);
    }
    return 0;
}

void * JNICALL
JNU_GetEnv(JavaVM *vm, jint version)
{
    void *env;
    vm->GetEnv(&env, version);
    return env;
}

jint JNICALL
JNU_IsInstanceOfByName(JNIEnv *env, jobject object, char* classname)
{
    jclass cls;
    if (env->EnsureLocalCapacity(1) < 0)
        return JNI_ERR;
    cls = env->FindClass(classname);
    if (cls != NULL) {
        jint result = env->IsInstanceOf(object, cls);
        env->DeleteLocalRef(cls);
        return result;
    }
    return JNI_ERR;
}

jboolean JNICALL
JNU_Equals(JNIEnv *env, jobject object1, jobject object2)
{
    static jmethodID mid = NULL;
    if (mid == NULL) {
        mid = env->GetMethodID(JNU_ClassObject(env), "equals",
                                  "(Ljava/lang/Object;)Z");
    }
    return env->CallBooleanMethod(object1, mid, object2);
}


/************************************************************************
 * Thread calls
 */

static jmethodID Object_waitMID;
static jmethodID Object_notifyMID;
static jmethodID Object_notifyAllMID;

void JNICALL
JNU_MonitorWait(JNIEnv *env, jobject object, jlong timeout)
{
    if (object == NULL) {
        JNU_ThrowNullPointerException(env, "JNU_MonitorWait argument");
        return;
    }
    if (Object_waitMID == NULL) {
        jclass cls = JNU_ClassObject(env);
        if (cls == NULL) {
            return;
        }
        Object_waitMID = env->GetMethodID(cls, "wait", "(J)V");
        if (Object_waitMID == NULL) {
            return;
        }
    }
    env->CallVoidMethod(object, Object_waitMID, timeout);
}

void JNICALL
JNU_Notify(JNIEnv *env, jobject object)
{
    if (object == NULL) {
        JNU_ThrowNullPointerException(env, "JNU_Notify argument");
        return;
    }
    if (Object_notifyMID == NULL) {
        jclass cls = JNU_ClassObject(env);
        if (cls == NULL) {
            return;
        }
        Object_notifyMID = env->GetMethodID(cls, "notify", "()V");
        if (Object_notifyMID == NULL) {
            return;
        }
    }
    env->CallVoidMethod(object, Object_notifyMID);
}

void JNICALL
JNU_NotifyAll(JNIEnv *env, jobject object)
{
    if (object == NULL) {
        JNU_ThrowNullPointerException(env, "JNU_NotifyAll argument");
        return;
    }
    if (Object_notifyAllMID == NULL) {
        jclass cls = JNU_ClassObject(env);
        if (cls == NULL) {
            return;
        }
        Object_notifyAllMID = env->GetMethodID(cls,"notifyAll", "()V");
        if (Object_notifyAllMID == NULL) {
            return;
        }
    }
    env->CallVoidMethod(object, Object_notifyAllMID);
}


/************************************************************************
 * Debugging utilities
 */

void JNICALL
JNU_PrintString(JNIEnv *env, char *hdr, jstring string)
{
    if (string == NULL) {
        fprintf(stderr, "%s: is NULL\n", hdr);
    } else {
        const char *stringPtr = JNU_GetStringPlatformChars(env, string, 0);
        if (stringPtr == 0)
            return;
        fprintf(stderr, "%s: %s\n", hdr, stringPtr);
        JNU_ReleaseStringPlatformChars(env, string, stringPtr);
    }
}

void JNICALL
JNU_PrintClass(JNIEnv *env, char* hdr, jobject object)
{
    if (object == NULL) {
        fprintf(stderr, "%s: object is NULL\n", hdr);
        return;
    } else {
        jclass cls = env->GetObjectClass(object);
        jstring clsName = JNU_ToString(env, cls);
        JNU_PrintString(env, hdr, clsName);
        env->DeleteLocalRef(cls);
        env->DeleteLocalRef(clsName);
    }
}

jstring JNICALL
JNU_ToString(JNIEnv *env, jobject object)
{
    if (object == NULL) {
        return env->NewStringUTF("NULL");
    } else {
        return (jstring)JNU_CallMethodByName(env,
                                             NULL,
                                             object,
                                             "toString",
                                             "()Ljava/lang/String;").l;
    }
}

jvalue JNICALL
JNU_GetFieldByName(JNIEnv *env,
                   jboolean *hasException,
                   jobject obj,
                   const char *name,
                   const char *signature)
{
    jclass cls;
    jfieldID fid;
    jvalue result;

    result.i = 0;

    if (env->EnsureLocalCapacity(3) < 0)
        goto done2;

    cls = env->GetObjectClass(obj);
    fid = env->GetFieldID(cls, name, signature);
    if (fid == 0)
        goto done1;

    switch (*signature) {
    case '[':
    case 'L':
        result.l = env->GetObjectField(obj, fid);
        break;
    case 'Z':
        result.z = env->GetBooleanField(obj, fid);
        break;
    case 'B':
        result.b = env->GetByteField(obj, fid);
        break;
    case 'C':
        result.c = env->GetCharField(obj, fid);
        break;
    case 'S':
        result.s = env->GetShortField(obj, fid);
        break;
    case 'I':
        result.i = env->GetIntField(obj, fid);
        break;
    case 'J':
        result.j = env->GetLongField(obj, fid);
        break;
    case 'F':
        result.f = env->GetFloatField(obj, fid);
        break;
    case 'D':
        result.d = env->GetDoubleField(obj, fid);
        break;

    default:
        env->FatalError("JNU_GetFieldByName: illegal signature");
    }

 done1:
    env->DeleteLocalRef(cls);
 done2:
    if (hasException) {
        *hasException = env->ExceptionCheck();
    }
    return result;
}

void JNICALL
JNU_SetFieldByName(JNIEnv *env,
                   jboolean *hasException,
                   jobject obj,
                   const char *name,
                   const char *signature,
                   ...)
{
    jclass cls;
    jfieldID fid;
    va_list args;

    if (env->EnsureLocalCapacity(3) < 0)
        goto done2;

    cls = env->GetObjectClass(obj);
    fid = env->GetFieldID(cls, name, signature);
    if (fid == 0)
        goto done1;

    va_start(args, signature);
    switch (*signature) {
    case '[':
    case 'L':
        env->SetObjectField(obj, fid, va_arg(args, jobject));
        break;
    case 'Z':
        env->SetBooleanField(obj, fid, (jboolean)va_arg(args, int));
        break;
    case 'B':
        env->SetByteField(obj, fid, (jbyte)va_arg(args, int));
        break;
    case 'C':
        env->SetCharField(obj, fid, (jchar)va_arg(args, int));
        break;
    case 'S':
        env->SetShortField(obj, fid, (jshort)va_arg(args, int));
        break;
    case 'I':
        env->SetIntField(obj, fid, va_arg(args, jint));
        break;
    case 'J':
        env->SetLongField(obj, fid, va_arg(args, jlong));
        break;
    case 'F':
        env->SetFloatField(obj, fid, (jfloat)va_arg(args, jdouble));
        break;
    case 'D':
        env->SetDoubleField(obj, fid, va_arg(args, jdouble));
        break;

    default:
        env->FatalError("JNU_SetFieldByName: illegal signature");
    }
    va_end(args);

 done1:
    env->DeleteLocalRef(cls);
 done2:
    if (hasException) {
        *hasException = env->ExceptionCheck();
    }
}

jvalue JNICALL
JNU_GetStaticFieldByName(JNIEnv *env,
                         jboolean *hasException,
                         const char *classname,
                         const char *name,
                         const char *signature)
{
    jclass cls;
    jfieldID fid;
    jvalue result;

    result.i = 0;

    if (env->EnsureLocalCapacity(3) < 0)
        goto done2;

    cls = env->FindClass(classname);
    if (cls == 0)
        goto done2;

    fid = env->GetStaticFieldID(cls, name, signature);
    if (fid == 0)
        goto done1;

    switch (*signature) {
    case '[':
    case 'L':
        result.l = env->GetStaticObjectField(cls, fid);
        break;
    case 'Z':
        result.z = env->GetStaticBooleanField(cls, fid);
        break;
    case 'B':
        result.b = env->GetStaticByteField(cls, fid);
        break;
    case 'C':
        result.c = env->GetStaticCharField(cls, fid);
        break;
    case 'S':
        result.s = env->GetStaticShortField(cls, fid);
        break;
    case 'I':
        result.i = env->GetStaticIntField(cls, fid);
        break;
    case 'J':
        result.j = env->GetStaticLongField(cls, fid);
        break;
    case 'F':
        result.f = env->GetStaticFloatField(cls, fid);
        break;
    case 'D':
        result.d = env->GetStaticDoubleField(cls, fid);
        break;

    default:
        env->FatalError("JNU_GetStaticFieldByName: illegal signature");
    }

 done1:
    env->DeleteLocalRef(cls);
 done2:
    if (hasException) {
        *hasException = env->ExceptionCheck();
    }
    return result;
}

void JNICALL
JNU_SetStaticFieldByName(JNIEnv *env,
                         jboolean *hasException,
                         const char *classname,
                         const char *name,
                         const char *signature,
                         ...)
{
    jclass cls;
    jfieldID fid;
    va_list args;

    if (env->EnsureLocalCapacity(3) < 0)
        goto done2;

    cls = env->FindClass(classname);
    if (cls == 0)
        goto done2;

    fid = env->GetStaticFieldID(cls, name, signature);
    if (fid == 0)
        goto done1;

    va_start(args, signature);
    switch (*signature) {
    case '[':
    case 'L':
        env->SetStaticObjectField(cls, fid, va_arg(args, jobject));
        break;
    case 'Z':
        env->SetStaticBooleanField(cls, fid, (jboolean)va_arg(args, int));
        break;
    case 'B':
        env->SetStaticByteField(cls, fid, (jbyte)va_arg(args, int));
        break;
    case 'C':
        env->SetStaticCharField(cls, fid, (jchar)va_arg(args, int));
        break;
    case 'S':
        env->SetStaticShortField(cls, fid, (jshort)va_arg(args, int));
        break;
    case 'I':
        env->SetStaticIntField(cls, fid, va_arg(args, jint));
        break;
    case 'J':
        env->SetStaticLongField(cls, fid, va_arg(args, jlong));
        break;
    case 'F':
        env->SetStaticFloatField(cls, fid, (jfloat)va_arg(args, jdouble));
        break;
    case 'D':
        env->SetStaticDoubleField(cls, fid, va_arg(args, jdouble));
        break;

    default:
        env->FatalError("JNU_SetStaticFieldByName: illegal signature");
    }
    va_end(args);

 done1:
    env->DeleteLocalRef(cls);
 done2:
    if (hasException) {
        *hasException = env->ExceptionCheck();
    }
}

// end of file
