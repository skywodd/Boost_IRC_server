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
#include "Prefix_wrapper.hpp"

irc::Prefix_generator::Prefix_generator(const std::string& nickname,
		const std::string& username, const std::string& hostname) :
		m_oss() {

	/* Generate prefix string */
	m_oss << nickname;
	if (username != "")
		m_oss << "!" << username;
	if (hostname != "")
		m_oss << "@" << hostname;
}

irc::Prefix_generator::~Prefix_generator(void) {
}

const std::string& irc::Prefix_generator::toString(void) {

	/* Return the generated prefix string */
	return m_oss.str();
}

irc::Prefix_parser::Prefix_parser(const std::string& raw_input) :
		m_iss(raw_input) {

	/* Parse prefix */
	std::getline(m_iss, m_nickname, '!');
	std::getline(m_iss, m_username, '@');
	std::getline(m_iss, m_hostname);
}

irc::Prefix_parser::~Prefix_parser(void) {
}

const std::string& irc::Prefix_parser::getNickname(void) const {

	/* Return the nickname of the prefix */
	return m_nickname;
}

const std::string& irc::Prefix_parser::getUsername(void) const {

	/* Return the username of the prefix */
	return m_username;
}

const std::string& irc::Prefix_parser::getHostname(void) const {

	/* Return the hostname of the prefix */
	return m_hostname;
}
