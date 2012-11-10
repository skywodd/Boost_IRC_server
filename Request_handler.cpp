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
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/bind.hpp>
#include "Request_handler.hpp"
#include "Connection.hpp"
#include "Configuration.hpp"
#include "Request_parser.hpp"
#include "Channels_manager.hpp"
#include "Users_manager.hpp"
#include "Channel_info.hpp"
#include "Reply_generator.hpp"
#include "Sanity_check.hpp"
#include "Debug_log.hpp"
#include "Server.hpp"

irc::Request_handler::Request_handler(Connection* parent) :
		m_parent(parent), m_argc(0), m_argv(), m_reply() {
}

irc::Request_handler::~Request_handler(void) {
}

void irc::Request_handler::handle_request(void) {

	/* Parse incoming request */
	Request_parser parser(m_parent->getBuffer());
	if (parser.parse()) { /* If parse success */

		/* Process command */
		std::string command = parser.getCommand();
		m_argv = parser.getArguments();
		m_argc = parser.getArgumentsCount();
		debug::DEBUG_LOG("Parser", "Command", command);
		debug::DEBUG_LOG("Parser", "Argc", m_argc);
		debug::DEBUG_LOG("Parser", "argv", "");
		std::copy(m_argv.begin(), m_argv.end(),
				std::ostream_iterator<std::string>(std::clog, " "));

		/* Handle the command */
		if (command == "PASS") {
			handle_PASS();
		} else if (command == "NICK") {
			handle_NICK();
		} else if (command == "USER") {
			handle_USER();
		} else if (command == "SERVER") {
			handle_SERVER();
		} else if (command == "OPER") {
			handle_OPER();
		} else if (command == "QUIT") {
			handle_QUIT();
		} else if (command == "SQUIT") {
			handle_SQUIT();
		} else if (command == "JOIN") {
			handle_JOIN();
		} else if (command == "PART") {
			handle_PART();
		} else if (command == "MODE") {
			handle_MODE();
		} else if (command == "TOPIC") {
			handle_TOPIC();
		} else if (command == "NAMES") {
			handle_NAMES();
		} else if (command == "LIST") {
			handle_LIST();
		} else if (command == "INVITE") {
			handle_INVITE();
		} else if (command == "KICK") {
			handle_KICK();
		} else if (command == "VERSION") {
			handle_VERSION();
		} else if (command == "STATS") {
			handle_STATS();
		} else if (command == "LINKS") {
			handle_LINKS();
		} else if (command == "TIME") {
			handle_TIME();
		} else if (command == "CONNECT") {
			handle_CONNECT();
		} else if (command == "TRACE") {
			handle_TRACE();
		} else if (command == "ADMIN") {
			handle_ADMIN();
		} else if (command == "INFO") {
			handle_INFO();
		} else if (command == "WHO") {
			handle_WHO();
		} else if (command == "WHOIS") {
			handle_WHOIS();
		} else if (command == "WHOAS") {
			handle_WHOAS();
		} else if (command == "NOTICE") {
			m_parent->updateIdleTime(); /* Update IDLE time */
			handle_NOTICE();
		} else if (command == "PRIVMSG") {
			m_parent->updateIdleTime(); /* Update IDLE time */
			handle_PRIVMSG();
		} else if (command == "KILL") {
			handle_KILL();
		} else if (command == "PING") {
			handle_PING();
		} else if (command == "PONG") {
			handle_PONG();
		} else if (command == "ERROR") {
			handle_ERROR();
		} else if (command == "AWAY") {
			handle_AWAY();
		} else if (command == "REHASH") {
			handle_REHASH();
		} else if (command == "RESTART") {
			handle_RESTART();
		} else if (command == "SUMMON") {
			handle_SUMMON();
		} else if (command == "USERS") {
			handle_USERS();
		} else if (command == "WALLOPS") {
			handle_WALLOPS();
		} else if (command == "USERHOST") {
			handle_USERHOST();
		} else if (command == "ISON") {
			handle_ISON();
		} else { /* Unknown command */
			debug::DEBUG_LOG(m_parent->getNickname(), "Unknown IRC command",
					command);

			/* Send answer */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_UNKNOWNCOMMAND(command);
			m_parent->write(m_reply.toString());
		}

		/* Clean reply generator */
		m_reply.flush();

	} else { /* Invalid command */
		debug::DEBUG_LOG(m_parent->getNickname(), "Invalid IRC request",
				parser.getRaw());
	}
}

void irc::Request_handler::handle_PASS(void) {

	/* Check status */
	if (m_parent->getState() != Connection::WAIT_FOR_PASS) {

		/* Already registered */
		debug::DEBUG_LOG(m_parent->getNickname(), "Already registered");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_ALREADYREGISTRED();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 1) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("PASS");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process PASS */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	process_PASS(m_argv[0]);
}

