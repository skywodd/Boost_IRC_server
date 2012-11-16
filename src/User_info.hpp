/**
 * @file User_info.hpp
 * @brief IRC user informations container
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

#ifndef USER_INFO_H_
#define USER_INFO_H_

/* Dependencies */
#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

/**
 * @namespace irc
 *
 * @brief Namespace regrouping all IRC features of the program.
 */
namespace irc {

/* Forward declarations */
class Channel_info;

/**
 * @class User_info
 * @brief IRC user informations container
 *
 * This class is designed as a container for IRC informations about an user.\n
 * This class store ALL informations about the user.\n
 * \n
 * Be aware, this class does not make any communication with other class, it's just like a big structure.\n
 * All informations are available by getter / setter functions (or only getter if read-only).\n
 * \n
 * This class keep a list of joined channels and a counter of joined channels.\n
 * Channels operations MUST be done TWO times, one time at "channel" side and one time at "user" side.\n
 * \n
 * Take a look at class's members documentation for details.\n
 */
class User_info: private boost::noncopyable {
public:
	/**
	 * User connection states
	 *
	 * (the request handler is designed as a states machine)
	 */
	typedef enum {
		REQUESTING_INIT, /*!< Connection need initialization before use */
		WAIT_FOR_PASS, /*!< Waiting for PASS command */
		WAIT_FOR_USER, /*!< Waiting for USER command */
		READY_FOR_MSG, /*!< Ready for speaking over IRC network */
	} User_connection_state;

protected:
	/** User nickname (set by the user using the NICK command) */
	std::string m_nickname;

	/** User name (set by the user using USER command)
	 *
	 * @remarks Read only (need class overloading)
	 */
	std::string m_username;

	/** User host name (reverse lookup of remote end point)
	 *
	 * @remarks Read only (need class overloading)
	 */
	std::string m_hostname;

	/** User server name (IRC relay server used by the user)
	 *
	 * @warning This implementation of IRC server does NOT include server-to-server communications !
	 */
	std::string m_servername;

	/** User real name (set by the user using USER command) */
	std::string m_realname;

	/** Computed prefix (updated at each call of setter function of m_nickname or m_realname) */
	std::string m_prefix;

	/** Connection state (used by the request handler (states machine)) */
	unsigned char m_state;

	/** True if the user is an IRC ops */
	bool m_is_ircop;

	/** True if the user can receive wallops messages */
	bool m_is_receiving_wallops;

	/** True if the user can receive notice messages */
	bool m_is_receiving_notices;

	/** True if the user is invisible on channels */
	bool m_is_invisible;

	/** True if the user is away */
	bool m_is_away;

	/** Away message of the user */
	std::string m_away_message;

	/** List of joined channels */
	std::set<boost::shared_ptr<Channel_info> > m_channels_joined;

public:
	/**
	 * Instantiate a new User_info object
	 */
	explicit User_info(void);

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
	 * Get the user name of the user
	 *
	 * @return The user name of the user
	 */
	const std::string& getUsername(void) const;

	/**
	 * Set the user name of the user
	 *
	 * @param username The new user name of the user
	 */
	void setUsername(const std::string& username);

	/**
	 * Get the host name of the user
	 *
	 * @return The host name of the user
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
	 * Get the computed prefix of the user
	 *
	 * @return The computed prefix of the user
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
	 * Get the "IRC ops" status of the user
	 *
	 * @return The "IRC ops" status of the user
	 */
	bool isIrcOp(void) const;

	/**
	 * Set the "IRC ops" status of the user
	 *
	 * @param is_ircop The new "IRC ops" status of the user
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
	 * @return True if the user is away, false otherwise
	 */
	bool isAway(void) const;

	/**
	 * Set if the user is away
	 *
	 * @param is_away Set to true if the user is away, false otherwise
	 */
	void setAway(const bool is_away);

	/**
	 * Get the current AWAY message of the user
	 *
	 * @return The current AWAY message of the user
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
	 * Get the maximum number of channels to be joined by the user
	 *
	 * @return The maximum number of channels to be joined by the user
	 */
	int getJoinCoutLimit(void) const;

	/**
	 * Add a channel to the user's join history
	 *
	 * @param channel_ptr An intelligent pointer to the channel to add to the user's join history
	 */
	void addJoin(boost::shared_ptr<Channel_info> channel_ptr);

	/**
	 * Remove a channel from the user's join history
	 *
	 * @param channel_ptr An intelligent pointer to the channel to remove from the user's join history
	 */
	void removeJoin(boost::shared_ptr<Channel_info> channel_ptr);

	/**
	 * Check if the user as join the specified channel
	 *
	 * @param channel_ptr An intelligent pointer to the target to test
	 * @return True if the user as join the specified channel, false otherwise
	 */
	bool asJoin(boost::shared_ptr<Channel_info> channel_ptr) const;

	/**
	 * Generate an unique nickname (anonymous nickname)
	 *
	 * @return An unique anonymous nickname
	 */
	static std::string generate_nickname(void);

};

} /* namespace irc */

#endif /* USER_INFO_H_ */
