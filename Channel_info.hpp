/**
 * @file Channel_info.hpp
 * @brief Channel informations wrapper
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is designed as a container for channels informations.\n
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

#ifndef CHANNEL_INFO_H_
#define CHANNEL_INFO_H_

/* Dependencies */
#include <map>
#include <set>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

/**
 * @namespace irc
 */
namespace irc {

/* Forward declaration */
class Connection;
class Configuration;

/**
 * @class Channel_info
 */
class Channel_info: public boost::enable_shared_from_this<Channel_info>,
		private boost::noncopyable {
public:
	/**
	 * Informations about an user in a channel
	 *
	 * @class Channel_user_info
	 */
	class Channel_user_info {
	protected:
		/** True if user can speak on the channel */
		bool m_can_speak;

		/** True if the user is op on the channel */
		bool m_is_op;

	public:
		/**
		 * Instantiate a new Channel_user_info object
		 *
		 * @param can_speak True if user can speak on the channel
		 * @param is_op True if the user is op on the channel
		 */
		explicit Channel_user_info(const bool can_speak = true,
				const bool is_op = false);

		/**
		 * Check if user can speak on the channel
		 *
		 * @return True if user can speak on the channel, false otherwise
		 */
		bool canSpeak(void) const;

		/**
		 * Set if user can speak on the channel
		 *
		 * @param can_speak Set to true if the user is op on the channel, false otherwise
		 */
		void setCanSpeak(const bool can_speak);

		/**
		 * Check if the user is op on the channel
		 *
		 * @return True if the user is op on the channel, false otherwise
		 */
		bool isOp(void) const;

		/**
		 * Set if the user is op on the channel
		 *
		 * @param is_op Set to true if the user is op on the channel, false otherwise
		 */
		void setOp(const bool is_op);

	};

protected:
	/** Topic of the channel */
	std::string m_topic;

	/** Password of the channel */
	std::string m_password;

	/** True if the channel is private */
	bool m_private;

	/** True if the channel is secret */
	bool m_secret;

	/** True if the channel is on invitation only */
	bool m_invite_only;

	/** True if the topic can be set by op only */
	bool m_topic_setby_op_only;

	/** True if messages outside from the channel cannot be sent */
	bool m_no_outside_msg;

	/** True if the channel is moderated */
	bool m_moderated;

	/** Maximum number of users on the channel */
	int m_users_limit;

	/** Number of users ont the channel */
	int m_nb_users;

	/** List of banned mask */
	std::set<std::string> m_banmask;

	/** List of trusted invitation to join the channel */
	std::set<boost::shared_ptr<Connection> > m_invitations;

	/** Database of users currently on the channel, with channel-specific informations */
	std::map<boost::shared_ptr<Connection>, Channel_user_info> m_users;

	/**
	 * Instantiate a new channel_info object
	 *
	 * @param configuration Channel configuration
	 */
	explicit Channel_info(const Configuration& configuration);

	/**
	 * Send message to user on channel
	 *
	 * @param user Pointer to the targeted user
	 * @param message Message to send
	 */
	static void send_message(
			std::map<boost::shared_ptr<Connection>, Channel_user_info>::value_type user_pair,
			std::string& message);

public:
	/**
	 * Instantiate a new channel_info object
	 *
	 * @param configuration Channel configuration
	 */
	static boost::shared_ptr<Channel_info> create(
			const Configuration& configuration);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	~Channel_info(void);

	/**
	 * Get the topic of the channel
	 *
	 * @return The topic of the channel
	 */
	const std::string& getTopic(void) const;

	/**
	 * Set the topic of the channel
	 *
	 * @param topic The new topic of the channel
	 */
	void setTopic(const std::string& topic);

	/**
	 * Get the password of the channel
	 *
	 * @return The password of the channel
	 */
	const std::string& getPassword(void) const;

	/**
	 * Set the password of the channel
	 *
	 * @param password The password of the channel
	 */
	void setPassword(const std::string& password);

	/**
	 * Check if the channel is private
	 *
	 * @return True if the channel is private, false otherwise
	 */
	bool isPrivate(void) const;

	/**
	 * Set if the channel is private
	 *
	 * @param is_private Set to true if the channel is private, false otherwise
	 */
	void setPrivate(const bool is_private);

	/**
	 * Check if the channel is secret
	 *
	 * @return True if the channel is secret, false otherwise
	 */
	bool isSecret(void) const;

	/**
	 * Set if the channel is secret
	 *
	 * @param is_secret Set to true if the channel is secret, false otherwise
	 */
	void setSecret(const bool is_secret);

	/**
	 * Check if the channel is on invitation only
	 *
	 * @return True if the channel is on invitation only, false otherwise
	 */
	bool isInviteOnly(void) const;

	/**
	 * Set if the channel is on invitation only
	 *
	 * @param is_invite_only Set to true if the channel is on invitation only, false otherwise
	 */
	void setInviteOnly(const bool is_invite_only);

	/**
	 * Check if the topic can be set by op only
	 *
	 * @return True if the topic can be set by op only, false otherwise
	 */
	bool isTopicSetByOpOnly(void) const;

	/**
	 * Set if the topic can be set by op only
	 *
	 * @param is_topic_setby_op_only Set to true if the topic can be set by op only, false otherwise
	 */
	void setTopicSetByOpOnly(const bool is_topic_setby_op_only);

	/**
	 * Check if message from the outside of the channel can be sent
	 *
	 * @return True if message from the outside of the channel can be sent, false otherwise
	 */
	bool isNoOutsideMsg(void) const;

	/**
	 * Set if message from the outside of the channel can be sent
	 *
	 * @param no_outside_msg Set to true if message from the outside of the channel can be sent, false otherwise
	 */
	void setNoOutsideMsg(const bool no_outside_msg);

	/**
	 * Check if the channel is moderated
	 *
	 * @return True if the channel is moderated, false otherwise
	 */
	bool isModerated(void) const;

	/**
	 * Set if the channel is moderated
	 *
	 * @param is_moderated Set to true if the channel is moderated, false otherwise
	 */
	void setModerated(const bool is_moderated);

	/**
	 * Get the maximum number of users on the channel
	 *
	 * @return The maximum number of users on the channel
	 */
	int getUsersLimit(void) const;

	/**
	 * Set the maximum number of users on the channel
	 *
	 * @param users_limit The maximum number of users on the channel
	 */
	void setUsersLimit(const int users_limit);

	/**
	 * Get the number of users on the channel
	 *
	 * @return The number of users on the channel
	 */
	int getUsersCount(void) const;

	/**
	 * Add user to the channel
	 *
	 * @param user Pointer to the user to add to the channel
	 * @param is_op True if user is op on the channel (false by default)
	 */
	void addJoin(boost::shared_ptr<Connection> user, const bool is_op = false);

	/**
	 * Remove user from the channel
	 *
	 * @param user Pointer to the user to remove from channel
	 */
	void removeJoin(boost::shared_ptr<Connection> user);

	/**
	 * Check if user has join the channel
	 *
	 * @return user Pointer to the user to test
	 */
	bool asJoin(boost::shared_ptr<Connection> user) const;

	/**
	 * Add invitation of user to join the channel
	 *
	 * @param user Pointer to the user to add invitation
	 */
	void addInvitation(boost::shared_ptr<Connection> user);

	/**
	 * Remove invitation of user to join the channel
	 *
	 * @param user Pointer to the user to remove invitation
	 */
	void removeInvitation(boost::shared_ptr<Connection> user);

	/**
	 * Check if user has been invited on this channel
	 *
	 * @return user Pointer to the user to test if invited
	 */
	bool isInvited(boost::shared_ptr<Connection> user) const;

	/**
	 * Add banmask to the blacklist of the channel
	 *
	 * @param banmask Banmask to add to the blacklist
	 */
	void addBanmask(const std::string& banmask);

	/**
	 * Remove banmask from the blacklist of the channel
	 *
	 * @param banmask Banmask to remove from the blacklist
	 */
	void removeBanmask(const std::string& banmask);

	/**
	 * Check if banmask is banned on this channel
	 *
	 * @param banmask Banmask to test against to the blacklist
	 * @return
	 */
	bool isBanned(const std::string& banmask) const;

	/**
	 * Get channel informations about an user
	 *
	 * @param user Pointer to the user who want to retrieve the channel informations
	 * @return Channel informations about the targeted user
	 */
	Channel_user_info& access(boost::shared_ptr<Connection> user);

	/**
	 * Write data to all users on the channel
	 *
	 * @param buffer Data to send to all users on the channel
	 */
	void writeToAll(const std::string& buffer);
};

}

#endif /* CHANNEL_INFO_H_ */
