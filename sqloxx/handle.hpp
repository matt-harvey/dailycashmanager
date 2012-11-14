#ifndef GUARD_handle_hpp
#define GUARD_handle_hpp

#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace sqloxx
{

template <typename T>
class Handle
{
public:
	Handle(boost::shared__ptr<T> p_pointer);
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
(	boost::shared_ptr<T> ptr
):
	m_pointer(p_pointer)
{
}

template <typename T>
Handle<T>::Handle()
{
	m_pointer->decrement_handle_counter();
}

template <typename T>
Handle<T>::Handle(Handle const& rhs)
{
	m_pointer = rhs.m_pointer;
	m_pointer->increment_handle_counter();
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
	throw (std::runtime_error("Unbound Handle."))
}

template <typename T>
T* Handle<T>::operator->() const
{
	if (static_cast<bool>(m_pointer))
	{
		return *m_pointer;
	}
	throw (std::runtime_error("Unbound Handle."));
}



		

}  // namespace sqloxx

#endif  // GUARD_handle_hpp
