/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_client_data_hpp_1960609588753716
#define GUARD_client_data_hpp_1960609588753716

#include <wx/clntdata.h>

namespace phatbooks
{
namespace gui
{

/**
 * Facilitates derivation from wxClientData where T is the class about
 * instances of which we want to hold data. T must be copy-constructible.
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
	virtual ~ClientData();
	T data() const;
private:
	T const m_data;

};  // ClientData


template <typename T>
inline
ClientData<T>::ClientData(T const& p_data):
	wxClientData(),
	m_data(p_data)
{
}

template <typename T>
inline
ClientData<T>::~ClientData()
{
}

template <typename T>
inline
T
ClientData<T>::data() const
{
	return m_data;
}

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_client_data_hpp_1960609588753716
