#ifndef GUARD_dialogue_hpp
#define GUARD_dialogue_hpp


namespace phatbooks
{


class PhatbooksDatabaseConnection;
class PhatbooksTextSession;


/**
 * Represents a certain, somewhat self-contained portion of
 * "dialogue" between the user and the application, during a
 * console/text session. Classes derived from dialogue are
 * intended as a means for encapsulating specific interactions
 * with the user, to prevent the class PhatbooksTextSession from
 * becoming a big, disorganized bag of functions.
 */
class Dialogue
{
public:
	Dialogue(PhatbooksTextSession const& p_session);
	void run();
	

private:
	PhatbooksDatabaseConnection& m_database_connection;
	virtual void do_run() = 0;



};  // class Dialogue



}  // namespace phatbooks


#endif  // GUARD_dialogue_hpp
