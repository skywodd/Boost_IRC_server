/**
 * @file Request_handler.hpp
 * @brief IRC request parser
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is designed to handle IRC request.\n
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

#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

/* Dependencies */
#include <string>
#include <vector>
#include "Reply_generator.hpp"

/**
 * @namespace irc
 *
 * Namespace regrouping all IRC features of the program.
 */
namespace irc {

/* Forward declaration */
class Connection;

/**
 * @class Request_handler
 */
class Request_handler {
protected:
	/** Parent connection pointer */
	Connection* m_parent;

	/** Number of IRC arguments */
	int m_argc;

	/** List of IRC arguments */
	std::vector<std::string> m_argv;

	/** Reply generator */
	Reply_generator m_reply;

	/* ----- Command callback ----- */
	void handle_PASS(void);
	void process_PASS(const std::string& password);
	void handle_NICK(void);
	void process_NICK(const std::string& nickname);
	void handle_USER(void);
	void process_USER(const std::string& username, const std::string& realname);
	void handle_SERVER(void);
	void handle_OPER(void);
	void process_OPER(const std::string& username, const std::string& password);
	void handle_QUIT(void);
	void process_QUIT(const std::string& message);
	void handle_SQUIT(void);
	void handle_JOIN(void);
	void process_JOIN(const std::string& channel, const std::string& key);
	void handle_PART(void);
	void process_PART(const std::string& channel);
	void handle_MODE(void);
	void process_MODE(const std::string& channel, const std::string& mode,
			const std::string& limit, const std::string& user,
			const std::string& banmask);
	void process_MODE(const std::string& nickname, const std::string& mode);
	void handle_TOPIC(void);
	void process_TOPIC(const std::string& channel, const std::string& topic);
	void handle_NAMES(void);
	void process_NAMES(const std::string& channel);
	void handle_LIST(void);
	void process_LIST(const std::string& channel);
	void handle_INVITE(void);
	void process_INVITE(const std::string& nickname, const std::string& channel);
	void handle_KICK(void);
	void process_KICK(const std::string& channel, const std::string& nickname, const std::string& comment);
	void handle_VERSION(void);
	void handle_STATS(void);
	void process_STATS(const std::string& query);
	void handle_LINKS(void);
	void process_LINKS(const std::string& rsvname, const std::string& svmask);
	void handle_TIME(void);
	void handle_CONNECT(void);
	void handle_TRACE(void);
	void handle_ADMIN(void);
	void handle_INFO(void);
	void handle_PRIVMSG(void);
	void process_PRIVMSG(const std::string& receiver,
			const std::string& message);
	void handle_NOTICE(void);
	void process_NOTICE(const std::string& nickname, const std::string& message);
	void handle_WHO(void);
	void process_WHO(const std::string& name, const std::string& op);
	void handle_WHOIS(void);
	void process_WHOIS(const std::string& nickmask);
	void handle_WHOAS(void);
	void handle_KILL(void);
	void process_KILL(const std::string& nickname, const std::string& comment);
	void handle_PING(void);
	void handle_PONG(void);
	void handle_ERROR(void);
	void process_ERROR(const std::string& emessage);
	void handle_AWAY(void);
	void process_AWAY(const std::string& message);
	void handle_REHASH(void);
	void handle_RESTART(void);
	void handle_SUMMON(void);
	void handle_USERS(void);
	void handle_WALLOPS(void);
	void process_WALLOPS(const std::string& message);
	void handle_USERHOST(void);
	void process_USERHOST(const std::string& nickname);
	void handle_ISON(void);
	void process_ISON(const std::string& nickname);

	/* ----- Special functions ----- */
	void send_welcome_msg(void);
	void send_motd(void);

public:

	/**
	 * Instantiate a new request handler
	 *
	 *  @param connection Parent connection object
	 */
	Request_handler(Connection* parent);

	/**
	 * Default constructor
	 *
	 * @remarks Ready for overload
	 */
	~Request_handler(void);

	/**
	 * Handle incoming request
	 */
	void handle_request(void);

};

}

#endif /* REQUEST_HANDLER_H_ */
