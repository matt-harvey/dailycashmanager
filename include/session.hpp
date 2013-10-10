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


#ifndef GUARD_session_hpp_36949449096317144
#define GUARD_session_hpp_36949449096317144

#include <string>

namespace phatbooks
{


/**
 * Phatbooks user session base class.
 * May be inherited by a class representing a more specific kind
 * of session, e.g. a console session or a GUI session.
 */
class Session
{
public:

	/**
	 * Only 1 Session may be created, and this is non-copyable.
	 * So this is the "Singleton pattern".
	 * However, the single instance is \e not globally accesssible via
	 * a static function: if we require references to it in multiple
	 * places, we must explicitly pass these around.
	 *
	 * @throws TooManySessions if we attempt to create a second
	 * Session.
	 */
	Session();

	Session(Session const&) = delete;
	Session(Session&&) = delete;
	Session& operator=(Session const&) = delete;
	Session& operator=(Session&&) = delete;

	virtual ~Session();

	/**
	 * Run a session in which the user chooses a file to open from
	 * within the session.
	 */
	int run();

	/**
	 * Run a session in which the file to open is given by filepath_str.
	 */
	int run(std::string const& filepath_str);

private:
	virtual int do_run() = 0;
	virtual int do_run(std::string const& filepath_str) = 0;
	static int const s_max_instances = 1;
	static int const s_default_caching_level = 10;
	static int s_num_instances;
};

}  // namespace phatbooks


#endif  // GUARD_session_hpp_36949449096317144
