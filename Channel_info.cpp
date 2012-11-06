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
#include <algorithm>
#include <boost/bind.hpp>
#include "Channel_info.hpp"
#include "Configuration.hpp"
#include "Connection.hpp"
#include "Debug_log.hpp"

void irc::Channel_info::send_message(
		std::map<boost::shared_ptr<Connection>, Channel_user_info>::value_type user_pair,
		std::string& message) {
	user_pair.first->write(message);
}

irc::Channel_info::Channel_user_info::Channel_user_info(const bool can_speak,
		const bool is_op) :
		m_can_speak(can_speak), m_is_op(is_op) {
}

bool irc::Channel_info::Channel_user_info::canSpeak(void) const {

	/* Return "can speak" flag of user */
	return m_can_speak;
}

void irc::Channel_info::Channel_user_info::setCanSpeak(const bool can_speak) {

	/* Set "can speak" flag of user */
	m_can_speak = can_speak;
}

bool irc::Channel_info::Channel_user_info::isOp(void) const {

	/* Return "is op" flag of user */
	return m_is_op;
}

void irc::Channel_info::Channel_user_info::setOp(const bool is_op) {

	/* Set "is op" flag of user */
	m_is_op = is_op;
}

irc::Channel_info::Channel_info(const Configuration& configuration) :
		m_topic(), m_password(), m_private(configuration.is_private), m_secret(
				configuration.is_secret), m_invite_only(
				configuration.is_invite_only), m_topic_setby_op_only(
				configuration.topic_setby_op_only), m_no_outside_msg(
				configuration.no_outside_msg), m_moderated(
				configuration.is_moderated), m_users_limit(
				configuration.users_limit_per_channel), m_nb_users(0), m_banmask(), m_invitations(), m_users() {

	/* Creating new channel */
	debug::DEBUG_LOG("Creating channel ...");
	debug::DEBUG_LOG("New channel", "Private", m_private);
	debug::DEBUG_LOG("New channel", "Secret", m_secret);
	debug::DEBUG_LOG("New channel", "Invite only", m_invite_only);
	debug::DEBUG_LOG("New channel", "Topic setby ops only", m_topic_setby_op_only);
	debug::DEBUG_LOG("New channel", "No outside msg", m_no_outside_msg);
	debug::DEBUG_LOG("New channel", "Moderated", m_moderated);
	debug::DEBUG_LOG("New channel", "Users count limit", m_users_limit);
}

boost::shared_ptr<irc::Channel_info> irc::Channel_info::create(
		const Configuration& configuration) {

	/* Instanciate new channel shared_ptr */
	return boost::shared_ptr<irc::Channel_info>(new Channel_info(configuration));
}

irc::Channel_info::~Channel_info(void) {

	/* Destroying channel object */
	debug::DEBUG_LOG("Destroying channel ...");
}

const std::string& irc::Channel_info::getTopic(void) const {

	/* Return topic string */
	return m_topic;
}

void irc::Channel_info::setTopic(const std::string& topic) {

	/* Set topic string */
	m_topic = topic;
}

const std::string& irc::Channel_info::getPassword(void) const {

	/* Return channel password */
	return m_password;
}

void irc::Channel_info::setPassword(const std::string& password) {

	/* Set channel password */
	m_password = password;
}

bool irc::Channel_info::isPrivate(void) const {

	/* Return "is private" flag of channel */
	return m_private;
}

void irc::Channel_info::setPrivate(const bool is_private) {

	/* Set "is private" flag of channel */
	m_private = is_private;
}

bool irc::Channel_info::isSecret(void) const {

	/* Return "is secret" flag of channel */
	return m_secret;
}

void irc::Channel_info::setSecret(const bool is_secret) {

	/* Set "is secret" flag of channel */
	m_secret = is_secret;
}

bool irc::Channel_info::isInviteOnly(void) const {

	/* Return "is invite only" flag of channel */
	return m_invite_only;
}

void irc::Channel_info::setInviteOnly(const bool is_invite_only) {

	/* Set "is invite only" flag of channel */
	m_invite_only = is_invite_only;
}

