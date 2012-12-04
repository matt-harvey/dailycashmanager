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
 * @todo Testing.
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
	 *
	 * @todo Testing.
	 */
	Handle(T* p_pointer);

	/**
	 * Preconditions:\n
	 * the object must have been managed
	 * throughout its life by (a single instance of) IdentityMap,
	 * and must only have ever been
	 * accessed via instances of Handle<Derived>; and\n
	 * The destructor of Derived must be non-throwing.
	 *
	 * Exception safety: <em>nothrow guarantee</em>, provided the
	 * preconditions are met.
	 *
	 * @todo Testing.
	 */
	~Handle();

	/**
	 * @throws sqloxx::OverflowException in the extremely unlikely
	 * event that the number of Handle instances pointing to the
	 * underlying instance of T is too large to be safely counted
	 * by the type PersistentObject<T, Connection>::HandleCounter.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 *
	 * @todo Testing.
	 */
	Handle(Handle const& rhs);
	
	/**
	 * @throws sqloxx::OverflowException in the extremely unlikely
	 * event that the number of Handle instances pointing to the
	 * underlying instance of T is too large to be safely counted
	 * by the type PersistentObject<T, Connection>::HandleCounter.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 *
	 * @todo Testing.
	 */
	Handle& operator=(Handle const& rhs);

	/**
	 * @returns \e true if this Handle<T> is bound to some instance
	 * of T; otherwise returns \e false.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 *
	 * @todo Testing.
	 */
	operator bool() const;

	/**
	 * @returns the instance of T that is handled by this Handle<T>.
	 *
	 * @throws UnboundHandleException if there is no instance of
	 * T bound to this Handle.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 *
	 * @todo Testing.
	 */
	T& operator*() const;

	/**
	 * Indirection operator analagous to operator*(), for
	 * accessing members of T via the underlying pointer.
	 *
	 * @throws UnboundHandleException if there is no instance
	 * of T bound to this Handle.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 *
	 * @todo Testing.
	 */
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
	// Strong guarantee, provided rhs has a valid pointer...
	rhs.m_pointer->notify_rhs_assignment_operation();

	// Nothrow guarantee, provided preconditions met, and
	// provided rhs has a valid pointer.
	m_pointer->notify_lhs_assignment_operation();

	m_pointer = rhs.m_pointer;  // nothrow

	return *this;  // throw, provided we have a valid pointer
}

template <typename T>
Handle<T>::operator bool() const
{
	return static_cast<bool>(m_pointer);  // nothrow
}

template <typename T>
T&
Handle<T>::operator*() const
{
	if (static_cast<bool>(m_pointer))  // nothrow
	{
		return *m_pointer;  // nothrow
	}
	throw (UnboundHandleException("Unbound Handle."));
}

template <typename T>
T*
Handle<T>::operator->() const
{
	if (static_cast<bool>(m_pointer))  // nothrow
	{
		return m_pointer;  // nothrow
	}
	throw (UnboundHandleException("Unbound Handle."));
}



		

}  // namespace sqloxx

#endif  // GUARD_handle_hpp
