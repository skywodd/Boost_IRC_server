/**
 * @file Sanity_check.hpp
 * @brief User input sanity checkig utility
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is designed to check user input against forbidden format or characters.\n
 * \n
 * Please report bug to <skywodd at gmail.com>
 *
 * @section licence_sec Licence
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
 */
namespace irc {

/**
 * @class Sanity_check
 */
class Sanity_check {
public:
	/**
	 * Check if input is a valid nickname string
	 *
	 * @return True if the input is a valid nickname string
	 */
	static bool is_valid_nickname(const std::string& input);

	/**
	 * Check if input is a valid channel name string
	 *
	 * @return True if the input is a valid channel name string
	 */
	static bool is_valid_channame(const std::string& input);

	/**
	 * Check if input is a valid hostname string
	 *
	 * @return True if the input is a valid hostname string
	 */
	static bool is_valid_hostname(const std::string& input);

	//static bool is_valid_(const std::string& input);
};

} /* namespace irc */

#endif /* SANITY_CHECK_H_ */
