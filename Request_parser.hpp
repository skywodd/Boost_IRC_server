/**
 * @file Request_parser.hpp
 * @brief IRC request parser
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

#ifndef REQUESTPARSER_H_
#define REQUESTPARSER_H_

/* Dependencies */
#include <vector>
#include <string>
#include <boost/asio.hpp>

/**
 * @namespace irc
 *
 * @brief Namespace regrouping all IRC features of the program.
 */
namespace irc {

/**
 * @class Arguments_Parser
 * @brief IRC request arguments parser
 *
 * This class is designed to parse IRC arguments list.\n
 * This class take a string as input, split it using ',' as separator and return a list of string.
 */
class Arguments_Parser {
protected:
	/** Arguments list */
	std::vector<std::string> m_arguments;

	/** Number of arguments */
	int m_nb_arguments;

public:
	/**
	 * Instantiate a new parser and process the input string
	 *
	 * @param raw_input Input string to parse
	 */
	Arguments_Parser(const std::string& raw_input);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Arguments_Parser();

	/**
	 * Get the list of arguments
	 *
	 * @return The list of arguments
	 */
	const std::vector<std::string>& getArguments(void) const;

	/**
	 * Get the number of arguments
	 *
	 * @return The number of arguments
	 */
	int getArgumentsCount(void) const;
};

/**
 * @class Request_parser
 * @brief IRC request parser
 *
 * This class is designed to parse IRC request.\n
 * The parser handle request with or without prefix, single command or erroneous request.
 */
class Request_parser {
protected:
	/** Prefix header */
	std::string m_prefix;

	/** True if request has prefix header, false otherwise */
	bool m_has_prefix;

	/** IRC command */
	std::string m_command;

	/** IRC arguments list (space as separator) */
	std::vector<std::string> m_arguments;

	/** Number of arguments */
	int m_nb_arguments;

	/** Raw input request line */
	std::string m_line;

public:
	/**
	 * Instantiate a new IRC request parser and grab a line from input stream buffer
	 *
	 * @remarks Line grabbed from the buffer will be removed from the stream.
	 * @param buffer Input stream buffer from raw line read
	 */
	Request_parser(boost::asio::streambuf& buffer);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Request_parser();

	/**
	 * Parse input request line and decode IRC command (if any)
	 *
	 * @return True if parsing is successfully, false otherwise
	 */
	bool parse(void);

	/**
	 * Get the prefix header of the command
	 *
	 * @return The prefix header of the command
	 */
	const std::string& getPrefix(void) const;

	/**
	 * Check if the command has a prefix header
	 *
	 * @return True if the command has a prefix header, false otherwise
	 */
	bool hasPrefix(void) const;

	/**
	 * Get the IRC command
	 *
	 * @return The IRC command
	 */
	const std::string& getCommand(void) const;

	/**
	 * Get the arguments of the IRC command
	 *
	 * @return The arguments of the IRC command
	 */
	const std::vector<std::string>& getArguments(void) const;

	/**
	 * Get the number of arguments of the IRC command
	 *
	 * @return The number of arguments of the IRC command
	 */
	int getArgumentsCount(void) const;

	/**
	 * Get the raw request line
	 *
	 * @return The raw request line
	 */
	const std::string& getRaw(void) const;

};

} /* namespace irc */

#endif /* REQUESTPARSER_H_ */
