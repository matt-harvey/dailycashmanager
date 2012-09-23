#ifndef GUARD_import_from_nap_hpp
#define GUARD_import_from_nap_hpp

/** \file import_from_nap.hpp
 *
 * \brief Header containing code to enable import of data
 * into a Phatbooks database, from csv files exported from
 * the N. A. P. application.
 *
 * The code here is solely for the purpose of migrating the developer's
 * (Matthew Harvey's) personal accounting data from his previous, prototype
 * Python application, N. A. P., to Phatbooks. This should be removed from
 * any released version of Phatbooks.
 */


#include "phatbooks_database_connection.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>


namespace phatbooks
{



/**
 * Import data from a N. A. P. csv set.
 *
 * @todo Implement this method.
 *
 * @todo Adjust existing implementation to use new DraftJournal and
 * OrdinaryJournal derived classes as appropriate.
 *
 * @throws std::logic_error if \c directory is not directory
 *
 * @throws std::runtime_error if \c directory does not contain the
 * following csv files (exported from nap):\n
 * 		"accountshelf.csv";\n
 * 		"draftentryshelf.csv";\n
 * 		"draftjournalshelf.csv";\n
 * 		"entryshelf";\n
 * 		"journalshelf";\n
 *
 * May throw various other exceptions, or crash wildly. This is not
 * a polished function but intended as a quick hack only! See
 * file documentation for import_from_nap.hpp.
 *
 * @todo The implementation of this function is hideously messy, slow,
 * bulky, inefficient and repetitive. It doesn't need to be perfect as
 * it's only a hack for my own use... but really...
 *
 * @todo I don't think the function currently reacts properly to
 * entries where a SINGLE ENTRY has both actual and budget impacts.
 */
void import_from_nap
(	boost::shared_ptr<PhatbooksDatabaseConnection> p_database_connection,
	boost::filesystem::path const& directory
);











}  // namespace phatbooks

#endif  // GUARD_phatbooks_database_connection_hpp
