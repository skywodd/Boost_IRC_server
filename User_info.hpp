/**
 * @file User_info.hpp
 * @brief IRC user informations container
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is a container for IRC user informations.\n
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

#ifndef USER_INFO_H_
#define USER_INFO_H_

/* Dependencies */
#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

/**
 * @namespace irc
 */
namespace irc {

/* Forward declarations */
class Channel_info;
class Configuration;

/**
 * @class User_info
 */
class User_info: private boost::noncopyable {
public:
	/**
	 * User connection states
	 */
	typedef enum {
		REQUESTING_INIT, /*!< Connection need init before use */
		WAIT_FOR_PASS, /*!< Waiting for PASS command */
		WAIT_FOR_NICK, /*!< Waiting for NICK command */
		WAIT_FOR_USER, /*!< Waiting for USER command */
		READY_FOR_MSG, /*!< Ready for speaking over IRC network */
	} User_connection_state;

protected:
	/** User nickname (set by user with NICK command) */
	std::string m_nickname;

	/** Username (specified with USER command) */
	std::string m_username; // Read only

	/** User hostname (reverse lookup of remote endpoint) */
	std::string m_hostname; // Read only

	/** User server name (IRC relay server name of user) */
	std::string m_servername;

	/** User real name */
	std::string m_realname;

	/** Pre-computed prefix */
	std::string m_prefix;

	/** Connection state */
	unsigned char m_state;

	/** True if user is IRC op */
	bool m_is_ircop;

	/** True if user can receive wallops messages */
	bool m_is_receiving_wallops;

	/** True if user can receive notice messages */
	bool m_is_receiving_notices;

	/** True if the user is invisible on channels */
	bool m_is_invisible;

	/** True if user is away */
	bool m_is_away;

	/** Away message */
	std::string m_away_message;

	/** Current count of joined channels */
	int m_nb_join;

	/** count limit of joinable channels */
	int m_nb_join_limit;

	/** List of joined channels */
	std::set<boost::shared_ptr<Channel_info> > m_channels_joined;

public:
	/**
	 * Instantiate a new User_info object
	 *
	 * @param configuration Default user informations configuration
	 */
	explicit User_info(const Configuration& configuration);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	~User_info(void);

	/**
	 * Get the nickname of the user
	 *
	 * @return The nickname of the user
	 */
	const std::string& getNickname(void) const;

	/**
	 * Set the nickname of the user
	 *
	 * @param nickname The new nickname of the user
	 */
	void setNickname(const std::string& nickname);

	/**
	 * Get the username of the user
	 *
	 * @return The username of the user
	 */
	const std::string& getUsername(void) const;

	/**
	 * Set the username of the user
	 *
	 * @param username The new username of the user
	 */
	void setUsername(const std::string& username);

	/**
	 * Get the hostname of the user
	 *
	 * @return The hostname of the user
	 */
	const std::string& getHostname(void) const;

	/**
	 * Get the server name of the user
	 *
	 * @return The server name of the user
	 */
	const std::string& getServername(void) const;

	/**
	 * Get the real name of the user
	 *
	 * @return The real name of the user
	 */
	const std::string& getRealname(void) const;

	/**
	 * Set the real name of the user
	 *
	 * @param realname The new real name of the user
	 */
	void setRealname(const std::string& realname);

	/**
	 * Get the pre-computed prefix of the user
	 *
	 * @return The pre-computed prefix of the user
	 */
	const std::string& getPrefix(void) const;

	/**
	 * Get the state of the user connection
	 *
	 * @return The state of the user connection
	 */
	unsigned char getState(void) const;

	/**
	 * Set the state of the user connection
	 *
	 * @param state The new state of the user connection
	 */
	void setState(const unsigned char state);

	/**
	 * Get the IRC op status of the user
	 *
	 * @return The IRC op status of the user
	 */
	bool isIrcOp(void) const;

	/**
	 * Set the IRC op status of the user
	 *
	 * @param is_ircop The new IRC op status of the user
	 */
	void setIrcOp(const bool is_ircop);

	/**
	 * Get the "wallops receive" status of the user
	 *
	 * @return The "wallops receive" status of the user
	 */
	bool isReceivingWallops(void) const;

	/**
	 * Set the "wallops receive" status of the user
	 *
	 * @param is_receiving_wallops The new "wallops receive" status of the user
	 */
	void setReceivingWallops(const bool is_receiving_wallops);

	/**
	 * Get the "notices receive" status of the user
	 *
	 * @return The "notices receive" status of the user
	 */
	bool isReceivingNotices(void) const;

	/**
	 * Set the "notices receive" status of the user
	 *
	 * @param is_receiving_notices The new "notices receive" status of the user
	 */
	void setReceivingNotices(const bool is_receiving_notices);

	/**
	 * Check if the user is invisible on channels
	 *
	 * @return True if the user is invisible on channels, false otherwise
	 */
	bool isInvisible(void) const;

	/**
	 * Set if the user is invisible on channels
	 *
	 * @param is_invisible Set to true if the user is invisible on channels, false otherwise
	 */
	void setInvisible(const bool is_invisible);

	/**
	 * Check if the user is away
	 *
	 * @return True if the user is away
	 */
	bool isAway(void) const;

	/**
	 * Set if the user is away
	 *
	 * @param is_away Set to true if the user is away, false otherwise
	 */
	void setAway(const bool is_away);

	/**
	 * Get the AWAY message of the user
	 *
	 * @return The AWAY message of the user
	 */
	const std::string& getAwayMsg(void) const;

	/**
	 * Set the AWAY message of the user
	 *
	 * @param away_msg The new AWAY message of the user
	 */
	void setAwayMsg(const std::string& away_msg);

	/**
	 * Get the number of channels joined by the user
	 *
	 * @return The number of channels joined by the user
	 */
	int getJoinCount(void) const;

	/**
	 * Get the maximum number of channels joinable by the user
	 *
	 * @return The maximum number of channels joinable by the user
	 */
	int getJoinCoutLimit(void) const;

	/**
	 * Add a channel to the user's join history
	 *
	 * @param channel_ptr The pointer to the channel to add
	 */
	void addJoin(boost::shared_ptr<Channel_info> channel_ptr);

	/**
	 * Remove a channel from the user's join history
	 *
	 * @param channel_ptr The pointer to the channel to remove
	 */
	void removeJoin(boost::shared_ptr<Channel_info> channel_ptr);

	/**
	 * Check if the user as join the specified channel
	 *
	 * @param channel_ptr The pointer to the target channel
	 */
	bool asJoin(boost::shared_ptr<Channel_info> channel_ptr) const;

	/**
	 * Generate an unique nickname (anonymous nickname)
	 *
	 * @return The generated nickname
	 */
	static std::string generate_nickname(void);
};

}

#endif /* USER_INFO_H_ */
