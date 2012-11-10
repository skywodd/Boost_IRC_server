/**
 * @file Users_manager.hpp
 * @brief Users database manager
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * Please report bug to <skywodd at gmail.com>
 *
 * @todo Users research using nickmask expression support
 *
 * @section license_sec License
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
 *
 * Namespace regrouping all IRC features of the program.
 */
namespace irc {

/* Forward declarations */
class Connection;
class Configuration;

/**
 * @class Users_manager
 *
 * This class is designed to manage users database.\n
 * This class allow developer to add, remove and access to users informations in the database.\n
 * \n
 * Add and remove operations are made using intelligent pointers.\n
 * Access operations are made using nickname.\n
 * \n
 * This class keep up-to-date a counter of users currently into the database.\n
 * This class can also count the number of IRC ops and invisible users.\n
 * \n
 * Broadcast operations (to all users or only to IRC ops) are handled by this class.
 */
class Users_manager: private boost::noncopyable {
protected:
	/** Users database */
	std::set<boost::shared_ptr<Connection> > m_database;

	/**
	 * Functor : Search user in the database by nickname
	 *
	 * @param cur_user Pointer to the user informations to test
	 * @param nickname Nickname to search in the database
	 * @return True if the nickname match, false otherwise
	 */
	static bool find_by_nickname(boost::shared_ptr<Connection> user,
			std::string& nickname);

	/**
	 * Functor : Send wallops message to user
	 *
	 * @remarks Message will be sent to user only if user is IRC op and have "receive wallops" set to true.
	 * @param user Pointer to the user to wallops
	 * @param message Wallops message to send
	 */
	static void send_wallops(boost::shared_ptr<Connection> user,
			std::string& message);

	/**
	 * Functor : Send notice message to user
	 *
	 * @remarks Message will be sent to user only if user have "receive notice" set to true.
	 * @param user Pointer to the user to notice
	 * @param message Notice message to send
	 */
	static void send_notice(boost::shared_ptr<Connection> user,
			std::string& message);

public:
	/**
	 * Instantiate a new users database
	 *
	 * @param configuration Database configuration to use
	 */
	explicit Users_manager(const Configuration& configuration);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	~Users_manager(void);

	/**
	 * Get the number of users currently into the database
	 *
	 * @return The number of users currently into the database
	 */
	int getUsersCount(void) const;

	/**
	 * Get the number of invisible users into the database
	 *
	 * @return The number of invisible users into the database
	 */
	int getInvisibleUsersCount(void) const;

	/**
	 * Get the number of IRC ops into the database
	 *
	 * @return The number of IRC ops into the database
	 */
	int getIRCopsCount(void) const;

	/**
	 * Get the maximum number of users of the database
	 *
	 * @return The maximum number of users of the database
	 */
	int getUsersCountLimit(void) const;

	/**
	 * Add an user into the database
	 *
	 * @param user Intelligent pointer to the user to add into the database
	 */
	void add(boost::shared_ptr<Connection> user);

	/**
	 * Remove an user from the database
	 *
	 * @param user Intelligent pointer to the user to remove from the database
	 */
	void remove(boost::shared_ptr<Connection> user);

	/**
	 * Access to the informations about an user
	 *
	 * @param nickname Nickname of the user to retrieve from the database
	 * @return An intelligent pointer to the user or NULL if not found
	 */
	boost::shared_ptr<Connection> access(const std::string& nickname);

	/**
	 * Send data to all users in the database (wall message)
	 *
	 * @param buffer Data to send to all users
	 */
	void writeToAll(const std::string& buffer);

	/**
	 * Send data to all IRC ops in the database (wallops message)
	 *
	 * @param buffer Data to send to all IRC ops
	 */
	void writeToIrcOp(const std::string& buffer);

};

} /* namespace irc */

#endif /* USER_MANAGER_H_ */
