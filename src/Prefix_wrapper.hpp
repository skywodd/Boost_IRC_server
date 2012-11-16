/**
 * @file Prefix_wrapper.hpp
 * @brief IRC prefix parser / generator
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

#ifndef PREFIX_WRAPPER_H_
#define PREFIX_WRAPPER_H_

/* Dependencies */
#include <sstream>
#include <string>

/**
 * @namespace irc
 *
 * @brief Namespace regrouping all IRC features of the program.
 */
namespace irc {

/**
 * @class Prefix_generator
 * @brief IRC prefix generator
 *
 * This class is designed to craft IRC prefix.\n
 * \n
 * This class can generate four types of IRC prefix :\n
 * * Single nickname prefix\n
 * * Nickname and user name prefix
 * * Full user prefix (nickname, user name and server name)\n
 * * Server prefix
 */
class Prefix_generator {
protected:
	/** Output prefix string stream */
	std::ostringstream m_oss;

public:
	/**
	 * Instantiate a new generator and craft the IRC prefix
	 *
	 * @param nickname Nickname of user
	 * @param username User name of user
	 * @param hostname Host name of user or server domain
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
	 * Get the computed prefix in string format
	 *
	 * @return The computed prefix in string format
	 */
	const std::string toString(void);
};

/**
 * @class Prefix_parser
 * @brief IRC prefix parser
 *
 * This class is designed to parse IRC prefix.\n
 * \n
 * This class can parse four types of IRC prefix :\n
 * * Single nickname prefix\n
 * * Nickname and user name prefix\n
 * * Full user prefix (nickname, user name and server name)\n
 * * Server prefix
 */
class Prefix_parser {
protected:
	/** Input prefix string stream */
	std::istringstream m_iss;

	/** Prefix : nick name field */
	std::string m_nickname;

	/** Prefix : user name field */
	std::string m_username;

	/** Prefix : host name field or server domain field */
	std::string m_hostname;

public:
	/**
	 * Instantiate a new parser and process input line
	 *
	 * @param raw_input The raw prefix line input
	 */
	Prefix_parser(const std::string& raw_input);

	/**
	 * Default destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Prefix_parser(void);

	/**
	 * Get the nickname field of the prefix
	 *
	 * @return The nickname field of the prefix
	 */
	const std::string& getNickname(void) const;

	/**
	 * Get the user name field of the prefix
	 *
	 * @return The user name field of the prefix
	 */
	const std::string& getUsername(void) const;

	/**
	 * Get the host name field of the prefix
	 *
	 * @return The host name field of the prefix
	 */
	const std::string& getHostname(void) const;

};

} /* namespace irc */

#endif /* PREFIX_WRAPPER_H_ */
