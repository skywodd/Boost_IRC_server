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

		/* Handle command */
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
			m_parent->updateIdleTime();
			handle_NOTICE();
		} else if (command == "PRIVMSG") {
			m_parent->updateIdleTime();
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
		} else { /* Unkwnon command */
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_ALREADYREGISTRED();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 1) {

		/* Need more params */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("PASS");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check password */
	if (m_parent->getConf().server_password.count(m_argv[0])) {

		/* Password match */
		m_parent->setState(Connection::WAIT_FOR_NICK);

	} else {

		/* Password don't match */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_PASSWDMISMATCH();
		m_parent->write(m_reply.toString());
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NONICKNAMEGIVEN();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check nickname validity */
	if (Sanity_check::is_valid_nickname(m_argv[0]) == false) {

		/* Bad formated nickname */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_ERRONEUSNICKNAME(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check if nickname already exist */
	if (m_parent->getUsersDatabase().access(m_argv[0])) {

		/* Nickname already exist */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NICKNAMEINUSE(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* No nickname collision check (ERR_NICKCOLLISION) */

	/* Store nickname */
	m_parent->setNickname(m_argv[0]);

	/* Update m_parent state if necesary */
	if (m_parent->getState() == Connection::WAIT_FOR_NICK) {

		/* State require NICK -> require USER to be ready */
		m_parent->setState(Connection::WAIT_FOR_USER);

	} else if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Notice users on joined channels of nickname change */
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_NICK(m_argv[0]);
		m_parent->writeToChannels(m_reply.toString());
	}

}

void irc::Request_handler::handle_USER(void) {

	/* Check state */
	if (m_parent->getState() != Connection::WAIT_FOR_USER
			&& m_parent->getState() != Connection::WAIT_FOR_NICK) {

		/* Already registered */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_ALREADYREGISTRED();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 4) {

		/* Need more params */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("USER");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* We don't use ident protocol, fake answer */
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

	/* Store user informations */
	m_parent->setUsername("~" + m_argv[0]);
	/* hostname and server name are already known */
	m_parent->setRealname(m_argv[3]);

	/* Update state */
	m_parent->setState(Connection::READY_FOR_MSG);

	/* Send welcome message */
	send_welcome_msg();
}

void irc::Request_handler::handle_SERVER(void) {

	/* Not implemented in this program */
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("OPER");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Ignored : ERR_NOOPERHOST */

	/* Find username */
	std::map<std::string, std::string>::const_iterator ircop =
			m_parent->getConf().server_ircop.find(m_argv[0]);

	/* Check username */
	if (ircop == m_parent->getConf().server_ircop.end()) {

		/* Wrong username */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_PASSWDMISMATCH();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check password */
	if ((*ircop).second == m_argv[1]) {

		/* Well done, your oper now ! */
		m_parent->setIrcOp(true);

		/* Notice user of +o */
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.RPL_YOUREOPER();
		m_parent->write(m_reply.toString());

	} else {

		/* Wrong password */
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

	/* Close connection */
	if (m_argc == 1) {

		/* With custom quit message */
		m_parent->close_because(m_argv[0]);

	} else {

		/* With empty quit message */
		m_parent->close_because("");
	}
}

void irc::Request_handler::handle_SQUIT(void) {

	/* Not implemented in this program */
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("JOIN");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check for join count limit */
	if (m_parent->getJoinCount() == m_parent->getJoinCoutLimit()) {

		/* Limit reached */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_TOOMANYCHANNELS(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search channel */
	boost::shared_ptr<Channel_info> channel =
			m_parent->getChannelsDatabase().access(m_argv[0]);

	/* Check if channel exist */
	if (channel) {

		/* Check if user is banned */
		if (channel->isBanned(m_parent->getPrefix())) {

			/* You're banned ... */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_BANNEDFROMCHAN(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}

		/* Check if channel is protected */
		if (channel->isPrivate()) {

			/* Check arguments count */
			if (m_argc != 2) {

				/* Need more params */
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_NEEDMOREPARAMS("JOIN");
				m_parent->write(m_reply.toString());

				/* Abort command */
				return;
			}

			/* Check password */
			if (channel->getPassword() != m_argv[1]) {

				/* Password don't match */
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_BADCHANNELKEY(m_argv[0]);
				m_parent->write(m_reply.toString());

				/* Abort command */
				return;
			}
		}

		/* Check if channel is on invitation */
		if (channel->isInviteOnly()) {

			/* Check if user is invited to join the channel */
			if (channel->isInvited(m_parent->shared_from_this()) == false) {

				/* User is not invited to join the channel */
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_INVITEONLYCHAN(m_argv[0]);
				m_parent->write(m_reply.toString());

				/* Abort command */
				return;
			}
		}

		/* Check if channel is full */
		if (channel->getUsersCount() == channel->getUsersLimit()) {

			/* No more free place */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_CHANNELISFULL(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}

	} else { /* Channel don't exist */

		/* Check channel name validity */
		if (Sanity_check::is_valid_channame(m_argv[0]) == false) {

			/* Channel name is crappy */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHCHANNEL(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}

		/* Create channel */
		channel = m_parent->getChannelsDatabase().add(m_argv[0]);
	}

	/* Add user to channel (channel and user side) */
	channel->addJoin(m_parent->shared_from_this(), true);
	m_parent->addJoin(m_parent->getChannelsDatabase().access(m_argv[0]));

	/* Notice users on channel of JOIN */
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_JOIN(m_argv[0], m_argv[1]);
	channel->writeToAll(m_reply.toString());

	/* Remarks: should normally send TOPIC and users list */
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("PART");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search channel */
	boost::shared_ptr<Channel_info> channel =
			m_parent->getChannelsDatabase().access(m_argv[0]);

	/* Check if channel exist */
	if (!channel) {

		/* Channel don't exist */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHCHANNEL(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check if user as join the channel */
	if (m_parent->asJoin(channel) == false) {

		/* User as not join the channel before PART */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTONCHANNEL(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Remove user from channel (client and channel side) */
	m_parent->removeJoin(channel);
	channel->removeJoin(m_parent->shared_from_this());

	/* Notice users on channel of client PART */
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_PART(m_argv[0]);
	channel->writeToAll(m_reply.toString());
}

void irc::Request_handler::handle_MODE(void) {
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("TOPIC");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search channel */
	boost::shared_ptr<Channel_info> channel =
			m_parent->getChannelsDatabase().access(m_argv[0]);

	/* Check if channel exist */
	if (!channel) {

		/* Channel don't exist */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHCHANNEL(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check arguments */
	if (m_argc == 1) { /* Read topic */

		/* Check if topic is set */
		if (channel->getTopic() != "") {

			/* Send topic */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.RPL_TOPIC(m_argv[0], channel->getTopic());
			m_parent->write(m_reply.toString());

		} else {

			/* Topic is not set */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.RPL_NOTOPIC(m_argv[0]);
			m_parent->write(m_reply.toString());
		}

	} else { /* Set topic */

		/* Check if user is on the channel */
		if (m_parent->asJoin(channel) == false) {

			/* Not on the channel */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOTONCHANNEL(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}

		/* If topic can be set by non-op */
		if (channel->isTopicSetByOpOnly()) {

			/* Check if user is op */
			if (channel->access(m_parent->shared_from_this()).isOp() == false) {

				/* Don't have auth to change topic */
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_CHANOPRIVSNEEDED(m_argv[0]);
				m_parent->write(m_reply.toString());

				/* Abort command */
				return;
			}
		}

		/* Change topic */
		channel->setTopic(m_argv[1]);

		/* Notice users on channels of topic change */
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_TOPIC(m_argv[0], m_argv[1]);
		channel->writeToAll(m_reply.toString());
	}
}

void irc::Request_handler::handle_NAMES(void) {
// TODO
}

void irc::Request_handler::handle_LIST(void) {
// TODO
}

void irc::Request_handler::handle_INVITE(void) {
// TODO
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("KICK");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search channel */
	boost::shared_ptr<Channel_info> channel =
			m_parent->getChannelsDatabase().access(m_argv[0]);

	/* Check if channel exist */
	if (!channel) {

		/* Channel don't exist */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHCHANNEL(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check is user is op on channel */
	if (channel->access(m_parent->shared_from_this()).isOp() == false) {

		/* User is not op on channel */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_CHANOPRIVSNEEDED(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search user */
	boost::shared_ptr<Connection> user = m_parent->getUsersDatabase().access(
			m_argv[1]);

	/* Check is user exist and is on the channel */
	if (!user || m_parent->asJoin(channel) == false) {

		/* User not on channel */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOTONCHANNEL(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Remove user from channel (client and channel side) */
	user->removeJoin(channel);
	channel->removeJoin(user);

	/* Notice users on channel of client KICK */
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_KICK(m_argv[0], m_argv[1], (m_argc == 3) ? m_argv[2] : "");
	channel->writeToAll(m_reply.toString());
}

void irc::Request_handler::handle_VERSION(void) {
// TODO
}

void irc::Request_handler::handle_STATS(void) {
// TODO
}

void irc::Request_handler::handle_LINKS(void) {
// TODO
}

void irc::Request_handler::handle_TIME(void) {
// TODO
}

void irc::Request_handler::handle_CONNECT(void) {
// TODO
}

void irc::Request_handler::handle_TRACE(void) {
// TODO
}

void irc::Request_handler::handle_ADMIN(void) {
// TODO
}

void irc::Request_handler::handle_INFO(void) {
// TODO
}

void irc::Request_handler::handle_WHO(void) {
// TODO
}

void irc::Request_handler::handle_WHOIS(void) {
// TODO
}

void irc::Request_handler::handle_WHOAS(void) {
// TODO
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

	/* Search user */
	boost::shared_ptr<Connection> user = m_parent->getUsersDatabase().access(
			m_argv[0]);

	/* Check if user exist */
	if (!user) {

		/* User don't exist */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHNICK(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Update away status if necesary */
	if (m_parent->isAway()) {

		/* User is no more away */
		m_parent->setAway(false);

		/* Notice user that is no more away */
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_NOTICE(m_argv[0], m_argv[1]);
		m_parent->write(m_reply.toString());
		m_reply.flush();
	}

	/* Send message to remote user */
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_NOTICE(m_argv[0], m_argv[1]);
	m_parent->write(m_reply.toString());
	user->write(m_reply.toString());
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

	/* Search user */
	boost::shared_ptr<Connection> user = m_parent->getUsersDatabase().access(
			m_argv[0]);

	/* Check if user exist */
	if (user) {

		/* Send message to remote user */
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_PRIVMSG(m_argv[0], m_argv[1]);
		user->write(m_reply.toString());

		/* Send-back away message if enable */
		if (user->isAway()) {

			/* Send away message */
			m_reply.flush();
			m_reply.addPrefix(user->getPrefix());
			m_reply.CMD_PRIVMSG(m_argv[0], user->getAwayMsg());
			m_parent->write(m_reply.toString());
		}

	} else { /* Switch to channel mode */

		/* Search channel */
		boost::shared_ptr<Channel_info> channel =
				m_parent->getChannelsDatabase().access(m_argv[0]);

		/* Check if channel exist */
		if (channel) {

			/* Send message to remote channel */
			m_reply.addPrefix(m_parent->getPrefix());
			m_reply.CMD_PRIVMSG(m_argv[0], m_argv[1]);
			channel->writeToAll(m_reply.toString());

		} else {

			/* User don't exist */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOSUCHNICK(m_argv[0]);
			m_parent->write(m_reply.toString());

			/* Abort command */
			return;
		}
	}

	/* Update away status if necesary */
	if (m_parent->isAway()) {

		/* User is no more away */
		m_parent->setAway(false);

		/* Notice user that is no more away */
		m_reply.flush();
		m_reply.addPrefix(m_parent->getPrefix());
		m_reply.CMD_NOTICE(m_argv[0], m_argv[1]);
		m_parent->write(m_reply.toString());
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("KILL");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check if user is irc op */
	if (m_parent->isIrcOp() == false) {

		/* No privileges */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Search user */
	boost::shared_ptr<Connection> user = m_parent->getUsersDatabase().access(
			m_argv[0]);

	/* If user is found */
	if (!user) {

		/* User don't exist */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHNICK(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Force close with comment */
	user->close_because(m_argv[1]);

	/* Back-notice op of KILL success */
	m_reply.addPrefix(m_parent->getServername());
	m_reply.CMD_NOTICE(m_parent->getNickname(), "User connection killed !");
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOORIGIN();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check target server */
	if (m_argv[0] == m_parent->getServername()) {

		/* Ping remote server is not implemented in this program */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHSERVER(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Answer with PONG */
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOORIGIN();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check target server */
	if (m_argv[0] == m_parent->getServername()) {

		/* Ping remote server is not implemented in this program */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOSUCHSERVER(m_argv[0]);
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check ping deamon */
	if (m_argv[1] == m_parent->getLastPingArg()) {

		/* Restart deadline timer */
		m_parent->restartDeadlineTimer();
	}
}

void irc::Request_handler::handle_ERROR(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Check arguments count */
	if (m_argc != 1) {

		/* Need more params */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("ERROR");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check if user is irc op */
	if (m_parent->isIrcOp() == false) {

		/* No auth to do what */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Output error to server stderr */
	std::cerr << "[ERROR] from " << m_parent->getNickname() << ": " << m_argv[0]
			<< std::endl;

	/* Prepare NOTICE message */
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_NOTICE(m_parent->getServername(), "ERROR: " + m_argv[0]);

	/* Notice all irc ops with notice */
	m_parent->getUsersDatabase().writeToIrcOp(m_reply.toString());
}

void irc::Request_handler::handle_AWAY(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* Switch according arguments */
	if (m_argc == 0) { /* Unset away message */

		/* Set as non-away */
		m_parent->setAway(false);

		/* Notice user of non-away */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_UNAWAY();
		m_parent->write(m_reply.toString());

	} else { /* Set away message */

		/* Set as away */
		m_parent->setAway(true);

		/* Set away message */
		m_parent->setAwayMsg(m_argv[0]);

		/* Notice user of non-away */
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

	/* Check if user is irc op */
	if (m_parent->isIrcOp() == false) {

		/* No auth to do what */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Answer but do nothing (no configuration file used yet) */
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

	/* Check if user is irc op */
	if (m_parent->isIrcOp() == false) {

		/* No auth to do what */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NOPRIVILEGES();
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* shutdown and restart server */
	// TODO
}

void irc::Request_handler::handle_SUMMON(void) {

	/* Check state */
	if (m_parent->getState() != Connection::READY_FOR_MSG) {

		/* Abort command */
		return;
	}

	/* No SUMMON support in this program */
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
		m_reply.addPrefix(m_parent->getServername());
		m_reply.ERR_NEEDMOREPARAMS("ERROR");
		m_parent->write(m_reply.toString());

		/* Abort command */
		return;
	}

	/* Check if user is irc op */
	if (m_parent->isIrcOp() == false) {

		/* Abort command */
		return;
	}

	/* Send message to all IRC ops connected */
	m_reply.addPrefix(m_parent->getPrefix());
	m_reply.CMD_WALLOPS(m_argv[0]);
	m_parent->getUsersDatabase().writeToIrcOp(m_reply.toString());
}

void irc::Request_handler::handle_USERHOST(void) {
// TODO
}

void irc::Request_handler::handle_ISON(void) {
// TODO
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
							m_parent->getConf().m_since));
	m_parent->write(m_reply.toString());
	m_reply.flush();

	/* Stats section */
	if (m_parent->getConf().send_stats) {

		/* Send users stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSERCLIENT(m_parent->getUsersDatabase().getUsersCount(),
				m_parent->getUsersDatabase().getInvisibleUsersCount(), 1);
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* Send irc ops stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSEROP(m_parent->getUsersDatabase().getIRCopsCount());
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* Send channels stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSERCHANNELS(
				m_parent->getChannelsDatabase().getChannelsCount());
		m_parent->write(m_reply.toString());
		m_reply.flush();

		/* Send local users stats */
		m_reply.addPrefix(m_parent->getServername());
		m_reply.RPL_LUSERME(m_parent->getUsersDatabase().getUsersCount(), 1);
		m_parent->write(m_reply.toString());
	}

	/* MOTD section */
	if (m_parent->getConf().send_motd) {

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
	std::ifstream motd_file(m_parent->getConf().motd_filename.c_str());

	/* Test file open */
	if (motd_file.is_open()) {

		/* Temp string */
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
		m_reply.ERR_FILEERROR("open", m_parent->getConf().motd_filename);
		m_parent->write(m_reply.toString());
	}
}
