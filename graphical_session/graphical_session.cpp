#include "graphical_session.hpp"
#include <iostream>
#include <string>

using std::cerr;
using std::endl;
using std::string;

namespace phatbooks
{
namespace gui
{

GraphicalSession::GraphicalSession()
{
}

GraphicalSession::~GraphicalSession()
{
}

int
GraphicalSession::do_run(string const& filename)
{
	cerr << "The graphical user interface has not yet been implemented."
	     << endl;
	return 1;
}


}  // namespace gui
}  // namespace phatbooks
