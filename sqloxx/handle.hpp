#ifndef GUARD_handle_hpp
#define GUARD_handle_hpp

#include "sqloxx_exceptions.hpp"

namespace sqloxx
{


// Class template

/**
 * Handle for handling business objects of type T where T is generally
 * managed via IdentityMap<T> to ensure only one instance of T exists in
 * memory at any one time, in relation to any given record in the database.
 */
template <typename T>
class Handle
{
public:
	Handle(T* p_pointer);
	~Handle();
	Handle(Handle const& rhs);
	Handle& operator=(Handle const& rhs);
	operator bool() const;
	T& operator*() const;
	T* operator->() const;
private:
	T* m_pointer;

};


template <typename T>
Handle<T>::Handle(T* p_pointer):
	m_pointer(p_pointer)
{
	p_pointer->notify_handle_construction();
}

template <typename T>
Handle<T>::~Handle()
{
	m_pointer->notify_handle_destruction();
}

template <typename T>
Handle<T>::Handle(Handle const& rhs)
{
	m_pointer = rhs.m_pointer;
	m_pointer->notify_handle_copy_construction();
}

template <typename T>
Handle<T>&
Handle<T>::operator=(Handle const& rhs)
{
	rhs.m_pointer->notify_rhs_assignment_operation();
	m_pointer->notify_lhs_assignment_operation();
	m_pointer = rhs.m_pointer;
	return *this;
}

template <typename T>
Handle<T>::operator bool() const
{
	return static_cast<bool>(m_pointer);
}

template <typename T>
T&
Handle<T>::operator*() const
{
	if (static_cast<bool>(m_pointer))
	{
		return *m_pointer;
	}
	throw (UnboundHandleException("Unbound Handle."));
}

template <typename T>
T*
Handle<T>::operator->() const
{
	if (static_cast<bool>(m_pointer))
	{
		return m_pointer;
	}
	throw (UnboundHandleException("Unbound Handle."));
}



		

}  // namespace sqloxx

#endif  // GUARD_handle_hpp
