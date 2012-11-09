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

	/* Create an input string stream */
	std::istringstream iss(raw_input);
	std::string tmp;

	/* Get all arguments using ',' as separator */
	while (std::getline(iss, tmp, ',')) {

		/* Add string to the arguments list */
		m_arguments.push_back(tmp);
		++m_nb_arguments; /* Update arguments count */
	}
}

irc::Arguments_Parser::~Arguments_Parser() {
}

const std::vector<std::string>& irc::Arguments_Parser::getArguments(
		void) const {

	/* Return the list of arguments */
	return m_arguments;
}

int irc::Arguments_Parser::getArgumentsCount(void) const {

	/* Return the number of arguments */
	return m_nb_arguments;
}

irc::Request_parser::Request_parser(boost::asio::streambuf& buffer) :
		m_prefix(), m_has_prefix(false), m_command(), m_arguments(), m_nb_arguments(
				0) {

	/* Get the raw request line */
	std::istream is(&buffer);
	std::getline(is, m_line, '\n');

	/* Erase the \r at the end of the string */
	m_line.erase(m_line.size() - 1, 1);
}

irc::Request_parser::~Request_parser() {
}

bool irc::Request_parser::parse(void) {

	/* Create an input string stream */
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
			tmp += " " + rtmp; // Fix skipped spaces
		}

		/* Add string to the arguments list */
		m_arguments.push_back(tmp);
		++m_nb_arguments; /* Update arguments count */

		/* Get the next word */
		std::getline(iss, tmp, ' ');
	}

	/* No error */
	return true;
}

const std::string& irc::Request_parser::getPrefix(void) const {

	/* Return the prefix string */
	return m_prefix;
}

bool irc::Request_parser::hasPrefix(void) const {

	/* Return the prefix flag */
	return m_has_prefix;
}

const std::string& irc::Request_parser::getCommand(void) const {

	/* Return the IRC command */
	return m_command;
}

const std::vector<std::string>& irc::Request_parser::getArguments(void) const {

	/* Return the list of arguments */
	return m_arguments;
}

int irc::Request_parser::getArgumentsCount(void) const {

	/* Return the number of arguments */
	return m_nb_arguments;
}

const std::string& irc::Request_parser::getRaw(void) const {

	/* Return the raw request line */
	return m_line;
}
