/**
 * @file Users_manager.hpp
 * @brief Users database manager
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is designed to manage users database.\n
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

#ifndef USER_MANAGER_H_
#define USER_MANAGER_H_

/* Dependencies */
#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>

/**
 * @namespace irc
 */
namespace irc {

/* Forward declarations */
class Connection;
class Configuration;

/**
 * @class Users_manager
 */
class Users_manager: private boost::noncopyable {
protected:
	/** Users database */
	std::set<boost::shared_ptr<Connection> > m_database;

	/** Number of users currently in the database */
	int m_nb_users;

	/** Maximum number of users of the database */
	int m_nb_users_limit;

	/**
	 * Search user informations in database by nickname
	 *
	 * @param cur_user Pointer to the user informations to test
	 * @param nickname Nickname to search in the database
	 * @return True if the nickname match, false otherwise
	 */
	static bool find_by_nickname(boost::shared_ptr<Connection> cur_user,
			std::string& nickname);

	/**
	 * Send wallops message to IRC op (if user is receiving wallops)
	 *
	 * @param user Pointer to the user to wallops
	 * @param message Wallops message to send
	 */
	static void send_wallops(boost::shared_ptr<Connection> user,
			std::string& message);

	/**
	 * Send notice message to user (if user is receiving notice)
	 *
	 * @param user Pointer to the user to notice
	 * @param message Wallops message to send
	 */
	static void send_notice(boost::shared_ptr<Connection> user,
			std::string& message);

public:

	/**
	 * Instantiate a new users database
	 *
	 * @param configuration Database configuration
	 */
	explicit Users_manager(const Configuration& configuration);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	~Users_manager(void);

	/**
	 * Get the number of users currently in the database
	 *
	 * @return The number of users currently in the database
	 */
	int getUsersCount(void) const;

	/**
	 * Get the maximum number of users of the database
	 *
	 * @return The maximum number of users of the database
	 */
	int getUsersCountLimit(void) const;

	/**
	 * Add an user to the database
	 *
	 * @param user Pointer to the informations about the user to add
	 */
	void add(boost::shared_ptr<Connection> user);

	/**
	 * Remove an user from the database
	 *
	 * @param user Pointer to the informations about the user to remove
	 */
	void remove(boost::shared_ptr<Connection> user);

	/**
	 * Access to the informations about an user
	 *
	 * @param nickname Nickname of the user to search & retrieve from the database
	 * @return Pointer to the informations about the user, or NULL if not exist
	 */
	boost::shared_ptr<Connection> access(const std::string& nickname);

	/**
	 * Write data to all users in the database (wall message)
	 *
	 * @param buffer Data to send to all users
	 */
	void writeToAll(const std::string& buffer);

	/**
	 * Write data to all IRC ops in the database (wallops message)
	 *
	 * @param buffer Data to send to all IRC ops
	 */
	void writeToIrcOp(const std::string& buffer);

};

}

#endif /* USER_MANAGER_H_ */