void irc::Request_handler::process_PASS(const std::string& password) {

	/* Check password */
	if (Server::getInstance()->getConfiguration().server_password.count(
			password)) {

		/* Password match */
		m_parent->setState(Connection::WAIT_FOR_USER);
		debug::DEBUG_LOG(m_parent->getNickname(), "Password match");

	} else {

		/* Password mismatch */
		debug::DEBUG_LOG(m_parent->getNickname(), "Password mismatch");
	}
}

void irc::Request_handler::handle_NICK(void) {

	/* Check state */
	if (m_parent->getState() == Connection::WAIT_FOR_PASS) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 1 && m_argc != 2) {

		/* No nickname given */
		debug::DEBUG_LOG(m_parent->getNickname(), "No nickname given");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NONICKNAMEGIVEN();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	process_NICK(m_argv[0]);
}

void irc::Request_handler::process_NICK(const std::string& nickname) {

	/* Check nickname validity */
	if (Sanity_check::is_valid_nickname(nickname) == false) {

		/* Bad formated nickname */
		debug::DEBUG_LOG(m_parent->getNickname(), "Erroneous nickname");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_ERRONEUSNICKNAME(nickname);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check if nickname already exist */
	if (Server::getInstance()->getUsersDatabase().access(nickname)) {

		/* Nickname already exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "Nickname in use");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NICKNAMEINUSE(nickname);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* TODO No nickname collision check (ERR_NICKCOLLISION) */

	/* Store nickname */
	debug::DEBUG_LOG(m_parent->getNickname(), "Nick change", nickname);
	m_parent->setNickname(nickname);

	/* Broadcast nick change if necessary */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Notice users on joined channels of nickname change */
		debug::DEBUG_LOG(m_parent->getNickname(),
				"Nick change broadcasting ...");
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_NICK(nickname);
		m_parent->writeToChannels(m_reply.toString());
	}
}

void irc::Request_handler::handle_USER(void) {

	/* Check state */
	if (m_parent->getState() != Connection::WAIT_FOR_USER) {

		/* Already registered */
		debug::DEBUG_LOG(m_parent->getNickname(), "Already registered");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_ALREADYREGISTRED();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 4) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("USER");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* We don't use ident protocol, fake answer */
	debug::DEBUG_LOG(m_parent->getNickname(), "Faking ident answer ...");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_NOTICE("AUTH", "*** Looking up your hostname...");
	m_parent->write(m_reply.toString());
	m_reply.flush();
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_NOTICE("AUTH", "** Checking ident...");
	m_parent->write(m_reply.toString());
	m_reply.flush();
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_NOTICE("AUTH", "*** Found your hostname");
	m_parent->write(m_reply.toString());
	m_reply.flush();
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_NOTICE("AUTH",
			"*** No ident response; username prefixed with ~");
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	process_USER(m_argv[0], m_argv[3]);

	/* Send welcome message */
	debug::DEBUG_LOG(m_parent->getNickname(), "Sending welcome message ...");
	send_welcome_msg();
}

void irc::Request_handler::process_USER(const std::string& username,
		const std::string& realname) {

	/* Store user informations */
	m_parent->setUsername("~" + username);
	/* host name and server name are already known */
	m_parent->setRealname(realname);

	/* Update state */
	m_parent->setState(Connection::READY_FOR_MSG);
}

void irc::Request_handler::handle_SERVER(void) {

	/* Not implemented in this program */
	debug::DEBUG_LOG(m_parent->getNickname(), "Not implemented, static answer");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.ERR_ALREADYREGISTRED();
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_OPER(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("OPER");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Ignored : ERR_NOOPERHOST */

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	process_OPER(m_argv[0], m_argv[1]);
}

void irc::Request_handler::process_OPER(const std::string& username,
		const std::string& password) {

	/* Find user name */
	std::map<std::string, std::string>::const_iterator ircop =
			Server::getInstance()->getConfiguration().server_ircop.find(
					username);

	/* Check user name */
	if (ircop == Server::getInstance()->getConfiguration().server_ircop.end()) {

		/* Wrong user name */
		debug::DEBUG_LOG(m_parent->getNickname(), "Wrong user name");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_PASSWDMISMATCH();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check password */
	if ((*ircop).second == password) {

		/* Well done, your oper now ! */
		m_parent->setIrcOp(true);

		/* Notice user of +oper */
		debug::DEBUG_LOG(m_parent->getNickname(), "User is oper");
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.RPL_YOUREOPER();
		m_parent->write(m_reply.toString());

	} else {

		/* Wrong password */
		debug::DEBUG_LOG(m_parent->getNickname(), "Password mismatch");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_PASSWDMISMATCH();
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_QUIT(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Close the connection */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	if (m_argc == 1) {

		/* With custom quit message */
		debug::DEBUG_LOG(m_parent->getNickname(), "QUIT with custom message");
		m_parent->close_because(m_argv[0]);

	} else {

		/* With default quit message */
		debug::DEBUG_LOG(m_parent->getNickname(), "QUIT with default message");
		m_parent->close_because(m_parent->getNickname());
	}
}

void irc::Request_handler::handle_SQUIT(void) {

	/* Not implemented in this program */
	debug::DEBUG_LOG(m_parent->getNickname(), "Not implemented, static answer");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.ERR_NOPRIVILEGES();
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_JOIN(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 1 && m_argc != 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("JOIN");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	// TODO multi args support
	process_JOIN(m_argv[0], m_argv[1]);
}

void irc::Request_handler::process_JOIN(const std::string& channel,
		const std::string& key) {

	/* Check for join count limit */
	if (m_parent->getJoinCount() == m_parent->getJoinCoutLimit()) {

		/* Limit reached */
		debug::DEBUG_LOG(m_parent->getNickname(), "Join limit reached");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_TOOMANYCHANNELS(channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search channel */
	debug::DEBUG_LOG(m_parent->getNickname(), "JOIN", channel);
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* True if a new channel have been created */
	bool new_chan_created = false;

	/* Check if channel exist */
	if (channel_ptr) {

		/* Check if user is banned */
		if (channel_ptr->isBanned(m_parent->getPrefix())) {

			/* You're banned ... */
			debug::DEBUG_LOG(m_parent->getNickname(), "User banned");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_BANNEDFROMCHAN(channel);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}

		/* Check if channel is protected */
		if (channel_ptr->isPrivate()) {

			/* Check arguments count */
			if (m_argc != 2) {

				/* Need more params */
				debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_NEEDMOREPARAMS("JOIN");
				m_parent->write(m_reply.toString());

				/* Abort command */
				return;
			}

			/* Check password */
			if (channel_ptr->getPassword() != key) {

				/* Password don't match */
				debug::DEBUG_LOG(m_parent->getNickname(), "Key mismatch");
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_BADCHANNELKEY(channel);
				m_parent->write(m_reply.toString());

				/* Abort command */
				return;
			}
		}

		/* Check if channel is on invitation */
		if (channel_ptr->isInviteOnly()) {

			/* Check if user is invited to join the channel */
			if (channel_ptr->isInvited(m_parent->shared_from_this()) == false) {

				/* User is not invited to join the channel */
				debug::DEBUG_LOG(m_parent->getNickname(), "Not invited");
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_INVITEONLYCHAN(channel);
				m_parent->write(m_reply.toString());

				/* Abort command */
				return;
			}
		}

		/* Check if channel is full */
		if (channel_ptr->getUsersCount() == channel_ptr->getUsersLimit()) {

			/* No more free place */
			debug::DEBUG_LOG(m_parent->getNickname(), "No more place");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_CHANNELISFULL(channel);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}

	} else { /* Channel don't exist */

		/* Check channel name validity */
		if (Sanity_check::is_valid_channame(channel) == false) {

			/* Channel name is crap */
			debug::DEBUG_LOG(m_parent->getNickname(), "Bad channel name");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHCHANNEL(channel); // or ERR_BADCHANMASK ?
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}

		/* Create channel */
		channel_ptr = Server::getInstance()->getChannelsDatabase().add(channel);

		/* Turn on flag */
		new_chan_created = true;
	}

	/* Add user to channel (channel and user side) */
	channel_ptr->addJoin(m_parent->shared_from_this(),
			new_chan_created || m_parent->isIrcOp());
	m_parent->addJoin(channel_ptr);

	/* Notice users on channel of JOIN */
	debug::DEBUG_LOG(m_parent->getNickname(), "Broadcast JOIN ...");
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_JOIN(channel, key);
	channel_ptr->writeToAll(m_reply.toString());

	/* Send back to user TOPIC and NAMES*/
	process_TOPIC(channel, ""); // Read mode
	process_NAMES(channel);
}

void irc::Request_handler::handle_PART(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 1) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("PART");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	Arguments_Parser args_parser(m_argv[0]);
	std::vector<std::string> args = args_parser.getArguments();
	std::for_each(args.begin(), args.end(),
			boost::bind(&Request_handler::process_PART, this, _1));
}

void irc::Request_handler::process_PART(const std::string& channel) {

	/* Search channel */
	debug::DEBUG_LOG(m_parent->getNickname(), "PART", channel);
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if channel exist */
	if (!channel_ptr) {

		/* Channel don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check if user as join the channel */
	if (m_parent->asJoin(channel_ptr) == false) {

		/* User as not join the channel before PART */
		debug::DEBUG_LOG(m_parent->getNickname(), "Not on channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTONCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Remove user from channel (client and channel side) */
	debug::DEBUG_LOG(m_parent->getNickname(), "Remove join ...");
	m_parent->removeJoin(channel_ptr);
	channel_ptr->removeJoin(m_parent->shared_from_this());

	/* Check if channel is empty */
	if (channel_ptr->getUsersCount() == 0) {

		/* Remove channel from database */
		debug::DEBUG_LOG(m_parent->getNickname(), "Remove channel ...");
		Server::getInstance()->getChannelsDatabase().remove(channel);

	} else { /* Channel is not empty */

		/* Notice users on channel of client PART */
		debug::DEBUG_LOG(m_parent->getNickname(), "Broadcast PART ...");
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_PART(channel);
		channel_ptr->writeToAll(m_reply.toString());
	}
}

void irc::Request_handler::handle_MODE(void) {
	// TODO
}

void irc::Request_handler::process_MODE(const std::string& channel,
		const std::string& mode, const std::string& limit,
		const std::string& user, const std::string& banmask) {
	// TODO
}

void irc::Request_handler::process_MODE(const std::string& nickname,
		const std::string& mode) {
	// TODO
}

void irc::Request_handler::handle_TOPIC(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 1 && m_argc != 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("TOPIC");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	process_TOPIC(m_argv[0], (m_argc == 2) ? m_argv[1] : "");
}

void irc::Request_handler::process_TOPIC(const std::string& channel,
		const std::string& topic) {

	/* Search channel */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if channel exist */
	if (!channel_ptr) {

		/* Channel don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check arguments */
	if (topic == "") { /* Read topic */

		/* Check if topic is set */
		if (channel_ptr->getTopic() != "") {

			/* Send topic */
			debug::DEBUG_LOG(m_parent->getNickname(), "Topic reply");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.RPL_TOPIC(channel, channel_ptr->getTopic());
			m_parent->write(m_reply.toString());

		} else {

			/* Topic is not set */
			debug::DEBUG_LOG(m_parent->getNickname(), "No topic reply");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.RPL_NOTOPIC(channel);
			m_parent->write(m_reply.toString());
		}

	} else { /* Set topic */

		/* Check if user is on the channel */
		if (m_parent->asJoin(channel_ptr) == false) {

			/* Not on the channel */
			debug::DEBUG_LOG(m_parent->getNickname(), "Not on channel");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOTONCHANNEL(channel);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}

		/* If topic can be set by non-ops */
		if (channel_ptr->isTopicSetByOpOnly()) {

			/* Check if user is ops */
			if (channel_ptr->access(m_parent->shared_from_this()).isOp()
					== false) {

				/* Don't have auth to change topic */
				debug::DEBUG_LOG(m_parent->getNickname(),
						"Channel privs needed");
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_CHANOPRIVSNEEDED(channel);
				m_parent->write(m_reply.toString());

				/* Abort command */
				return;
			}
		}

		/* Change topic */
		debug::DEBUG_LOG(m_parent->getNickname(), "Change topic");
		channel_ptr->setTopic(topic);

		/* Notice users on channels of topic change */
		debug::DEBUG_LOG(m_parent->getNickname(), "Broadcast topic change ...");
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_TOPIC(channel, topic);
		channel_ptr->writeToAll(m_reply.toString());
	}
}

void irc::Request_handler::handle_NAMES(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	if (m_argc > 0) { /* Channels names specified */

		Arguments_Parser args_parser(m_argv[0]);
		std::vector<std::string> args = args_parser.getArguments();
		std::for_each(args.begin(), args.end(),
				boost::bind(&Request_handler::process_NAMES, this, _1));

	} else { /* Process all channels */

		// TODO
	}
}

void irc::Request_handler::process_NAMES(const std::string& channel) {

	/* Search channel */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if channel exist */
	if (channel_ptr) {

		/* Check if channel is visible by the user */
		if ((channel_ptr->isPrivate() || channel_ptr->isSecret())
				&& channel_ptr->asJoin(m_parent->shared_from_this()) == false) {

			/* Abort command */
			return;
		}

		/* Process each users on channel */
		std::map<boost::shared_ptr<Connection>, Channel_info::Channel_user_info>::iterator i =
				channel_ptr->getProcessingIterator();
		std::map<boost::shared_ptr<Connection>, Channel_info::Channel_user_info>::iterator end =
				channel_ptr->getEndingIterator();
		for (; i != end; ++i) {

			/* Check if user is invisible */
			if ((*i).first->isInvisible() == false) {

				/* Send NAMES line */
				m_reply.addPrefix(m_parent->getServername());
				m_reply.RPL_NAMREPLY(channel, (*i).second.isOp(),
						(*i).second.canSpeak(), (*i).first->getNickname());
				m_parent->write(m_reply.toString());
			}
		}

		/* Closure /NAMES list */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_ENDOFNAMES(channel);
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_LIST(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc == 2) {

		/* Check target server name */
		if (m_argv[1] != m_parent->getServername()) {

			/* No server-to-server communications */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[1]);
			m_parent->write(m_reply.toString());
		}
	}

	/* Start /LIST list */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_LISTSTART();
	m_parent->write(m_reply.toString());

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	if (m_argc > 0) { /* Channels names specified */

		Arguments_Parser args_parser(m_argv[0]);
		std::vector<std::string> args = args_parser.getArguments();
		std::for_each(args.begin(), args.end(),
				boost::bind(&Request_handler::process_LIST, this, _1));

	} else { /* Process all channels */

		// TODO
	}

	/* Closure /LIST list */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_LISTEND();
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::process_LIST(const std::string& channel) {

	/* Search channel */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if channel exist */
	if (channel_ptr) {

		/* Check if user is not the channel */
		if (channel_ptr->asJoin(m_parent->shared_from_this()) == false) {

			/* Check if channel is secret */
			if (channel_ptr->isSecret()) {

				/* Abort command */
				return;
			}

			/* Check if channel is private */
			if (channel_ptr->isPrivate()) {

				/* Process channel */
				m_reply.addPrefix(m_parent->getServername());
				m_reply.RPL_LIST(channel, 0, "Prv");
				m_parent->write(m_reply.toString());

				/* Abort command */
				return;
			}
		}

		/* Process channel */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LIST(channel, channel_ptr->getUsersCount(),
				channel_ptr->getTopic());
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_INVITE(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("INVITE");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	process_INVITE(m_argv[0], m_argv[1]);
}

void irc::Request_handler::process_INVITE(const std::string& nickname,
		const std::string& channel) {

	/* Search user */
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(nickname);

	/* Check if user exist */
	if (!user) {

		/* User don't exist */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHNICK(nickname);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search channel */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if channel exist */
	if (!channel_ptr) {

		/* Abort command */
		return;
	}

	/* Check if user is on the channel */
	if (m_parent->asJoin(channel_ptr) == false) {

		/* User not on channel */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTONCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check if remote user is already on the channel */
	if (m_parent->asJoin(channel_ptr)) {

		/* User already on channel */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_USERONCHANNEL(nickname, channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check if user is ops on the channel */
	if (channel_ptr->access(m_parent->shared_from_this()).isOp() == false) {

		/* Channel privs needed  */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_CHANOPRIVSNEEDED(channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Invite user */
	channel_ptr->addInvitation(user);

	/* Notice remote user */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_INVITE(nickname, channel);
	user->write(m_reply.toString());

	/* Back-notice user */
	m_reply.flush();
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_INVITING(channel, nickname);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_KICK(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 2 && m_argc != 3) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("KICK");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	process_KICK(m_argv[0], m_argv[1], (m_argc == 3) ? m_argv[2] : "");
}

void irc::Request_handler::process_KICK(const std::string& channel,
		const std::string& nickname, const std::string& comment) {

	/* Search channel */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if channel exist */
	if (!channel_ptr) {

		/* Channel don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check is user is on the channel */
	if (m_parent->asJoin(channel_ptr) == false) {

		/* User not on channel */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTONCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check is user is ops on channel */
	if (channel_ptr->access(m_parent->shared_from_this()).isOp() == false) {

		/* User is not ops on channel */
		debug::DEBUG_LOG(m_parent->getNickname(), "Channel privs needed");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_CHANOPRIVSNEEDED(channel);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search user */
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(nickname);

	/* Check is target user exist and is on the channel */
	if (!user || user->asJoin(channel_ptr) == false) {

		/* Abort command */
		return;
	}

	/* Remove user from channel (client and channel side) */
	debug::DEBUG_LOG(m_parent->getNickname(), "Kick user");
	user->removeJoin(channel_ptr);
	channel_ptr->removeJoin(user);

	/* Notice users on channel of client KICK */
	debug::DEBUG_LOG(m_parent->getNickname(), "Broadcast kick ...");
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_KICK(channel, nickname, comment);
	channel_ptr->writeToAll(m_reply.toString());
}

void irc::Request_handler::handle_VERSION(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check argument count */
	if (m_argc == 1) {

		/* Check target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	debug::DEBUG_LOG(m_parent->getNickname(), "Version reply");
	m_reply.addPrefix(m_parent->getServername());
#ifdef DEBUG
	m_reply.RPL_VERSION("1", "1", m_parent->getServername(),
			"SkyIRC by SkyWodd");
#else
	m_reply.RPL_VERSION("1", "0", m_parent->getServername(), "SkyIRC by SkyWodd");
#endif
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_STATS(void) {
// TODO
}

void irc::Request_handler::process_STATS(const std::string& query) {
// TODO
}

void irc::Request_handler::handle_LINKS(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check argument count */
	if (m_argc == 2) {

		/* Check target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}
	}

	/* Not implemented in this program, static answer */
	debug::DEBUG_LOG(m_parent->getNickname(), "Not implemented, static answer");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ENDOFLINKS((m_argc == 1) ? m_argv[0] : "");
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_TIME(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check argument count */
	if (m_argc == 1) {

		/* Check target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	debug::DEBUG_LOG(m_parent->getNickname(), "Time reply");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_TIME(m_parent->getServername(),
			boost::posix_time::to_simple_string(
					Server::getInstance()->runSince()));
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_CONNECT(void) {

	/* Not implemented in this program */
	debug::DEBUG_LOG(m_parent->getNickname(), "Not implemented, static answer");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.ERR_NOPRIVILEGES();
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_TRACE(void) {

	/* Not implemented in this program */
}

void irc::Request_handler::handle_ADMIN(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check argument count */
	if (m_argc == 1) {

		/* Check target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	debug::DEBUG_LOG(m_parent->getNickname(), "Administrator info reply");

	/* Start administrator answer */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ADMINME(m_parent->getServername());
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Send administrator location 1 */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ADMINLOC1(
			Server::getInstance()->getConfiguration().admin_location_1);
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Start administrator location 2 */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ADMINLOC2(
			Server::getInstance()->getConfiguration().admin_location_2);
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Start administrator email */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ADMINMAIL(Server::getInstance()->getConfiguration().admin_mail);
	m_parent->write(m_reply.toString());
	m_reply.flush();
}

void irc::Request_handler::handle_INFO(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check argument count */
	if (m_argc == 1) {

		/* Check target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	debug::DEBUG_LOG(m_parent->getNickname(), "Server info reply");

	/* Process each server informations lines */
	std::vector<std::string>::const_iterator i =
			Server::getInstance()->getConfiguration().server_infos.begin();
	std::vector<std::string>::const_iterator end =
			Server::getInstance()->getConfiguration().server_infos.end();
	for (; i != end; ++i) {

		/* Send info line */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_INFO(*i);
		m_parent->write(m_reply.toString());
		m_reply.flush();
	}

	/* Closure info line lists */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ENDOFINFO();
	m_parent->write(m_reply.toString());
	m_reply.flush();
}

void irc::Request_handler::handle_WHO(void) {
// TODO
}

void irc::Request_handler::process_WHO(const std::string& name, const bool op) {
// TODO
}

void irc::Request_handler::handle_WHOIS(void) {
// TODO
}

void irc::Request_handler::process_WHOIS(const std::string& nickname) {
// TODO
}

void irc::Request_handler::handle_WHOAS(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc == 0) {

		/* No nick given */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NONICKNAMEGIVEN();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;

	}

	/* Not implemented in this program, static answer */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.ERR_WASNOSUCHNICK(m_argv[0]);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_NOTICE(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc == 1) {

		/* No recipient */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NORECIPIENT("NOTICE");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;

	} else if (m_argc != 2) {

		/* No text to send */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTEXTTOSEND();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	Arguments_Parser args_parser(m_argv[0]);
	std::vector<std::string> args = args_parser.getArguments();
	std::for_each(args.begin(), args.end(),
			boost::bind(&Request_handler::process_NOTICE, this, _1, m_argv[1]));
}

void irc::Request_handler::process_NOTICE(const std::string& nickname,
		const std::string& message) {

	/* Search user */
	debug::DEBUG_LOG(m_parent->getNickname(), "Notice", nickname);
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(nickname);

	/* Check if user exist */
	if (!user) {

		/* User don't exist */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHNICK(nickname);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Send message to remote user */
	if (user->isReceivingNotices()) {

		/* Craft and send NOTICE */
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_NOTICE(nickname, message);
		if (user != m_parent->shared_from_this()) // Send-back echo
			m_parent->write(m_reply.toString());
		user->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_PRIVMSG(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc == 1) {

		/* No recipient */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NORECIPIENT("PRIVMSG");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;

	} else if (m_argc != 2) {

		/* No text to send */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTEXTTOSEND();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check for target limit */
	if (m_argc > 5) {

		/* No many targets */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_TOOMANYTARGETS(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	Arguments_Parser args_parser(m_argv[0]);
	std::vector<std::string> args = args_parser.getArguments();
	std::for_each(args.begin(), args.end(),
			boost::bind(&Request_handler::process_PRIVMSG, this, _1,
					m_argv[1]));
}

void irc::Request_handler::process_PRIVMSG(const std::string& receiver,
		const std::string& message) {

	/* Search user */
	debug::DEBUG_LOG(m_parent->getNickname(), "Privmsg", receiver);
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(receiver);

	/* Check if user exist */
	if (user) {

		/* Send message to remote user */
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_PRIVMSG(receiver, message);
		user->write(m_reply.toString());

		/* Send-back away message if enable */
		if (user->isAway()) {

			/* Send-back away message */
			m_reply.flush();
			m_reply.addPrefix(user->getPrefix());
			m_reply.CMD_PRIVMSG(receiver, user->getAwayMsg());
			m_parent->write(m_reply.toString());
		}

	} else { /* Switch to channel mode */

		/* Search channel */
		boost::shared_ptr<Channel_info> channel =
				Server::getInstance()->getChannelsDatabase().access(receiver);

		/* Check if channel exist */
		if (channel) {

			/* Check if user is on the channel */
			if (m_parent->asJoin(channel) == false) {

				/* Check if channel allow outside message */
				if (channel->isNoOutsideMsg() == false) {

					/* Cannot send to channel */
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_CANNOTSENDTOCHAN(receiver);
					m_parent->write(m_reply.toString());

					/* Abort command */
					return;
				}
			}

			/* Check if channel is moderated */
			if (channel->isModerated()) {

				/* Check if user is allowed to speak */
				if (channel->access(m_parent->shared_from_this()).canSpeak()
						== false) {

					/* Cannot send to channel */
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_CANNOTSENDTOCHAN(receiver);
					m_parent->write(m_reply.toString());

					/* Abort command */
					return;
				}
			}

			/* Send message to remote channel */
			m_reply.addPrefix(m_parent->getPrefix());
			m_reply.CMD_PRIVMSG(receiver, message);
			channel->writeToAll(m_reply.toString());

		} else {

			/* User don't exist */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHNICK(receiver);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}
	}
}

void irc::Request_handler::handle_KILL(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("KILL");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	process_KILL(m_argv[0], m_argv[1]);
}

void irc::Request_handler::process_KILL(const std::string& nickname,
		const std::string& comment) {

	/* Ignored : ERR_CANTKILLSERVER */

	/* Check if user is irc ops */
	if (m_parent->isIrcOp() == false) {

		/* No privileges */
		debug::DEBUG_LOG(m_parent->getNickname(), "No privileges");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search user */
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(nickname);

	/* If user is found */
	if (!user) {

		/* User don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such nick");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHNICK(nickname);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Force close with comment */
	debug::DEBUG_LOG(m_parent->getNickname(), "Force close ...");
	user->close_because(comment);

	/* Back-notice ops of KILL success */
	debug::DEBUG_LOG(m_parent->getNickname(), "Back-notice for success");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_NOTICE(user->getNickname(), "User connection killed !");
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_PING(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 1 && m_argc != 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "No origin");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOORIGIN();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check target server */
	if (m_argv[0] == m_parent->getServername()) {

		/* Ping remote server is not implemented in this program */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHSERVER(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Answer with PONG */
	debug::DEBUG_LOG(m_parent->getNickname(), "Pong answer");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_PONG(m_argv[0], (m_argc == 2) ? m_argv[1] : "");
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_PONG(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "No origin");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOORIGIN();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check the target server */
	if (m_argv[0] == m_parent->getServername()) {

		/* Ping remote server is not implemented in this program */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHSERVER(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check the ping daemon */
	if (m_argv[1] == m_parent->getLastPingArg()) {

		/* Restart deadline timer */
		m_parent->restartDeadlineTimer();
	}
}

void irc::Request_handler::handle_ERROR(void) {

	/* Not implemented in this program */
}

void irc::Request_handler::handle_AWAY(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	process_AWAY((m_argc == 1) ? m_argv[0] : "");
}

void irc::Request_handler::process_AWAY(const std::string& message) {

	/* Switch according arguments */
	if (message == "") { /* Unset away message */

		/* Set as non-away */
		m_parent->setAway(false);

		/* Notice user of non-away */
		debug::DEBUG_LOG(m_parent->getNickname(), "No away");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_UNAWAY();
		m_parent->write(m_reply.toString());

	} else { /* Set away message */

		/* Set as away */
		m_parent->setAway(true);
		m_parent->setAwayMsg(message);

		/* Notice user of away */
		debug::DEBUG_LOG(m_parent->getNickname(), "Now away");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_NOWAWAY();
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_REHASH(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check if user is irc ops */
	if (m_parent->isIrcOp() == false) {

		/* No privileges to do what */
		debug::DEBUG_LOG(m_parent->getNickname(), "No privileges");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* TODO Answer but do nothing (no configuration file used yet) */
	debug::DEBUG_LOG(m_parent->getNickname(),
			"Reloading configuration file ...");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_REHASHING("dummy.conf");
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_RESTART(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check if user is irc ops */
	if (m_parent->isIrcOp() == false) {

		/* No privileges to do what */
		debug::DEBUG_LOG(m_parent->getNickname(), "No privileges");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Shutdown the server */
	debug::DEBUG_LOG(m_parent->getNickname(), "Restarting server ...");
	Server::getInstance()->stop();
}

void irc::Request_handler::handle_SUMMON(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* No SUMMON support in this program */
	debug::DEBUG_LOG(m_parent->getNickname(), "No SUMMON support");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.ERR_SUMMONDISABLED();
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_USERS(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* No USERS support in this program */
	debug::DEBUG_LOG(m_parent->getNickname(), "No USERS support");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.ERR_USERSDISABLED();
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_WALLOPS(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 1) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("ERROR");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	process_WALLOPS(m_argv[0]);
}

void irc::Request_handler::process_WALLOPS(const std::string& message) {

	/* Check if user is irc ops */
	if (m_parent->isIrcOp() == false) {

		/* Abort command */
		return;
	}

	/* Send message to all IRC ops connected */
	debug::DEBUG_LOG(m_parent->getNickname(), "Send wallops message ...");
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_WALLOPS(message);
	Server::getInstance()->getUsersDatabase().writeToIrcOp(m_reply.toString());
}

void irc::Request_handler::handle_USERHOST(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc == 0) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("USERSHOST");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check arguments count limit */
	if (m_argc > 5) {

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	std::string users_reply;
	for (int i = 0; i < m_argc; ++i) {

		/* Search nickname */
		boost::shared_ptr<Connection> user =
				Server::getInstance()->getUsersDatabase().access(m_argv[i]);

		/* Check if user exist */
		if (user) {

			/* Add user informations to list */
			debug::DEBUG_LOG(m_parent->getNickname(), "USEROST", m_argv[i]);
			users_reply += m_argv[i] + ((user->isIrcOp()) ? '*' : ' ') + '='
					+ ((user->isAway()) ? '+' : '-') + user->getHostname()
					+ ' ';
		}

	}

	/* Send informations as reply */
	debug::DEBUG_LOG(m_parent->getNickname(), "User host reply");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_USERHOST(users_reply);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_ISON(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc == 0) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("ISON");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Process command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing command ...");
	std::string ison_reply;
	for (int i = 0; i < m_argc; ++i) {

		/* Search nickname */
		boost::shared_ptr<Connection> user =
				Server::getInstance()->getUsersDatabase().access(m_argv[i]);

		/* Check if the user exist in the database */
		if (user) {

			/* Add user informations to list */
			debug::DEBUG_LOG(m_parent->getNickname(), "ISON", m_argv[i]);
			ison_reply += m_argv[i] + ' ';
		}

	}

	/* Send informations as reply */
	debug::DEBUG_LOG(m_parent->getNickname(), "Is on reply");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ISON(ison_reply);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::send_welcome_msg(void) {

	/* Send welcome message */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_CUSTOM("001",
			m_parent->getNickname() + " :Welcome to the "
					+ m_parent->getServername() + " IRC network "
					+ m_parent->getPrefix());
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Send server info message */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_CUSTOM("002",
			m_parent->getNickname() + " :our host is "
					+ m_parent->getServername()
					+ ", running version SkyIRC1.0");
	m_parent->write(m_reply.toString());
	m_reply.flush();
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_CUSTOM("003",
			m_parent->getNickname() + " :This server was created "
					+ boost::posix_time::to_simple_string(
							Server::getInstance()->runSince()));
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Stats section */
	if (Server::getInstance()->getConfiguration().send_stats) {

		/* Send users stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSERCLIENT(
				Server::getInstance()->getUsersDatabase().getUsersCount(),
				Server::getInstance()->getUsersDatabase().getInvisibleUsersCount(),
				1);
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* Send irc ops stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSEROP(
				Server::getInstance()->getUsersDatabase().getIRCopsCount());
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* Send channels stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSERCHANNELS(
				Server::getInstance()->getChannelsDatabase().getChannelsCount());
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* Send local users stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSERME(
				Server::getInstance()->getUsersDatabase().getUsersCount(), 1);
		m_parent->write(m_reply.toString());
	}

	/* MOTD section */
	if (Server::getInstance()->getConfiguration().send_motd) {

		/* Send MOTD */
		send_motd();

	} else {

		/* Send "no MOTD" message */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOMOTD();
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::send_motd(void) {

	/* Open file */
	std::ifstream motd_file(
			Server::getInstance()->getConfiguration().motd_filename.c_str());

	/* Test file open */
	if (motd_file.is_open()) {

		/* Tmp string */
		std::string line;

		/* Send MOTD header*/
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_MOTDSTART(m_parent->getServername());
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* For each text line of MOTD */
		while (motd_file.good()) {

			/* Get line from file */
			std::getline(motd_file, line);

			/* Send line */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.RPL_MOTD(line);
			m_parent->write(m_reply.toString());
			m_reply.flush();
		}

		/* Send MOTD footer */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_ENDOFMOTD();
		m_parent->write(m_reply.toString());

		/* Close file */
		motd_file.close();

	} else { /* File open error */

		/* Send file I/O error message */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_FILEERROR("open",
				Server::getInstance()->getConfiguration().motd_filename);
		m_parent->write(m_reply.toString());
	}
}
