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

	/* Parse the incoming request */
	Request_parser parser(m_parent->getBuffer());
	if (parser.parse()) { /* If parse success */

		/* Process the request */
		std::string command = parser.getCommand();
		m_argv = parser.getArguments();
		m_argc = parser.getArgumentsCount();
		debug::DEBUG_LOG("Parser", "Command", command);
		debug::DEBUG_LOG("Parser", "Argc", m_argc);
		debug::DEBUG_LOG("Parser", "argv", "");
		if (m_argc != 0) {
			std::copy(m_argv.begin(), m_argv.end(),
					std::ostream_iterator<std::string>(std::clog, " "));
			std::clog << std::endl;
		}

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
			m_parent->updateIdleTime(); /* Update the IDLE time */
			handle_NOTICE();
		} else if (command == "PRIVMSG") {
			m_parent->updateIdleTime(); /* Update the IDLE time */
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

			/* Send the ERR_UNKNOWNCOMMAND answer */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_UNKNOWNCOMMAND(command);
			m_parent->write(m_reply.toString());
		}

		/* Clean the reply generator */
		m_reply.flush();

	} else { /* Invalid command format received */
		debug::DEBUG_LOG(m_parent->getNickname(), "Invalid IRC request",
				parser.getRaw());
	}
}

void irc::Request_handler::handle_PASS(void) {

	/* Check the user status */
	if (m_parent->getState() != Connection::WAIT_FOR_PASS) {

		/* The user is already registered */
		debug::DEBUG_LOG(m_parent->getNickname(), "Already registered");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_ALREADYREGISTRED();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* Need more parameters */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("PASS");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the PASS command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing PASS command ...");
	process_PASS(m_argv[0]);
}

void irc::Request_handler::process_PASS(const std::string& password) {

	/* Check the password */
	if (Server::getInstance()->getConfiguration().server_password.count(
			password)) {

		/* Password match */
		debug::DEBUG_LOG(m_parent->getNickname(), "Password match");
		m_parent->setState(Connection::WAIT_FOR_USER);

	} else {

		/* Password mismatch */
		debug::DEBUG_LOG(m_parent->getNickname(), "Password mismatch");
	}
}

void irc::Request_handler::handle_NICK(void) {

	/* Check the user state */
	if (m_parent->getState() == Connection::WAIT_FOR_PASS) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* No nickname given */
		debug::DEBUG_LOG(m_parent->getNickname(), "No nickname given");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NONICKNAMEGIVEN();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing NICK command ...");
	process_NICK(m_argv[0]);
}

void irc::Request_handler::process_NICK(const std::string& nickname) {

	/* Check the nickname validity */
	if (Sanity_check::is_valid_nickname(nickname) == false) {

		/* Erroneous nickname */
		debug::DEBUG_LOG(m_parent->getNickname(), "Erroneous nickname");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_ERRONEUSNICKNAME(nickname);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check if the nickname already exist in the database */
	if (Server::getInstance()->getUsersDatabase().access(nickname)) {

		/* Nickname already exist in the database */
		debug::DEBUG_LOG(m_parent->getNickname(), "Nickname in use");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NICKNAMEINUSE(nickname);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* TODO No nickname collision check (ERR_NICKCOLLISION)
	 * Require : Function to search users by regex (Users_manager)
	 *           Function to transform nickname into collision matching regex (Sanity_check) */

	/* Store the nickname */
	debug::DEBUG_LOG(m_parent->getNickname(), "Nick change", nickname);
	m_parent->setNickname(nickname);

	/* Broadcast the nick change if necessary */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Notice users on joined channels of the nickname change */
		debug::DEBUG_LOG(m_parent->getNickname(),
				"Nick change broadcasting ...");
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_NICK(nickname);
		m_parent->writeToChannels(m_reply.toString());
	}
}

void irc::Request_handler::handle_USER(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::WAIT_FOR_USER) {

		/* Already registered */
		debug::DEBUG_LOG(m_parent->getNickname(), "Already registered");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_ALREADYREGISTRED();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc < 4) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("USER");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* We don't use ident protocol, fake the answer */
	debug::DEBUG_LOG(m_parent->getNickname(), "Faking ident answer ...");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_NOTICE("AUTH", "*** Looking up your hostname...");
	m_parent->write(m_reply.toString());
	m_reply.flush();
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_NOTICE("AUTH", "*** Checking ident...");
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

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing USER command ...");
	process_USER(m_argv[0], m_argv[3]);

	/* Send the welcome message */
	debug::DEBUG_LOG(m_parent->getNickname(), "Sending welcome message ...");
	send_welcome_msg();
}

void irc::Request_handler::process_USER(const std::string& username,
		const std::string& realname) {

	/* Store the user informations */
	m_parent->setUsername("~" + username);
	/* host name and server name are already known */
	m_parent->setRealname(realname);

	/* Update the user state */
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

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc < 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("OPER");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Ignored : ERR_NOOPERHOST */

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing OPER command ...");
	process_OPER(m_argv[0], m_argv[1]);
}

void irc::Request_handler::process_OPER(const std::string& username,
		const std::string& password) {

	/* Search the user name in the IRC ops logins list */
	std::map<std::string, std::string>::const_iterator ircop =
			Server::getInstance()->getConfiguration().server_ircop.find(
					username);

	/* Check if the login exist */
	if (ircop == Server::getInstance()->getConfiguration().server_ircop.end()) {

		/* Wrong user name */
		debug::DEBUG_LOG(m_parent->getNickname(), "Wrong user name");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_PASSWDMISMATCH();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check the password */
	if ((*ircop).second == password) {

		/* Well done, your ops now ! */
		m_parent->setIrcOp(true);

		/* Notice user of ops status */
		debug::DEBUG_LOG(m_parent->getNickname(), "User is IRC ops");
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

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Close the user connection */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing QUIT command ...");
	if (m_argc > 0) {

		/* QUIT with custom message */
		debug::DEBUG_LOG(m_parent->getNickname(), "QUIT with custom message");
		m_parent->close_because(m_argv[0]);

	} else {

		/* QUIT with default message (= nickname) */
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

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("JOIN");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing JOIN command ...");
	// TODO Multiple (crossed) arguments support
	process_JOIN(m_argv[0], (m_argc >= 2) ? m_argv[1] : "");
}

void irc::Request_handler::process_JOIN(const std::string& channel,
		const std::string& key) {

	/* Check the join count limit */
	if (m_parent->getJoinCount() == m_parent->getJoinCoutLimit()) {

		/* Limit reached */
		debug::DEBUG_LOG(m_parent->getNickname(), "Join limit reached");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_TOOMANYCHANNELS(channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Search the channel in the database */
	debug::DEBUG_LOG(m_parent->getNickname(), "JOIN process", channel);
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* True if a new channel have been created for this JOIN */
	bool new_chan_created = false;

	/* Check if the channel already exist */
	if (channel_ptr) {

		/* Check if user has already join the channel */
		if (m_parent->asJoin(channel_ptr)) {

			/* Abort the command */
			return;
		}

		/* Check if the user is banned */
		if (channel_ptr->isBanned(m_parent->getPrefix())) {

			/* You're banned ... */
			debug::DEBUG_LOG(m_parent->getNickname(), "User banned");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_BANNEDFROMCHAN(channel);
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}

		/* Check if the channel is protected */
		if (channel_ptr->isPrivate()) {

			/* Check the key */
			if (key == "" || channel_ptr->getPassword() != key) {

				/* Password mismatch */
				debug::DEBUG_LOG(m_parent->getNickname(), "Key mismatch");
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_BADCHANNELKEY(channel);
				m_parent->write(m_reply.toString());

				/* Abort the command */
				return;
			}
		}

		/* Check if the channel is on invitation only */
		if (channel_ptr->isInviteOnly()) {

			/* Check if the user is invited to join the channel */
			if (channel_ptr->isInvited(m_parent->shared_from_this()) == false) {

				/* User is not invited to join the channel */
				debug::DEBUG_LOG(m_parent->getNickname(), "Not invited");
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_INVITEONLYCHAN(channel);
				m_parent->write(m_reply.toString());

				/* Abort the command */
				return;
			}
		}

		/* Check if the channel is full */
		if (channel_ptr->getUsersCount() == channel_ptr->getUsersLimit()) {

			/* No more place */
			debug::DEBUG_LOG(m_parent->getNickname(), "No more place");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_CHANNELISFULL(channel);
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}

	} else { /* Channel don't exist */

		/* Check the channel name validity */
		if (Sanity_check::is_valid_channame(channel) == false) {

			/* Channel name is crap */
			debug::DEBUG_LOG(m_parent->getNickname(), "Bad channel name");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHCHANNEL(channel); // or ERR_BADCHANMASK ?
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}

		/* Create a new channel */
		channel_ptr = Server::getInstance()->getChannelsDatabase().add(channel);
		new_chan_created = true;
	}

	/* Add the user to the channel (channel and user side) */
	channel_ptr->addJoin(m_parent->shared_from_this(),
			new_chan_created || m_parent->isIrcOp());
	m_parent->addJoin(channel_ptr);

	/* Notice users on channel of JOIN */
	debug::DEBUG_LOG(m_parent->getNickname(), "Broadcast JOIN ...");
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_JOIN(channel, key);
	channel_ptr->writeToAll(m_reply.toString());
	m_reply.flush();

	/* Send back to the user the TOPIC and NAMES list */
	process_TOPIC(channel, ""); // Read-only mode
	m_reply.flush();
	process_NAMES(channel);
}

void irc::Request_handler::handle_PART(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("PART");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing PART command ...");
	Arguments_Parser args_parser(m_argv[0]);
	std::vector<std::string> args = args_parser.getArguments();
	std::for_each(args.begin(), args.end(),
			boost::bind(&Request_handler::process_PART, this, _1));
}

void irc::Request_handler::process_PART(const std::string& channel) {

	/* Search the channel in the database */
	debug::DEBUG_LOG(m_parent->getNickname(), "PART process", channel);
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if the channel exist */
	if (!channel_ptr) {

		/* Channel don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check if the user as join the channel */
	if (m_parent->asJoin(channel_ptr) == false) {

		/* User as not join the channel */
		debug::DEBUG_LOG(m_parent->getNickname(), "Not on channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTONCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Remove the user from the channel (client and channel side) */
	debug::DEBUG_LOG(m_parent->getNickname(), "Remove join ...");
	m_parent->removeJoin(channel_ptr);
	channel_ptr->removeJoin(m_parent->shared_from_this());

	/* Check if the channel is now empty */
	if (channel_ptr->getUsersCount() == 0) {

		/* Remove the channel from the database */
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

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("TOPIC");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing TOPIC command ...");
	process_TOPIC(m_argv[0], (m_argc >= 2) ? m_argv[1] : "");
}

void irc::Request_handler::process_TOPIC(const std::string& channel,
		const std::string& topic) {

	/* Search the channel in the database */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if the channel exist */
	if (!channel_ptr) {

		/* Channel don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check the arguments for get/set mode */
	if (topic == "") { /* Read topic */

		/* Check if the topic is set */
		if (channel_ptr->getTopic() != "") {

			/* Send the topic */
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

	} else { /* Set the topic */

		/* Check if the user is on the channel */
		if (m_parent->asJoin(channel_ptr) == false) {

			/* User not on the channel */
			debug::DEBUG_LOG(m_parent->getNickname(), "Not on channel");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOTONCHANNEL(channel);
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}

		/* Check if the topic can be set by non-ops */
		if (channel_ptr->isTopicSetByOpOnly()) {

			/* Check if the user is channel ops */
			if (channel_ptr->access(m_parent->shared_from_this()).isOp()
					== false) {

				/* User don't have privileges to change the topic */
				debug::DEBUG_LOG(m_parent->getNickname(),
						"Channel privs needed");
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_CHANOPRIVSNEEDED(channel);
				m_parent->write(m_reply.toString());

				/* Abort the command */
				return;
			}
		}

		/* Change the topic */
		debug::DEBUG_LOG(m_parent->getNickname(), "Change topic");
		channel_ptr->setTopic(topic);

		/* Notice users on the channel of topic change */
		debug::DEBUG_LOG(m_parent->getNickname(), "Broadcast topic change ...");
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_TOPIC(channel, topic);
		channel_ptr->writeToAll(m_reply.toString());
	}
}

void irc::Request_handler::handle_NAMES(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing NAMES command ...");
	if (m_argc > 0) { /* Channels names specified */

		Arguments_Parser args_parser(m_argv[0]);
		std::vector<std::string> args = args_parser.getArguments();
		std::for_each(args.begin(), args.end(),
				boost::bind(&Request_handler::process_NAMES, this, _1));

	} else { /* Process all channels */

		// TODO NAMES all channels in the database
	}
}

void irc::Request_handler::process_NAMES(const std::string& channel) {

	/* Search the channel in the database */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if the channel exist */
	if (channel_ptr) {

		/* Check if the channel is visible by the user */
		if ((channel_ptr->isPrivate() || channel_ptr->isSecret())
				&& channel_ptr->asJoin(m_parent->shared_from_this()) == false) {

			/* Abort the command */
			return;
		}

		/* Process each visible users on the channel */
		std::map<boost::shared_ptr<Connection>, Channel_info::Channel_user_info>::iterator i =
				channel_ptr->getProcessingIterator();
		std::map<boost::shared_ptr<Connection>, Channel_info::Channel_user_info>::iterator end =
				channel_ptr->getEndingIterator();
		for (; i != end; ++i) {

			/* Check if the user is invisible */
			if ((*i).first->isInvisible() == false) {

				/* Send the current NAMES line */
				debug::DEBUG_LOG(m_parent->getNickname(), "Send NAMES line");
				m_reply.addPrefix(m_parent->getServername());
				m_reply.RPL_NAMREPLY(channel, (*i).second.isOp(),
						(*i).second.canSpeak(), (*i).first->getPrefix());
				m_parent->write(m_reply.toString());
				m_reply.flush();
			}
		}

		/* Closure the /NAMES list */
		debug::DEBUG_LOG(m_parent->getNickname(), "Closure NAMES list");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_ENDOFNAMES(channel);
		m_parent->write(m_reply.toString());
		m_reply.flush();
	}
}

void irc::Request_handler::handle_LIST(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc >= 2) {

		/* Check the target server name */
		if (m_argv[1] != m_parent->getServername()) {

			/* No server-to-server communications */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[1]);
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}
	}

	/* Start the /LIST list */
	debug::DEBUG_LOG(m_parent->getNickname(), "Start LIST list");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_LISTSTART();
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing LIST command ...");
	if (m_argc > 0) { /* Channels names specified */

		Arguments_Parser args_parser(m_argv[0]);
		std::vector<std::string> args = args_parser.getArguments();
		std::for_each(args.begin(), args.end(),
				boost::bind(&Request_handler::process_LIST, this, _1));

	} else { /* Process all channels */

		// TODO Process all channels in the database
	}

	/* Closure the /LIST list */
	debug::DEBUG_LOG(m_parent->getNickname(), "Closure LIST list");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_LISTEND();
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::process_LIST(const std::string& channel) {

	/* Search the channel in the database */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if the channel exist */
	if (channel_ptr) {

		/* Check if the user is not the channel */
		if (channel_ptr->asJoin(m_parent->shared_from_this()) == false) {

			/* Check if the channel is secret */
			if (channel_ptr->isSecret()) {

				/* Abort the command */
				return;
			}

			/* Check if the channel is private */
			if (channel_ptr->isPrivate()) {

				/* Process the channel as private */
				debug::DEBUG_LOG(m_parent->getNickname(),
						"LIST private channel");
				m_reply.addPrefix(m_parent->getServername());
				m_reply.RPL_LIST(channel, 0, "Prv");
				m_parent->write(m_reply.toString());
				m_reply.flush();

				/* Abort the command */
				return;
			}
		}

		/* Process the channel */
		debug::DEBUG_LOG(m_parent->getNickname(), "LIST channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LIST(channel, channel_ptr->getUsersCount(),
				channel_ptr->getTopic());
		m_parent->write(m_reply.toString());
		m_reply.flush();
	}
}

void irc::Request_handler::handle_INVITE(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc < 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("INVITE");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing INVITE command ...");
	process_INVITE(m_argv[0], m_argv[1]);
}

void irc::Request_handler::process_INVITE(const std::string& nickname,
		const std::string& channel) {

	/* Search the user in the database */
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(nickname);

	/* Check if the user exist */
	if (!user) {

		/* User don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such nick");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHNICK(nickname);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Search the channel in the database */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if the channel exist */
	if (!channel_ptr) {

		/* Abort the command */
		return;
	}

	/* Check if the user is on the channel */
	if (m_parent->asJoin(channel_ptr) == false) {

		/* User is not on channel */
		debug::DEBUG_LOG(m_parent->getNickname(), "User not on channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTONCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check if the remote user is already on the channel */
	if (m_parent->asJoin(channel_ptr)) {

		/* Remote user is already on the channel */
		debug::DEBUG_LOG(m_parent->getNickname(), "User on channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_USERONCHANNEL(nickname, channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check if the user is ops on the channel */
	if (channel_ptr->access(m_parent->shared_from_this()).isOp() == false) {

		/* Channel privileges needed  */
		debug::DEBUG_LOG(m_parent->getNickname(), "Channel privs needed");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_CHANOPRIVSNEEDED(channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Invite the remote user */
	debug::DEBUG_LOG(m_parent->getNickname(), "Inviting user ...");
	channel_ptr->addInvitation(user);

	/* Notice the remote user */
	debug::DEBUG_LOG(m_parent->getNickname(), "Notice remote user ...");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_INVITE(nickname, channel);
	user->write(m_reply.toString());
	m_reply.flush();

	/* Back-notice the local user */
	debug::DEBUG_LOG(m_parent->getNickname(), "Back-notice user ...");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_INVITING(channel, nickname);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_KICK(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc < 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("KICK");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing KICK command ...");
	process_KICK(m_argv[0], m_argv[1], (m_argc >= 3) ? m_argv[2] : "");
}

void irc::Request_handler::process_KICK(const std::string& channel,
		const std::string& nickname, const std::string& comment) {

	/* Search the channel in the database */
	boost::shared_ptr<Channel_info> channel_ptr =
			Server::getInstance()->getChannelsDatabase().access(channel);

	/* Check if the channel exist */
	if (!channel_ptr) {

		/* Channel don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check is the user is on the channel */
	if (m_parent->asJoin(channel_ptr) == false) {

		/* User is not on channel */
		debug::DEBUG_LOG(m_parent->getNickname(), "Not on channel");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTONCHANNEL(channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check if the user is ops on the channel */
	if (channel_ptr->access(m_parent->shared_from_this()).isOp() == false) {

		/* User is not ops on the channel */
		debug::DEBUG_LOG(m_parent->getNickname(), "Channel privs needed");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_CHANOPRIVSNEEDED(channel);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Search the remote user in the database */
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(nickname);

	/* Check if the remote user exist and is on the channel */
	if (!user || user->asJoin(channel_ptr) == false) {

		/* Abort the command */
		return;
	}

	/* Notice the users on the channel of client KICK */
	debug::DEBUG_LOG(m_parent->getNickname(), "Broadcast kick ...");
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_KICK(channel, nickname, comment);
	channel_ptr->writeToAll(m_reply.toString());

	/* Remove user from the channel (client and channel side) */
	debug::DEBUG_LOG(m_parent->getNickname(), "Kick user");
	user->removeJoin(channel_ptr);
	channel_ptr->removeJoin(user);
}

void irc::Request_handler::handle_VERSION(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the argument count */
	if (m_argc >= 1) {

		/* Check the target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing VERSION command ...");
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

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the argument count */
	if (m_argc >= 1) {

		/* Check the target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}
	}

	/* Not implemented in this program, static answer */
	debug::DEBUG_LOG(m_parent->getNickname(), "Not implemented, static answer");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ENDOFLINKS((m_argc >= 1) ? m_argv[0] : "");
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_TIME(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the argument count */
	if (m_argc >= 1) {

		/* Check the target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing TIME command ...");
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

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc >= 1) {

		/* Check the target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing ADMIN command ...");
	debug::DEBUG_LOG(m_parent->getNickname(), "Administrator info reply");

	/* Start the administrator answer */
	debug::DEBUG_LOG(m_parent->getNickname(), "Start ADMIN answer");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ADMINME(m_parent->getServername());
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Send the administrator location 1 */
	debug::DEBUG_LOG(m_parent->getNickname(), "Send administrator location 1");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ADMINLOC1(
			Server::getInstance()->getConfiguration().admin_location_1);
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Send the administrator location 2 */
	debug::DEBUG_LOG(m_parent->getNickname(), "Send administration location 2");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ADMINLOC2(
			Server::getInstance()->getConfiguration().admin_location_2);
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Send the administrator email */
	debug::DEBUG_LOG(m_parent->getNickname(), "Send administration mail");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ADMINMAIL(Server::getInstance()->getConfiguration().admin_mail);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_INFO(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc >= 1) {

		/* Check the target server */
		if (m_argv[0] != m_parent->getServername()) {

			/* No server-to-server communication support */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHSERVER(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort the command */
			return;
		}
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing INFO command ...");
	debug::DEBUG_LOG(m_parent->getNickname(), "Server info reply");

	/* Process each server informations lines */
	std::vector<std::string>::const_iterator i =
			Server::getInstance()->getConfiguration().server_infos.begin();
	std::vector<std::string>::const_iterator end =
			Server::getInstance()->getConfiguration().server_infos.end();
	for (; i != end; ++i) {

		/* Send the current INFO line */
		debug::DEBUG_LOG(m_parent->getNickname(), "Send INFO line");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_INFO(*i);
		m_parent->write(m_reply.toString());
		m_reply.flush();
	}

	/* Closure the /INFO list */
	debug::DEBUG_LOG(m_parent->getNickname(), "Closure INFO list");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ENDOFINFO();
	m_parent->write(m_reply.toString());
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

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* No nick given */
		debug::DEBUG_LOG(m_parent->getNickname(), "No nick given");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NONICKNAMEGIVEN();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Not implemented in this program, static answer */
	debug::DEBUG_LOG(m_parent->getNickname(), "Not implemented, static answer");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.ERR_WASNOSUCHNICK(m_argv[0]);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_NOTICE(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* No recipient */
		debug::DEBUG_LOG(m_parent->getNickname(), "No recipient");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NORECIPIENT("NOTICE");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;

	} else if (m_argc == 1) {

		/* No text to send */
		debug::DEBUG_LOG(m_parent->getNickname(), "No text to send");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTEXTTOSEND();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing NOTICE command ...");
	Arguments_Parser args_parser(m_argv[0]);
	std::vector<std::string> args = args_parser.getArguments();
	std::for_each(args.begin(), args.end(),
			boost::bind(&Request_handler::process_NOTICE, this, _1, m_argv[1]));
}

void irc::Request_handler::process_NOTICE(const std::string& nickname,
		const std::string& message) {

	/* Search the user in the database */
	debug::DEBUG_LOG(m_parent->getNickname(), "Notice", nickname);
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(nickname);

	/* Check if the user exist */
	if (!user) {

		/* User don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such nick");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHNICK(nickname);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Send the message to the remote user */
	if (user->isReceivingNotices()) {

		/* Craft and send the NOTICE */
		debug::DEBUG_LOG(m_parent->getNickname(), "Send notice");
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_NOTICE(nickname, message);
		user->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_PRIVMSG(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* No recipient */
		debug::DEBUG_LOG(m_parent->getNickname(), "No recipient");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NORECIPIENT("PRIVMSG");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;

	} else if (m_argc == 1) {

		/* No text to send */
		debug::DEBUG_LOG(m_parent->getNickname(), "No text to send");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTEXTTOSEND();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check the targets count limit */
	if (m_argc > 5) {

		/* No many targets */
		debug::DEBUG_LOG(m_parent->getNickname(), "Too many targets");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_TOOMANYTARGETS(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing PRIVMSG command ...");
	Arguments_Parser args_parser(m_argv[0]);
	std::vector<std::string> args = args_parser.getArguments();
	std::for_each(args.begin(), args.end(),
			boost::bind(&Request_handler::process_PRIVMSG, this, _1,
					m_argv[1]));
}

void irc::Request_handler::process_PRIVMSG(const std::string& receiver,
		const std::string& message) {

	/* Search the user in the database */
	debug::DEBUG_LOG(m_parent->getNickname(), "Privmsg", receiver);
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(receiver);

	/* Check if the user exist */
	if (user) {

		/* Send the message to the remote user */
		debug::DEBUG_LOG(m_parent->getNickname(), "Send privmsg to user");
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_PRIVMSG(receiver, message);
		user->write(m_reply.toString());
		m_reply.flush();

		/* Send-back the away message if enable */
		if (user->isAway()) {

			/* Send-back the away message */
			debug::DEBUG_LOG(m_parent->getNickname(), "Send away message");
			m_reply.addPrefix(user->getPrefix());
			m_reply.CMD_PRIVMSG(receiver, user->getAwayMsg());
			m_parent->write(m_reply.toString());
			m_reply.flush();
		}

	} else { /* Switch to channel mode */

		/* Search the channel in the database */
		boost::shared_ptr<Channel_info> channel =
				Server::getInstance()->getChannelsDatabase().access(receiver);

		/* Check if the channel exist */
		if (channel) {

			/* Check if the user is on the channel */
			if (m_parent->asJoin(channel) == false) {

				/* Check if the channel allow outside message */
				if (channel->isNoOutsideMsg() == false) {

					/* Cannot send to the channel */
					debug::DEBUG_LOG(m_parent->getNickname(),
							"Cannot send to channel");
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_CANNOTSENDTOCHAN(receiver);
					m_parent->write(m_reply.toString());
					m_reply.flush();

					/* Abort the command */
					return;
				}
			}

			/* Check if the channel is moderated */
			if (channel->isModerated()) {

				/* Check if the user is allowed to speak */
				if (channel->access(m_parent->shared_from_this()).canSpeak()
						== false) {

					/* Cannot send to the channel */
					debug::DEBUG_LOG(m_parent->getNickname(),
							"Cannot send to the channel");
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_CANNOTSENDTOCHAN(receiver);
					m_parent->write(m_reply.toString());
					m_reply.flush();

					/* Abort the command */
					return;
				}
			}

			/* Send the message to the remote channel */
			debug::DEBUG_LOG(m_parent->getNickname(),
					"Send privmsg to channel");
			m_reply.addPrefix(m_parent->getPrefix());
			m_reply.CMD_PRIVMSG(receiver, message);
			channel->writeToAllExcept(m_reply.toString(),
					m_parent->getNickname());
			m_reply.flush();

		} else {

			/* User don't exist */
			debug::DEBUG_LOG(m_parent->getNickname(), "No such nick");
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHNICK(receiver);
			m_parent->write(m_reply.toString());
			m_reply.flush();

			/* Abort the command */
			return;
		}
	}
}

void irc::Request_handler::handle_KILL(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc < 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("KILL");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing KILL command ...");
	process_KILL(m_argv[0], m_argv[1]);
}

void irc::Request_handler::process_KILL(const std::string& nickname,
		const std::string& comment) {

	/* Ignored : ERR_CANTKILLSERVER */

	/* Check if the user is irc ops */
	if (m_parent->isIrcOp() == false) {

		/* No privileges */
		debug::DEBUG_LOG(m_parent->getNickname(), "No privileges");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Search the user in the database */
	boost::shared_ptr<Connection> user =
			Server::getInstance()->getUsersDatabase().access(nickname);

	/* Check if the user is found */
	if (!user) {

		/* User don't exist */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such nick");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHNICK(nickname);
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Force close the remote connection with comment */
	debug::DEBUG_LOG(m_parent->getNickname(), "Force close",
			user->getNickname());
	user->close_because(comment);

	/* Back-notice local user of KILL success */
	debug::DEBUG_LOG(m_parent->getNickname(), "Back-notice for success");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_NOTICE(user->getNickname(), "User connection killed !");
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_PING(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "No origin");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOORIGIN();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check the target server */
	//if (m_argc >= 2 && m_argv[1] != m_parent->getServername()) {
	//
	//	/* Ping remote server is not implemented in this program */
	//	debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
	//	m_reply.addPrefix(m_parent->getServername());
	//	m_reply.ERR_NOSUCHSERVER(m_argv[1]);
	// m_parent->write(m_reply.toString());
	//
	//	/* Abort the command */
	//	return;
	//}

	/* Answer with PONG reply */
	debug::DEBUG_LOG(m_parent->getNickname(), "Answer with PONG");
	debug::DEBUG_LOG(m_parent->getNickname(), "Pong answer");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_PONG((m_argc >= 2) ? m_argv[1] : "", m_argv[0]);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_PONG(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc < 2) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "No origin");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOORIGIN();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check the target server */
	if (m_argv[0] != m_parent->getServername()) {

		/* Ping remote server is not implemented in this program */
		debug::DEBUG_LOG(m_parent->getNickname(), "No such server");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHSERVER(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort the command */
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

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing AWAY command ...");
	process_AWAY((m_argc >= 1) ? m_argv[0] : "");
}

void irc::Request_handler::process_AWAY(const std::string& message) {

	/* Switch according the argument */
	if (message == "") { /* Unset the away message */

		/* Set as non-away */
		m_parent->setAway(false);

		/* Notice the user of non-away */
		debug::DEBUG_LOG(m_parent->getNickname(), "No away");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_UNAWAY();
		m_parent->write(m_reply.toString());

	} else { /* Set the away message */

		/* Set as away */
		m_parent->setAway(true);
		m_parent->setAwayMsg(message);

		/* Notice the user of away */
		debug::DEBUG_LOG(m_parent->getNickname(), "Now away");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_NOWAWAY();
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_REHASH(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check if the user is IRC ops */
	if (m_parent->isIrcOp() == false) {

		/* No privileges */
		debug::DEBUG_LOG(m_parent->getNickname(), "No privileges");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort the command */
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

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check if the user is IRC ops */
	if (m_parent->isIrcOp() == false) {

		/* No privileges */
		debug::DEBUG_LOG(m_parent->getNickname(), "No privileges");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Shutdown the server */
	debug::DEBUG_LOG(m_parent->getNickname(), "Restarting server ...");
	Server::getInstance()->stop();
}

void irc::Request_handler::handle_SUMMON(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* No SUMMON support in this program */
	debug::DEBUG_LOG(m_parent->getNickname(), "No SUMMON support");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.ERR_SUMMONDISABLED();
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_USERS(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* No USERS support in this program */
	debug::DEBUG_LOG(m_parent->getNickname(), "No USERS support");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.ERR_USERSDISABLED();
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_WALLOPS(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("WALLOPS");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing WALLOPS command ...");
	process_WALLOPS(m_argv[0]);
}

void irc::Request_handler::process_WALLOPS(const std::string& message) {

	/* Check if the user is IRC ops */
	if (m_parent->isIrcOp() == false) {

		/* Abort the command */
		return;
	}

	/* Send message to all IRC ops in the database */
	debug::DEBUG_LOG(m_parent->getNickname(), "Send wallops message ...");
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_WALLOPS(message);
	Server::getInstance()->getUsersDatabase().writeToIrcOp(m_reply.toString());
}

void irc::Request_handler::handle_USERHOST(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("USERHOST");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Check the arguments count limit */
	if (m_argc > 5) {

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(),
			"Processing USERHOST command ...");
	std::string users_reply;
	for (int i = 0; i < m_argc; ++i) {

		/* Search the nickname in the database */
		boost::shared_ptr<Connection> user =
				Server::getInstance()->getUsersDatabase().access(m_argv[i]);

		/* Check if the user exist */
		if (user) {

			/* Add the user informations to the list */
			debug::DEBUG_LOG(m_parent->getNickname(), "USERHOST", m_argv[i]);
			users_reply += m_argv[i] + ((user->isIrcOp()) ? '*' : ' ') + '='
					+ ((user->isAway()) ? '+' : '-') + user->getHostname()
					+ ' ';
		}

	}

	/* Send the informations as complete reply */
	debug::DEBUG_LOG(m_parent->getNickname(), "USERHOST complete reply");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_USERHOST(users_reply);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_ISON(void) {

	/* Check the user state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort the command */
		return;
	}

	/* Check the arguments count */
	if (m_argc == 0) {

		/* Need more params */
		debug::DEBUG_LOG(m_parent->getNickname(), "Need more params");
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("ISON");
		m_parent->write(m_reply.toString());

		/* Abort the command */
		return;
	}

	/* Process the command */
	debug::DEBUG_LOG(m_parent->getNickname(), "Processing ISON command ...");
	std::string ison_reply;
	for (int i = 0; i < m_argc; ++i) {

		/* Search the nickname in the database */
		boost::shared_ptr<Connection> user =
				Server::getInstance()->getUsersDatabase().access(m_argv[i]);

		/* Check if the user exist in the database */
		if (user) {

			/* Add the user informations to the list */
			debug::DEBUG_LOG(m_parent->getNickname(), "ISON", m_argv[i]);
			ison_reply += m_argv[i] + ' ';
		}

	}

	/* Send the informations as complete reply */
	debug::DEBUG_LOG(m_parent->getNickname(), "ISON complete reply");
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_ISON(ison_reply);
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::send_welcome_msg(void) {

	/* Send the welcome message */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_CUSTOM("001",
			m_parent->getNickname() + " :Welcome to the "
					+ m_parent->getServername() + " IRC network "
					+ m_parent->getPrefix());
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Send the server info message */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_CUSTOM("002",
			m_parent->getNickname() + " :Your host is "
					+ m_parent->getServername()
					+ ", running SkyIRC version 1.0");
	m_parent->write(m_reply.toString());
	m_reply.flush();
	m_reply.addPrefix(m_parent->getServername());
	m_reply.RPL_CUSTOM("003",
			m_parent->getNickname() + " :This server was created on "
					+ boost::posix_time::to_simple_string(
							Server::getInstance()->runSince()));
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Stats section */
	if (Server::getInstance()->getConfiguration().send_stats) {

		/* Send the users stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSERCLIENT(
				Server::getInstance()->getUsersDatabase().getUsersCount(),
				Server::getInstance()->getUsersDatabase().getInvisibleUsersCount(),
				1);
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* Send the irc ops stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSEROP(
				Server::getInstance()->getUsersDatabase().getIRCopsCount());
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* Send the channels stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSERCHANNELS(
				Server::getInstance()->getChannelsDatabase().getChannelsCount());
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* Send the local users stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSERME(
				Server::getInstance()->getUsersDatabase().getUsersCount(), 1);
		m_parent->write(m_reply.toString());
	}

	/* MOTD section */
	if (Server::getInstance()->getConfiguration().send_motd) {

		/* Send the MOTD message */
		send_motd();

	} else {

		/* Send the "no MOTD" message */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOMOTD();
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::send_motd(void) {

	/* Open the MOTD file */
	std::ifstream motd_file(
			Server::getInstance()->getConfiguration().motd_filename.c_str());

	/* Check if file is open */
	if (motd_file.is_open()) {

		/* Tmp string */
		std::string line;

		/* Send the MOTD header*/
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_MOTDSTART(m_parent->getServername());
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* For each line of the MOTD */
		while (motd_file.good()) {

			/* Get line from the file */
			std::getline(motd_file, line);

			/* Send the line */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.RPL_MOTD(line);
			m_parent->write(m_reply.toString());
			m_reply.flush();
		}

		/* Send the MOTD footer */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_ENDOFMOTD();
		m_parent->write(m_reply.toString());

		/* Close the file */
		motd_file.close();

	} else { /* File open error */

		/* Send the file I/O error message */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_FILEERROR("open",
				Server::getInstance()->getConfiguration().motd_filename);
		m_parent->write(m_reply.toString());
	}
}
