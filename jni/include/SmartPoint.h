#ifndef _SMART_POINT_H_
#define _SMART_POINT_H_

#include <stddef.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
// 

// #define COMPARE(_op_)                                           \
// 	inline bool operator _op_ (const lsp<T>& o) const {              \
// 	return m_ptr _op_ o.m_ptr;                                  \
// }

//////////////////////////////////////////////////////////////////////////
// 轻量级智能指针(Light Smart Point)。
// 错误用法：
// lsp<int> a(1);
// lsp<int> b = a.get();

template <typename T>
class lsp
{
public:
	inline lsp() : m_ptr(0) { }

	lsp(T* other);
	lsp(const lsp<T>& other);
	template<typename U> lsp(U* other);
	template<typename U> lsp(const lsp<U>& other);

	~lsp();

	// Assignment

	//lsp& operator = (T* other);
	lsp& operator = (const lsp<T>& other);

	template<typename U> lsp& operator = (const lsp<U>& other);
	//template<typename U> lsp& operator = (U* other);

	//! Special optimization for use by ProcessState (and nobody else).
	//void force_set(T* other);

	// Reset

	void clear();

	// Accessors

	inline  T&      operator* () const  { return *m_ptr; }
	inline  T*      operator-> () const { return m_ptr;  }
	inline  T*      get() const         { return m_ptr; }
	
	int32_t getCount() const   { return NULL == mpCount ? 0 : *mpCount; }

	// Operators

// 	COMPARE(==)
// 	COMPARE(!=)
// 	COMPARE(>)
// 	COMPARE(<)
// 	COMPARE(<=)
// 	COMPARE(>=)

private:
	template<typename Y> friend class lsp;
// 	template<typename Y> friend class sp;
// 	template<typename Y> friend class wp;
// 	void set_pointer(T* ptr);
	T* m_ptr;
	//mutable volatile int32_t mCount;
	int32_t* mpCount;

	inline void incStrong() const {
		__sync_fetch_and_add(mpCount, 1);
	}
	inline void decStrong() {
		if (__sync_fetch_and_sub(mpCount, 1) == 1) {
			delete static_cast<const T*>(m_ptr);
			m_ptr = NULL;
			delete mpCount;
			mpCount = NULL;
		}
	}
};

// ---------------------------------------------------------------------------
// No user serviceable parts below here.

template<typename T>
lsp<T>::lsp(T* other)
: m_ptr(other), mpCount(new int32_t(1))
{}

template<typename T>
lsp<T>::lsp(const lsp<T>& other)
: m_ptr(other.m_ptr), mpCount(other.mpCount)
{
	if (m_ptr) incStrong();
}

template<typename T> template<typename U>
lsp<T>::lsp(U* other) : m_ptr(other), mpCount(new int32_t(1))
{}

template<typename T> template<typename U>
lsp<T>::lsp(const lsp<U>& other)
: m_ptr(other.m_ptr), mpCount(other.mpCount)
{
	if (m_ptr) incStrong();
}

template<typename T>
lsp<T>::~lsp()
{
	if (m_ptr) decStrong();
}

template<typename T>
lsp<T>& lsp<T>::operator = (const lsp<T>& other) {
	T* otherPtr(other.m_ptr);
	if (otherPtr) other.incStrong();
	if (m_ptr) decStrong();
	mpCount = other.mpCount;
	m_ptr = otherPtr;
	
	return *this;
}

// template<typename T>
// lsp<T>& lsp<T>::operator = (T* other)
// {
// 	if (other) other->incStrong(this);
// 	if (m_ptr) m_ptr->decStrong(this);
// 	m_ptr = other;
// 	return *this;
// }

template<typename T> template<typename U>
lsp<T>& lsp<T>::operator = (const lsp<U>& other) {
	T* otherPtr(other.m_ptr);
	if (otherPtr) other.incStrong();
	if (m_ptr) decStrong();
	mpCount = other.mpCount;
	m_ptr = otherPtr;

	return *this;
}

// template<typename T> template<typename U>
// lsp<T>& lsp<T>::operator = (U* other)
// {
// 	if (other) ((T*)other)->incStrong(this);
// 	if (m_ptr) m_ptr->decStrong(this);
// 	m_ptr = other;
// 	return *this;
// }

// template<typename T>
// void lsp<T>::force_set(T* other)
// {
// 	other->forceIncStrong(this);
// 	m_ptr = other;
// }

template<typename T>
void lsp<T>::clear()
{
	if (m_ptr) {
		decStrong();
		m_ptr = NULL;
		mpCount = NULL;
	}
}

// template<typename T>
// void lsp<T>::set_pointer(T* ptr) {
// 	m_ptr = ptr;
// }


#endif