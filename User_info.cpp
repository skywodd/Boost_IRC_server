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
#include "User_info.hpp"
#include "Configuration.hpp"
#include "Channel_info.hpp"
#include "Debug_log.hpp"

irc::User_info::User_info(const Configuration& configuration) :
		m_nickname(), m_username(), m_hostname(), m_servername(), m_realname(), m_state(
				REQUESTING_INIT), m_is_ircop(configuration.is_ircop), m_is_receiving_wallops(
				configuration.is_receiving_wallops), m_is_receiving_notices(
				configuration.is_receiving_notices), m_is_invisible(
				configuration.is_invisible), m_is_away(configuration.is_away), m_away_message(
				configuration.away_message), m_nb_join(0), m_nb_join_limit(
				configuration.nb_join_limit), m_channels_joined() {

	/* Generate a new anonymous nickname (used to track init process) */
	m_nickname = generate_nickname();
	debug::DEBUG_LOG("Creating user", m_nickname);
	debug::DEBUG_LOG(m_nickname, "IRC op", m_is_ircop);
	debug::DEBUG_LOG(m_nickname, "Receiving wallops", m_is_receiving_wallops);
	debug::DEBUG_LOG(m_nickname, "Receving notices", m_is_receiving_notices);
	debug::DEBUG_LOG(m_nickname, "Invisible", m_is_invisible);
	debug::DEBUG_LOG(m_nickname, "Away", m_is_away);
	debug::DEBUG_LOG(m_nickname, "Away msg", m_away_message);
	debug::DEBUG_LOG(m_nickname, "Join limit", m_nb_join_limit);

	/* Initialize connection state according to the server protection */
	if (configuration.is_password_protected) { /* Server is password protected */

		/* Waiting for PASS command */
		debug::DEBUG_LOG(m_nickname, "Waiting for PASS ...");
		m_state = WAIT_FOR_PASS;

	} else { /* Server is not password protected */

		/* Waiting for NICK command */
		debug::DEBUG_LOG(m_nickname, "Waiting for NICK ...");
		m_state = WAIT_FOR_NICK;
	}
}

irc::User_info::~User_info(void) {

	/* Destroying user object */
	debug::DEBUG_LOG(m_nickname, "Destroying user ...");
}

const std::string& irc::User_info::getNickname(void) const {

	/* Return the nickname of the user */
	return m_nickname;
}

void irc::User_info::setNickname(const std::string& nickname) {

	/* Set the nickname of the user */
	debug::DEBUG_LOG(m_nickname, "Nickname", nickname);
	m_nickname = nickname;
	// TODO update prefix
}

const std::string& irc::User_info::getUsername(void) const {

	/* Return the username of the user */
	return m_username;
}

void irc::User_info::setUsername(const std::string& username) {

	/* Set the username of the user */
	debug::DEBUG_LOG(m_nickname, "Username", username);
	m_username = username;
	// TODO update prefix
}

const std::string& irc::User_info::getHostname(void) const {

	/* Return the hostname of the user */
	return m_hostname;
}

const std::string& irc::User_info::getServername(void) const {

	/* Return the server name of the user */
	return m_servername;
}

const std::string& irc::User_info::getRealname(void) const {

	/* Return the real name of the user */
	return m_realname;
}

void irc::User_info::setRealname(const std::string& realname) {

	/* Set the real name of the user */
	debug::DEBUG_LOG(m_nickname, "Real name", realname);
	m_realname = realname;
}

const std::string& irc::User_info::getPrefix(void) const {

	/* Return the current pre-computed prefix */
	return m_prefix;
}

unsigned char irc::User_info::getState(void) const {

	/* Return the connection state of the user */
	return m_state;
}

void irc::User_info::setState(const unsigned char state) {

	/* Set the connection state of the user */
	debug::DEBUG_LOG(m_nickname, "Conn state", (int) state);
	m_state = state;
}

bool irc::User_info::isIrcOp(void) const {

	/* Return the "irc op" status of the user */
	return m_is_ircop;
}

void irc::User_info::setIrcOp(const bool is_ircop) {

	/* Set the "irc op" status of the user */
	debug::DEBUG_LOG(m_nickname, "IRC op", is_ircop);
	m_is_ircop = is_ircop;
}

bool irc::User_info::isReceivingWallops(void) const {

	/* Return the "receiving wallops" status of the user */
	return m_is_receiving_wallops;
}

void irc::User_info::setReceivingWallops(const bool is_receiving_wallops) {

	/* Set the "receiving wallops" status of the user */
	debug::DEBUG_LOG(m_nickname, "receiving wallops", is_receiving_wallops);
	m_is_receiving_wallops = is_receiving_wallops;
}

bool irc::User_info::isReceivingNotices(void) const {

	/* Return the "receiving notices" status of the user */
	return m_is_receiving_notices;
}

void irc::User_info::setReceivingNotices(const bool is_receiving_notices) {

	/* Set the "receiving notices" status of the user */
	debug::DEBUG_LOG(m_nickname, "Receiving notices", is_receiving_notices);
	m_is_receiving_notices = is_receiving_notices;
}

bool irc::User_info::isInvisible(void) const {

	/* Return the "invisible" status of the user */
	return m_is_invisible;
}

void irc::User_info::setInvisible(const bool is_invisible) {

	/* Set the "invisible" status of the user */
	debug::DEBUG_LOG(m_nickname, "Invisible", is_invisible);
	m_is_invisible = is_invisible;
}

bool irc::User_info::isAway(void) const {

	/* Return the "is away" status of the user */
	return m_is_away;
}

void irc::User_info::setAway(const bool is_away) {

	/* Set the "is away" status of the user */
	debug::DEBUG_LOG(m_nickname, "Away", is_away);
	m_is_away = is_away;
}

const std::string& irc::User_info::getAwayMsg(void) const {

	/* Return the "is away" status of the user */
	return m_away_message;
}

void irc::User_info::setAwayMsg(const std::string& away_msg) {

	/* Set the "is away" status of the user */
	debug::DEBUG_LOG(m_nickname, "Away msg", away_msg);
	m_away_message = away_msg;
}

int irc::User_info::getJoinCount(void) const {

	/* Return the number of channels joined by the user */
	return m_nb_join;
}

int irc::User_info::getJoinCoutLimit(void) const {

	/* Return the maximum number of channels joinable by the user */
	return m_nb_join_limit;
}

void irc::User_info::addJoin(boost::shared_ptr<Channel_info> channel_ptr) {

	/* Add a channel to the user's join history */
	debug::DEBUG_LOG(m_nickname, "Add channel to history");
	if (m_channels_joined.insert(channel_ptr).second) /* Check for error */
		++m_nb_join; /* Update channels count */
}

void irc::User_info::removeJoin(boost::shared_ptr<Channel_info> channel_ptr) {

	/* Remove a channel from the user's join history */
	debug::DEBUG_LOG(m_nickname, "Remove channel from history");
	if (m_channels_joined.erase(channel_ptr)) /* Check for error */
		--m_nb_join; /* Update channels count */
}

bool irc::User_info::asJoin(boost::shared_ptr<Channel_info> channel_ptr) const {

	/* Check if user as join the specified channel */
	return m_channels_joined.count(channel_ptr);
}

std::string irc::User_info::generate_nickname(void) {

	/* Name seed (static to be shared with all threads) */
	static int name_seed = 0;

	/* Craft nickname from seed */
	std::ostringstream oss;
	oss << "Anon_" << std::hex << name_seed;

	/* Go to the next seed */
	++name_seed;

	/* Return generated nickname */
	return oss.str();
}
