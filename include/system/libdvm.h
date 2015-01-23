/*
 * Copyright (C) 2009 Tomken Corp.
 *
 * Author: Tomken
 * Date:   2011-04-02
 * 
 */


#ifndef __LIB_DVM_H__
#define __LIB_DVM_H__

#include "Object_4.h"

struct Thread;
struct Method;
struct HashTable;

JNIEnv* dvmGetThreadJNIEnv(Thread* self);

bool dvmCheckException(Thread* self);
void dvmClearException(Thread* self);
void dvmSetException(Thread* self, Object* exception);
Object* dvmGetException(Thread* self);

Method* dvmFindVirtualMethod(const ClassObject* clazz, const char* methodName, const DexProto* proto);
const Method* dvmGetVirtualizedMethod(const ClassObject* clazz, const Method* meth);
	
void dvmCallMethod(Thread* self, const Method* method, Object* obj,
    JValue* pResult, ...);
void dvmCallMethodV(Thread* self, const Method* method, Object* obj, bool fromJni, JValue* pResult,    va_list args);

StringObject* dvmCreateStringFromCstr(const char* utf8Str);
char* dvmCreateCstrFromString(const StringObject* jstr);
u4 dvmComputeUtf8Hash(const char* utf8Str);

//

ClassObject* dvmFindLoadedClass(const char* descriptor);

void dvmUseJNIBridge(Method* method, void* func);
void dvmSetNativeFunc(Method* method, DalvikBridgeFunc func, const u2* insns);

/** hash table */
typedef void (*HashFreeFunc)(void* ptr);
typedef int (*HashForeachFunc)(void* data, void* arg);
typedef int (*HashForeachRemoveFunc)(void* data);
typedef int (*HashCompareFunc)(const void* tableItem, const void* looseItem);

HashTable* dvmHashTableCreate(size_t initialSize, HashFreeFunc freeFunc);
size_t dvmHashSize(size_t size);
void dvmHashTableClear(HashTable* pHashTable);
void dvmHashTableFree(HashTable* pHashTable);
void dvmHashTableLock(HashTable* pHashTable);
void dvmHashTableUnlock(HashTable* pHashTable);
int dvmHashTableNumEntries(HashTable* pHashTable);
int dvmHashTableMemUsage(HashTable* pHashTable);
void* dvmHashTableLookup(HashTable* pHashTable, u4 itemHash, void* item, HashCompareFunc cmpFunc, bool doAdd);
bool dvmHashTableRemove(HashTable* pHashTable, u4 hash, void* item);
int dvmHashForeach(HashTable* pHashTable, HashForeachFunc func, void* arg);
int dvmHashForeachRemove(HashTable* pHashTable, HashForeachRemoveFunc func);

/*
static int hashcmpDexOrJar(const void* tableVal, const void* newVal)
{
    return (int) newVal - (int) tableVal;
}

static int hashcmpClassByClass(const void* vclazz, const void* vaddclazz)
{
    const ClassObject* clazz = (const ClassObject*) vclazz;
    const ClassObject* addClazz = (const ClassObject*) vaddclazz;
    bool match;

    match = (strcmp(clazz->descriptor, addClazz->descriptor) == 0 &&
             (clazz->classLoader == addClazz->classLoader ||
              (addClazz->classLoader != NULL &&
               dvmLoaderInInitiatingList(clazz, addClazz->classLoader)) ));
    return !match;
}

*/

#endif // __LIB_DVM_H__
