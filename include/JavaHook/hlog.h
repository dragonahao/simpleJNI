/*
 * Copyright (C) 2009 Tencent Corp.
 *
 * Author: Tomken
 * Date:   2011-04-02
 * 
 */

#ifndef __H_LOG_H__
#define __H_LOG_H__


#include <android/log.h>

#define LOG_TAG "NNNN"
#define HLOGI(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)


/*
#define LOG_TAG "NNNN"
#define CONDITION(cond)     (__builtin_expect((cond)!=0, 0))

#ifdef USING_LOG

#ifdef  LOGCAT_LOG

#include <android/log.h>
#define HLOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))

#ifndef HLOGI_IF
#define HLOGI_IF(cond, ...) \
    ( (CONDITION(cond)) \
    ? ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif


#else // LOGCAT_LOG

#define HLOGI(...) printf(__VA_ARGS__)
#ifndef HLOGI_IF
#define HLOGI_IF(cond, ...) \
    ( (CONDITION(cond)) \
    ? ((void)printf(__VA_ARGS__)) \
    : (void)0 )
#endif

#endif // LOGCAT_LOG

#else // USING_LOG

# define HLOGI(...) ((void)0)

#endif //USING_LOG
*/
#endif // __H_LOG_H__
