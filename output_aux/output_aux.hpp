#ifndef GUARD_output_aux_hpp
#define GUARD_output_aux_hpp

#include <ostream>
#include <sstream>

namespace phatbooks
{

/**
 * @todo HIGH PRIORITY This could be used by consolixx::Table
 * as well. It should be moved out of phatbooks and into either
 * consolixx or jewel.
 */
template <typename charT, typename traits, typename T>
void output_aux
(	std::basic_ostream<charT, traits>& os,
	T const& x,
	void (*do_output)(std::basic_ostream<charT, traits>&, T const&)
)
{
	if (!os)
	{
		return;
	}
	try
	{
		std::basic_ostringstream<charT, traits> ss;
		ss.exceptions(os.exceptions());
		ss.imbue(os.getloc());
		do_output(os, x);
		if (!ss)
		{
			os.setstate(ss.rdstate());
			return;
		}
		os << ss.str();
	}
	catch (...)
	{
		os.setstate(std::ios_base::badbit);
	}
	return;
}



}  // namespace phatbooks

#endif  // GUARD_output_aux_hpp
