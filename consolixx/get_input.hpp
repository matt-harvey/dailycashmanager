#ifndef GUARD_get_input_hpp
#define GUARD_get_input_hpp

#include <boost/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/decimal.hpp>
#include <string>

namespace consolixx
{


/**
 * Function for safely getting a line of console input from a user.
 *
 * @param error_message Message to display to user if there is an error
 * receiving their input.
 *
 * @param is Input stream from which to accept input.
 *
 * @param os Output stream to which error message is printed in the event of
 * an input error.
 *
 * @returns text entered by the user
 *
 * @todo Determine and document throwing behaviour.
 */
std::string get_user_input
(	std::string const& error_message =
		"\nThere has been an error reading your input. Please try again: ",
	std::istream& is = std::cin,
	std::ostream& os = std::cout
);

/**
 * Function for safely getting a line of console input from a user, where the
 * input is required to satisfy a given criterion. The user is repeatedly
 * prompted until they provide "satisactory" input, or else (if this option is
 * provided) escape the process by entering a string that satisfies
 * an "escape criterion".
 *
 * @param criterion Boolean test that must return true when applied to the
 * user's input for it to be regarded as "satisfactory".
 * 
 * @param message_on_invalid_input Message to display if user's input is
 * successfully read, but it does not satisfy \c criterion.
 *
 * @param user_can_escape Boolean indicating whether the user has the option
 * to escape the operation by entering a string that satisfies
 * \c escape_criterion.
 *
 * @param escape_criterion Boolean test such that, if it returns true, the
 * user will escape this procedure. Note this takes higher priority over
 * \c criterion when determining the response to the user's input.
 *
 * @param escape_message String to display if the user successfully escapes
 * the procedure.
 *
 * @param return_string_on_escape String to return in the event the user
 * escapes the procedure.
 *
 * @param message_on_unsuccessful_read Message to display if the read
 * operation itself fails. (If this happens it means no user input has even
 * been received that might be checked against the criterion.)

 * @param is Input stream from which to accept input.
 * 
 * @param os Output stream to which messages are to printed in the event of
 * input error or input that does not satisfy the criterion.
 *
 * Note that by default, newlines are \e not printed after any of the error
 * messages or etc. displayed to the user.
 */
std::string get_constrained_user_input
(	boost::function< bool(std::string const&) > criterion,
	std::string const& message_on_invalid_input,
	bool user_can_escape = true,
	boost::function< bool(std::string const&) > escape_criterion =
		boost::bind(&std::string::empty, _1),
	std::string const& escape_message = "\nProcedure aborted.\n",
	std::string const& return_string_on_escape = "",
	std::string const& message_on_unsuccessful_read =
		"\nThere has been an error reading your input. Please try again: ",	
	std::istream& is = std::cin,
	std::ostream& os = std::cout
);

/**
 * @returns a jewel::Decimal from user's input. User is reprompted until
 * a valid Decimal is entered.
 *
 * Does not throw (except possibly std::bad_alloc in extreme conditions).
 */
jewel::Decimal get_decimal_from_user();

/**
 * @returns a boost::gregorian::date from user's input. User is
 * reprompted until a valid date is entered. The date must be entered
 * in six-digit ISO format of the form "YYYYDDMM". The initial prompt
 * is \e not displayed within this function, and should be output
 * prior to calling this function.
 *
 * @todo Determine and document throwing behaviour.
 *
 * @todo This function does not currently provide any help to the user
 * as to why their input failed, in the case that the failure
 * is due to a date prior to 1400 CE being rejected by the contructor
 * of boost::gregorian::date.
 */
boost::gregorian::date get_date_from_user
(	std::string const& error_prompt =
		"Try again, entering a six-digit date in the form YYYYMMDD: "
);





}  // namespace consolixx


#endif  // GUARD_get_input_hpp
