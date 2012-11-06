/*
 * See header file for details
 *
 *  This program is free software: you can redistribute it and/or modify\n
 *  it under the terms of the GNU General Public License as published by\n
 *  the Free Software Foundation, either version 3 of the License, or\n
 *  (at your option) any later version.\n
 *
 *  This program is distributed in the hope that it will be useful,\n
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n
 *  GNU General Public License for more details.\n
 *
 *  You should have received a copy of the GNU General Public License\n
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.\n
 */

/* Includes */
#include <boost/regex.hpp>
#include "Sanity_check.hpp"

bool irc::Sanity_check::is_valid_nickname(const std::string& input) {

	/* Compile regular expression */
	boost::regex expression(
			"^[a-z_\\-\\[\\]\\\\^{}|`][a-z0-9_\\-\\[\\]\\\\^{}|`]{0, 15}$");
	// Allow : 1 to 16 chr nickname

	/* Match the input with the regex */
	return regex_match(input, expression);
}

bool irc::Sanity_check::is_valid_channame(const std::string& input) {

	/* Compile regular expression */
	boost::regex expression("^([#&][^\r\n\0]{0, 199})$");
	// Allow : 1 to 200 chr channel name

	/* Match the input with the regex */
	return regex_match(input, expression);

}

bool irc::Sanity_check::is_valid_hostname(const std::string& input) {

	/* Compile regular expression */
	boost::regex expression(
			"^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$");

	/* Match the input with the regex */
	return regex_match(input, expression);

}
