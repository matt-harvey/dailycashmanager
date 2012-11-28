#ifndef GUARD_handle_hpp
#define GUARD_handle_hpp

#include "sqloxx_exceptions.hpp"

namespace sqloxx
{

/**
 * Handle for handling business objects of type T where T is a class
 * derived from PersistentObject and is
 * managed via IdentityMap<T> to ensure only one instance of T exists in
 * memory at any one time, in relation to any given record in a given
 * database.
 *
 * @todo Testing and documentation.
 */
template <typename T>
class Handle
{
public:

	/** Construct a Handle<T> from a T*.
	 * 
	 * @throws sqloxx::OverflowException if the maximum number
	 * of handles for this underlying instance of T has been reached.
	 * The circumstances under which this occurs depend on the
	 * implementation of T::notify_handle_construction(), but should
	 * be extremely rare.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
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
