#ifndef GUARD_handle_hpp
#define GUARD_handle_hpp

#include "general_typedefs.hpp"
#include "identity_map.hpp"
#include "sqloxx_exceptions.hpp"

namespace sqloxx
{

// Forward declaration
template <typename T>
class PersistentObjectHandleAttorney;


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
	 */
	~Handle();

	/**
	 * @todo Documentation and testing.
	 */
	template <typename Connection>
	Handle(Connection& p_connection);

	/**
	 * @todo Documentation and testing.
	 */
	template <typename Connection>
	Handle(Connection& p_connection, Id p_id);

	/**
	 * @todo Documentation and testing.
	 */
	template <typename Connection>
	Handle(Connection& p_connection, Id p_id, char p_no_check_flag);

	/**
	 * @throws sqloxx::OverflowException in the extremely unlikely
	 * event that the number of Handle instances pointing to the
	 * underlying instance of T is too large to be safely counted
	 * by the type PersistentObject<T, Connection>::HandleCounter.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	Handle(Handle const& rhs);
	
	/**
	 * @throws sqloxx::OverflowException in the extremely unlikely
	 * event that the number of Handle instances pointing to the
	 * underlying instance of T is too large to be safely counted
	 * by the type PersistentObject<T, Connection>::HandleCounter.
	 *
	 * Exception safety: <em>strong guarantee</em>.
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
	 */
	T* operator->() const;

private:


	T* m_pointer;
};


template <typename T>
Handle<T>::~Handle()
{
	PersistentObjectHandleAttorney<T>::notify_handle_destruction(*m_pointer);
}

template <typename T>
Handle<T>::Handle(Handle const& rhs)
{
	m_pointer = rhs.m_pointer;
	PersistentObjectHandleAttorney<T>::notify_handle_copy_construction
	(	*m_pointer
	);
}


template <typename T>
template <typename Connection>
Handle<T>::Handle(Connection& p_connection):
	m_pointer
	(	IdentityMap<T, Connection>::HandleAttorney::get_pointer
		(	p_connection.template identity_map<T>()
		)
	)
{
	PersistentObjectHandleAttorney<T>::notify_handle_construction(*m_pointer);
}


template <typename T>
template <typename Connection>
Handle<T>::Handle(Connection& p_connection, Id p_id):
	m_pointer
	(	IdentityMap<T, Connection>::HandleAttorney::get_pointer
		(	p_connection.template identity_map<T>(),
			p_id
		)
	)
{
	PersistentObjectHandleAttorney<T>::notify_handle_construction(*m_pointer);
}


template <typename T>
template <typename Connection>
Handle<T>::Handle(Connection& p_connection, Id p_id, char p_no_check_flag):
	m_pointer
	(	IdentityMap<T, Connection>::HandleAttorney::unchecked_get_pointer
		(	p_connection.template identity_map<T>(),
			p_id
		)
	)
{
	p_no_check_flag;  // Silence compiler re. unused variable
	PersistentObjectHandleAttorney<T>::notify_handle_construction(*m_pointer);
}

template <typename T>
Handle<T>&
Handle<T>::operator=(Handle const& rhs)
{
	// Strong guarantee, provided rhs has a valid pointer...
	PersistentObjectHandleAttorney<T>::notify_rhs_assignment_operation
	(	*(rhs.m_pointer)
	);

	// Nothrow guarantee, provided preconditions met, and
	// provided rhs has a valid pointer.
	PersistentObjectHandleAttorney<T>::notify_lhs_assignment_operation
	(	*m_pointer
	);

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
