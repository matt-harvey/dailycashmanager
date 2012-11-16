#ifndef GUARD_handle_hpp
#define GUARD_handle_hpp

#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <boost/shared_ptr.hpp>

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
	Handle(typename boost::shared_ptr<T> p_pointer);
	~Handle();
	Handle(Handle const& rhs);
	operator bool() const;
	T& operator*() const;
	T* operator->() const;
private:
	boost::shared_ptr<T> m_pointer;
	Handle& operator=(Handle const& rhs);  // Unimplemented

};



template <typename T>
Handle<T>::Handle
(	typename boost::shared_ptr<T> p_pointer
):
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
Handle<T>::operator bool() const
{
	return static_cast<bool>(m_pointer);
}

template <typename T>
T& Handle<T>::operator*() const
{
	if (static_cast<bool>(m_pointer))
	{
		return *m_pointer;
	}
	throw (UnboundHandleException("Unbound Handle."));
}

template <typename T>
T* Handle<T>::operator->() const
{
	if (static_cast<bool>(m_pointer))
	{
		return *m_pointer;
	}
	throw (UnboundHandleException("Unbound Handle."));
}



		

}  // namespace sqloxx

#endif  // GUARD_handle_hpp
