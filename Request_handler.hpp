/*
 * Request_handler.hpp
 *
 *  Created on: 2 nov. 2012
 *      Author: skywodd
 */

#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

/* Dependencies */
#include <string>
#include <vector>
#include "Reply_generator.hpp"

/**
 * @namespace irc
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

	/* ----- Command callbacks ----- */
	void handle_PASS(void);
	void handle_NICK(void);
	void handle_USER(void);
	void handle_SERVER(void);
	void handle_OPER(void);
	void handle_QUIT(void);
	void handle_SQUIT(void);
	void handle_JOIN(void);
	void handle_PART(void);
	void handle_MODE(void);
	void handle_TOPIC(void);
	void handle_NAMES(void);
	void handle_LIST(void);
	void handle_INVITE(void);
	void handle_KICK(void);
	void handle_VERSION(void);
	void handle_STATS(void);
	void handle_LINKS(void);
	void handle_TIME(void);
	void handle_CONNECT(void);
	void handle_TRACE(void);
	void handle_ADMIN(void);
	void handle_INFO(void);
	void handle_WHO(void);
	void handle_WHOIS(void);
	void handle_WHOAS(void);
	void handle_NOTICE(void);
	void handle_PRIVMSG(void);
	void handle_KILL(void);
	void handle_PING(void);
	void handle_PONG(void);
	void handle_ERROR(void);
	void handle_AWAY(void);
	void handle_REHASH(void);
	void handle_RESTART(void);
	void handle_SUMMON(void);
	void handle_USERS(void);
	void handle_WALLOPS(void);
	void handle_USERHOST(void);
	void handle_ISON(void);

public:

	/**
	 * Intantiate a new request handler
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
