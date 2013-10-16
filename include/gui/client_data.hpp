/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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