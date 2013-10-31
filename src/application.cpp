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


#include "application.hpp"
#include "string_conv.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <wx/config.h>
#include <wx/string.h>
#include <cstdlib>
#include <string>

using boost::optional;
using jewel::value;
using std::getenv;
using std::string;
namespace filesystem = boost::filesystem;

namespace phatbooks
{


namespace
{
	wxString config_location_for_last_opened_file()
	{
		return wxString("/General/LastOpenedFile");
	}

}  // end anonymous namespace


}  // namespace phatbooks
