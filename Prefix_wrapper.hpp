/**
 * @file Prefix_wrapper.hpp
 * @brief IRC prefix parser / generator
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is designed to parse and generate IRC prefix.\n
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

#ifndef PREFIX_WRAPPER_H_
#define PREFIX_WRAPPER_H_

/* Dependencies */
#include <sstream>
#include <string>

/**
 * @namespace irc
 */
namespace irc {

/**
 * @class Prefix_generator
 */
class Prefix_generator {
protected:
	/** Output prefix string */
	std::ostringstream m_oss;

public:
	/**
	 * Create and generate a new IRC prefix
	 *
	 * @param nickname Nickname of user
	 * @param username Username of user
	 * @param hostname Hostname of user
	 */
	Prefix_generator(const std::string& nickname, const std::string& username =
			"", const std::string& hostname = "");

	/**
	 * Default destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Prefix_generator(void);

	/**
	 * Get the prefix in string format
	 *
	 * @return The prefix in string format
	 */
	const std::string& toString(void);
};

/**
 * @class Prefix_parser
 */
class Prefix_parser {
protected:
	/** Input prefix string */
	std::istringstream m_iss;

	/** Prefix : nick name */
	std::string m_nickname;

	/** Prefix : user name */
	std::string m_username;

	/** Prefix : host name */
	std::string m_hostname;

public:
	/**
	 * Instantiate a new IRC prefix parser object
	 *
	 * @param raw_input Raw prefix line input
	 */
	Prefix_parser(const std::string& raw_input);

	/**
	 * Default destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Prefix_parser(void);

	/**
	 * Get the nickname of prefix
	 *
	 * @return The nickname of prefix
	 */
	const std::string& getNickname(void) const;

	/**
	 * Get the username of prefix
	 *
	 * @return The username of prefix
	 */
	const std::string& getUsername(void) const;

	/**
	 * Get the hostname of prefix
	 *
	 * @return The hostname of prefix
	 */
	const std::string& getHostname(void) const;
};

} /* namespace irc */

#endif /* PREFIX_WRAPPER_H_ */
