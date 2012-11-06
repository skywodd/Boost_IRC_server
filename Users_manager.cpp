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
#include "Users_manager.hpp"
#include "Configuration.hpp"
#include "Connection.hpp"
#include "Debug_log.hpp"

bool irc::Users_manager::find_by_nickname(
		boost::shared_ptr<Connection> cur_user, std::string& nickname) {

	/* Check if nickname match */
	return cur_user->getNickname() == nickname;
}

void irc::Users_manager::send_wallops(boost::shared_ptr<Connection> user,
		std::string& message) {

	/* If user is ready for msg, is receiving wallops and is an IRC op */
	if ((user->getState() == Connection::READY_FOR_MSG)
			&& user->isReceivingWallops() && user->isIrcOp())
		/* Send message to the user */
		user->write(message);
}

void irc::Users_manager::send_notice(boost::shared_ptr<Connection> user,
		std::string& message) {

	/* If user is ready for msg and is receiving notices */
	if ((user->getState() == Connection::READY_FOR_MSG)
			&& user->isReceivingNotices())
		/* Send message to the user */
		user->write(message);
}

irc::Users_manager::Users_manager(const Configuration& configuration) :
		m_database(), m_nb_users(0), m_nb_users_limit(
				configuration.nb_users_limit) {

	/* Create a new users database */
	debug::DEBUG_LOG("Users database", "Creating database ...");
	debug::DEBUG_LOG("Users database", "max numbers of users",
			m_nb_users_limit);
}

irc::Users_manager::~Users_manager(void) {

	/* Destroying the database object */
	debug::DEBUG_LOG("Users database", "Destroying database ...");
}

int irc::Users_manager::getUsersCount(void) const {

	/* Return the numbers of users currently in the database */
	return m_nb_users;
}

int irc::Users_manager::getInvisibleUsersCount(void) const {

	/* Count result */
	int nb_users = 0;

	/* Test each users */
	std::set<boost::shared_ptr<Connection> >::iterator i = m_database.begin();
	std::set<boost::shared_ptr<Connection> >::iterator end;
	for (; i != end; ++i) {

		/* Check user status */
		if ((*i)->isInvisible())
			++nb_users;
	}

	/* Return result */
	return nb_users;
}

int irc::Users_manager::getIRCopsCount(void) const {

	/* Count result */
	int nb_users = 0;

	/* Test each users */
	std::set<boost::shared_ptr<Connection> >::iterator i = m_database.begin();
	std::set<boost::shared_ptr<Connection> >::iterator end;
	for (; i != end; ++i) {

		/* Check user status */
		if ((*i)->isIrcOp())
			++nb_users;
	}

	/* Return result */
	return nb_users;
}

int irc::Users_manager::getUsersCountLimit(void) const {

	/* Return the maxmimum numbers of users of the database */
	return m_nb_users_limit;
}

void irc::Users_manager::add(boost::shared_ptr<Connection> user) {

	/* Add an user to the database */
	debug::DEBUG_LOG("Users database", "add user", user->getNickname());
	if (m_database.insert(user).second) /* Check for error */
		++m_nb_users; /* Update users count */
}

void irc::Users_manager::remove(boost::shared_ptr<Connection> user) {

	/* Remove an user from the database */
	debug::DEBUG_LOG("Users database", "remove user", user->getNickname());
	if (m_database.erase(user))/* Check for error */
		--m_nb_users; /* Update users count */
}

boost::shared_ptr<irc::Connection> irc::Users_manager::access(
		const std::string& nickname) {

	/* Acces to user informations */
	debug::DEBUG_LOG("Users database", "search user", nickname);
	std::set<boost::shared_ptr<Connection> >::iterator result;

	/* Search user in database */
	result = std::find_if(m_database.begin(), m_database.end(),
			boost::bind(&Users_manager::find_by_nickname, _1, nickname));

	/* Check if user have been found or not */
	if (result != m_database.end())
		return *result; /* User found, return pointer to it */
	else
		/* User not found, return null pointer */
		return boost::shared_ptr<irc::Connection>();
}

void irc::Users_manager::writeToAll(const std::string& buffer) {

	/* Write message to all users in the database */
	debug::DEBUG_LOG("Users database", "write to all users", buffer);
	std::for_each(m_database.begin(), m_database.end(),
			boost::bind(&Users_manager::send_notice, _1, buffer));
}

void irc::Users_manager::writeToIrcOp(const std::string& buffer) {

	/* Write message to all IRC ops in the database */
	debug::DEBUG_LOG("Users database", "write to all IRC ops", buffer);
	std::for_each(m_database.begin(), m_database.end(),
			boost::bind(&Users_manager::send_wallops, _1, buffer));
}
