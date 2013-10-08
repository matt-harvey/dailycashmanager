// Copyright (c) 2013, Matthew Harvey. All rights reserved.

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
