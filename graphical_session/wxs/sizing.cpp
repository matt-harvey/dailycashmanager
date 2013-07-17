#include "sizing.hpp"

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

}  // namespace gui
}  // namespace phatbooks
