#include "dialogue.hpp"
#include "phatbooks_text_session.hpp"

namespace phatbooks
{



Dialogue::Dialogue(PhatbooksTextSession const& p_session):
	m_database_connection(p_session.database_connection())
{
}




}  // namespace phatbooks