bool irc::Channel_info::isTopicSetByOpOnly(void) const {

	/* Return "topic setby ops only" flag of channel */
	return m_topic_setby_op_only;
}

void irc::Channel_info::setTopicSetByOpOnly(const bool is_topic_setby_op_only) {

	/* Set "topic setby ops only" flag of channel */
	m_topic_setby_op_only = is_topic_setby_op_only;
}

bool irc::Channel_info::isNoOutsideMsg(void) const {

	/* Return "no outside msg" flag of channel */
	return m_no_outside_msg;
}

void irc::Channel_info::setNoOutsideMsg(const bool no_outside_msg) {

	/* Set "no outside msg" flag of channel */
	m_no_outside_msg = no_outside_msg;
}

bool irc::Channel_info::isModerated(void) const {

	/* Return "is moderated" flag of channel */
	return m_moderated;
}

void irc::Channel_info::setModerated(const bool is_moderated) {

	/* Set "is moderated" flag of channel */
	m_moderated = is_moderated;
}

int irc::Channel_info::getUsersLimit(void) const {

	/* Return users count limit of the channel */
	return m_users_limit;
}

void irc::Channel_info::setUsersLimit(const int users_limit) {

	/* Set users count limit of the channel */
	m_users_limit = users_limit;
}

int irc::Channel_info::getUsersCount(void) const {

	/* Return current users count of the channel */
	return m_nb_users;
}

void irc::Channel_info::addJoin(boost::shared_ptr<Connection> user,
		const bool is_op) {

	/* Add user to the channel */
	debug::DEBUG_LOG("Add user to channel", user->getNickname());
	m_users[user] = Channel_user_info((m_moderated) ? false : true, is_op);
	++m_nb_users; /* Update users count */
}

void irc::Channel_info::removeJoin(boost::shared_ptr<Connection> user) {

	/* Remove user from channel */
	debug::DEBUG_LOG("Remove user from channel", user->getNickname());
	if(m_users.erase(user)) /* Check for error */
		--m_nb_users; /* Update users count */
}

bool irc::Channel_info::asJoin(boost::shared_ptr<Connection> user) const {

	/* Check if user as join channel or not */
	return m_users.count(user);
}

void irc::Channel_info::addInvitation(boost::shared_ptr<Connection> user) {

	/* Add invitation to join the channel */
	debug::DEBUG_LOG("Add invitation to join channel", user->getNickname());
	m_invitations.insert(user);
}

void irc::Channel_info::removeInvitation(boost::shared_ptr<Connection> user) {

	/* Remove invitation to join the channel */
	debug::DEBUG_LOG("Remove invitation to join channel", user->getNickname());
	m_invitations.erase(user);
}

bool irc::Channel_info::isInvited(boost::shared_ptr<Connection> user) const {

	/* Check if user is invited to join the channel */
	return m_invitations.count(user);
}

void irc::Channel_info::addBanmask(const std::string& banmask) {

	/* Add banmask to the channel */
	debug::DEBUG_LOG("Add banmask to channel", banmask);
	m_banmask.insert(banmask);
}

void irc::Channel_info::removeBanmask(const std::string& banmask) {

	/* Remove banmask from the channel */
	debug::DEBUG_LOG("Remove banmask from channel", banmask);
	m_banmask.erase(banmask);
}

bool irc::Channel_info::isBanned(const std::string& banmask) const {

	/* Check if banmask is banned or not */
	return m_banmask.count(banmask);
}

irc::Channel_info::Channel_user_info& irc::Channel_info::access(
		boost::shared_ptr<Connection> user) {

	/* Return channel informations of the user */
	return m_users[user];
}

void irc::Channel_info::writeToAll(const std::string& buffer) {

	/* Send message to all users on the channel */
	debug::DEBUG_LOG("Channel msg", buffer);
	std::for_each(m_users.begin(), m_users.end(),
			boost::bind(&Channel_info::send_message, _1, buffer));
}
