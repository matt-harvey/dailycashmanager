// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_client_data_hpp_1960609588753716
#define GUARD_client_data_hpp_1960609588753716

#include <wx/clntdata.h>

namespace phatbooks
{
namespace gui
{

/**
 * Facilitates derivation from wxClientData where T is the class about
 * instances of we want to hold data. T must be copy-constructible.
 *
 * (Pointers to instances of wxClientData may be associated with certain
 * controls in wxWidgets to assist in processing data associated with
 * those controls.)
 */
template <typename T>
class ClientData: public wxClientData
{
public:
	ClientData(T const& p_data);
	ClientData(ClientData const&) = default;
	ClientData(ClientData&&) = default;
	ClientData& operator=(ClientData const&) = default;
	ClientData& operator=(ClientData&&) = default;
	virtual ~ClientData() = default;
	T data() const;
private:
	T const m_data;

};  // ClientData


template <typename T>
ClientData<T>::ClientData(T const& p_data):
	wxClientData(),
	m_data(p_data)
{
}

template <typename T>
T
ClientData<T>::data() const
{
	return m_data;
}

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_client_data_hpp_1960609588753716
