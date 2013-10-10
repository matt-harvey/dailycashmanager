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


#include "gui/sizing.hpp"

namespace phatbooks
{
namespace gui
{

int
standard_gap()
{
	return 5;
}

int
standard_border()
{
	return standard_gap();
}

int
medium_width()
{
	return 160;
}

int
large_width()
{
	return medium_width() * 2 + standard_gap();
}

int
extra_large_width()
{
	return medium_width() * 3 + standard_gap() * 2;
}

int scrollbar_width_allowance()
{
	// TODO Use conditional compilation to adapt this to different platforms.
	return 20;
}

}  // namespace gui
}  // namespace phatbooks
