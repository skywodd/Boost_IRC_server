/**
 * @file Sanity_check.hpp
 * @brief User input sanity checking utility
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * Please report bug to <skywodd at gmail.com>
 *
 * @section licence_sec License
 *  This program is free software: you can redistribute it and/or modify\n
 *  it under the terms of the GNU General Public License as published by\n
 *  the Free Software Foundation, either version 3 of the License, or\n
 *  (at your option) any later version.\n
 * \n
 *  This program is distributed in the hope that it will be useful,\n
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n
 *  GNU General Public License for more details.\n
 * \n
 *  You should have received a copy of the GNU General Public License\n
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.\n
 */

#ifndef SANITY_CHECK_H_
#define SANITY_CHECK_H_

/* Dependencies */
#include <string>

/**
 * @namespace irc
 *
 * Namespace regrouping all IRC features of the program.
 */
namespace irc {

/**
 * @class Sanity_check
 *
 * This class is in fact a static functions bundle.\n
 * This class contain useful functions to check validity of various IRC fields.\n
 * For now the supported fields are : nicknames, channels name, host name.
 *
 * @todo Function to transform IRC mask to boost regular expression.
 */
class Sanity_check {
public:
	/**
	 * Check if the input is a valid nickname string
	 *
	 * @return True if the input is a valid nickname string, false otherwise
	 */
	static bool is_valid_nickname(const std::string& input);

	/**
	 * Check if the input is a valid channel name string
	 *
	 * @return True if the input is a valid channel name string, false otherwise
	 */
	static bool is_valid_channame(const std::string& input);

	/**
	 * Check if the input is a valid host name string
	 *
	 * @return True if the input is a valid host name string, false otherwise
	 */
	static bool is_valid_hostname(const std::string& input);

};

} /* namespace irc */

#endif /* SANITY_CHECK_H_ */
