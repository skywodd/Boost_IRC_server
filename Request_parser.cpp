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
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "Request_parser.hpp"
#include "Debug_log.hpp"

irc::Arguments_Parser::Arguments_Parser(const std::string& raw_input) :
		m_arguments(), m_nb_arguments(0) {

	/* Create input stream */
	std::istringstream iss(raw_input);
	std::string tmp;

	/* Get all arguments */
	while (std::getline(iss, tmp, ',')) {

		/* Add to arguments list */
		m_arguments.push_back(tmp);
		++m_nb_arguments;
	}
}

irc::Arguments_Parser::~Arguments_Parser() {
}

const std::vector<std::string>& irc::Arguments_Parser::getArguments(
		void) const {

	/* Return the list of arguments of the command */
	return m_arguments;
}

int irc::Arguments_Parser::getArgumentsCount(void) const {

	/* Return the number of arguments of the command */
	return m_nb_arguments;
}

irc::Request_parser::Request_parser(boost::asio::streambuf& buffer) :
		m_prefix(), m_has_prefix(false), m_command(), m_arguments(), m_nb_arguments(
				0) {

	/* Get raw line */
	std::istream is(&buffer);
	std::getline(is, m_line, '\n');

	/* Erase \r at end of string */
	m_line.erase(m_line.size() - 1, 1);
}

irc::Request_parser::~Request_parser() {
}

bool irc::Request_parser::parse(void) {

	/* Parse request */
	debug::DEBUG_LOG("Parsing request ...");

	/* Create input stream */
	std::istringstream iss(m_line);
	std::string tmp, rtmp;

	/* Get the first word */
	std::getline(iss, tmp, ' ');

	/* Check for blank line */
	if (!iss)
		return false;

	/* Check for prefix */
	if (tmp[0] == ':') {

		/* Turn on flag */
		m_has_prefix = true;

		/* remove ':' */
		tmp.erase(0, 1);

		/* Store prefix */
		m_prefix = tmp;

		/* Get the second word */
		std::getline(iss, tmp, ' ');

		/* Check for end of line */
		if (!iss)
			return false;
	}

	/* Store command */
	boost::to_upper(tmp);
	m_command = tmp;

	/* Get the next word */
	std::getline(iss, tmp, ' ');

	/* For each words remaining */
	while (iss) {

		/* Check for end-of-line string */
		if (tmp[0] == ':') {

			/* remove ':' */
			tmp.erase(0, 1);

			/* Get the end of line */
			std::getline(iss, rtmp, '\n');
			tmp += " " + rtmp; // Fix skipped whitespaces
		}

		/* Add to arguments list */
		m_arguments.push_back(tmp);
		++m_nb_arguments;

		/* Get the next word */
		std::getline(iss, tmp, ' ');
	}

	return true;
}

const std::string& irc::Request_parser::getPrefix(void) const {

	/* Return prefix string */
	return m_prefix;
}

bool irc::Request_parser::hasPrefix(void) const {

	/* Return prefix flag */
	return m_has_prefix;
}

const std::string& irc::Request_parser::getCommand(void) const {

	/* Return the IRC command */
	return m_command;
}

const std::vector<std::string>& irc::Request_parser::getArguments(void) const {

	/* Return the list of arguments of the command */
	return m_arguments;
}

int irc::Request_parser::getArgumentsCount(void) const {

	/* Return the number of arguments of the command */
	return m_nb_arguments;
}

const std::string& irc::Request_parser::getRaw(void) const {

	/* Return raw request line */
	return m_line;
}
