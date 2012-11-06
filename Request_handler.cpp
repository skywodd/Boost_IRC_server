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
#include <boost/bind.hpp>
#include "Request_handler.hpp"
#include "Connection.hpp"
#include "Configuration.hpp"
#include "Request_parser.hpp"
#include "Channels_manager.hpp"
#include "Users_manager.hpp"
#include "Channel_info.hpp"
#include "Reply_generator.hpp"
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
	if (m_parent->getState() == Connection::WAIT_FOR_PASS) { /* Require PASS */

		/* Add server prefix */
		m_reply.addPrefix(m_parent->getServername());

		/* Check arguments count */
		if (m_argc == 1) {

			/* Check password */
			if (m_parent->getConf().server_password.count(m_argv[0])) {

				/* Password match */
				m_parent->setState(Connection::WAIT_FOR_NICK);

			} else { /* Password don't match */
				m_reply.ERR_PASSWDMISMATCH();
				m_parent->write(m_reply.toString());
			}

		} else { /* No arguments */
			m_reply.ERR_NEEDMOREPARAMS("PASS");
			m_parent->write(m_reply.toString());
		}

	} else { /* Already registered */
		m_reply.ERR_ALREADYREGISTRED();
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_NICK(void) {
	/* Check state */
	if (m_parent->getState() != Connection::WAIT_FOR_PASS) {

		/* Check arguments count */
		if (m_argc == 1 || m_argc == 2) {

			/* Check if nickname already exist */
			if (!m_parent->getUsersDatabase().access(m_argv[0])) {

				/* Store nickname */
				m_parent->setNickname(m_argv[0]);

				/* Update m_parent state if necesary */
				if (m_parent->getState() == Connection::WAIT_FOR_NICK) {
					m_parent->setState(Connection::WAIT_FOR_USER);

				} else if (m_parent->getState()
						== Connection::READY_FOR_MSG) {

					/* Notice users on joined channels of nickname change */
					m_reply.addPrefix(m_parent->getNickname());
					m_reply.CMD_NICK(m_argv[0]);
					m_parent->writeToChannels(m_reply.toString());
				}

			} else { /* Nickname already exist */
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_NICKNAMEINUSE(m_argv[0]);
				m_parent->write(m_reply.toString());
			}

		} else { /* No nickname */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NONICKNAMEGIVEN();
			m_parent->write(m_reply.toString());
		}
	}
}

void irc::Request_handler::handle_USER(void) {
	/* Check state */
	if (m_parent->getState() == Connection::WAIT_FOR_USER
			|| m_parent->getState() == Connection::WAIT_FOR_NICK) {

		/* Add server prefix */
		m_reply.addPrefix(m_parent->getServername());

		/* Check arguments count */
		if (m_argc == 4) {

			/* Store informations */
			m_parent->setUsername(m_argv[0]);
			// hostname and server name are already known
			m_parent->setRealname(m_argv[3]);

			/* Update state */
			m_parent->setState(Connection::READY_FOR_MSG);

			/* Send 001 message */
			//m_reply.addPrefix(m_parent->getServername());
			m_reply.RPL_CUSTOM("001",
					m_parent->getNickname() + " :Welcome to the "
							+ m_parent->getServername() + " IRC network "
							+ m_parent->getNickname());

		} else { /* Arguments error */
			m_reply.ERR_NEEDMOREPARAMS("USER");
		}

	} else { /* Already registered */
		m_reply.ERR_ALREADYREGISTRED();
	}

	/* Send answer */
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_SERVER(void) {
	// TODO
}

void irc::Request_handler::handle_OPER(void) {
	/* Check state */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Add server prefix */
		m_reply.addPrefix(m_parent->getServername());

		/* Check arguments count */
		if (m_argc == 2) {

			/* Check username */
			std::map<std::string, std::string>::const_iterator ircop =
					m_parent->getConf().server_ircop.find(m_argv[0]);
			if (ircop != m_parent->getConf().server_ircop.end()) {

				/* Check password */
				if ((*ircop).second == m_argv[1]) {

					/* Well done, your oper now ! */
					m_parent->setIrcOp(true);
					m_reply.RPL_YOUREOPER();

				} else { /* Wrong password */
					m_reply.ERR_PASSWDMISMATCH();
				}

			} else { /* Wrong username */
				m_reply.ERR_PASSWDMISMATCH();
			}

		} else { /* Arguments error */
			m_reply.ERR_NEEDMOREPARAMS("OPER");
		}

		/* Send answer */
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_QUIT(void) {
	/* Check state */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Close m_parent */
		if (m_argc == 1) { /* With custom quit message */
			m_parent->close_because(m_argv[0]);

		} else { /* With empty quit message */
			m_parent->close_because("");
		}
	}
}

void irc::Request_handler::handle_SQUIT(void) {
	// TODO
}

void irc::Request_handler::handle_JOIN(void) {
	/* Check state */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Check arguments count */
		if (m_argc == 1 || m_argc == 2) {

			/* Search channel */
			boost::shared_ptr<Channel_info> channel =
					m_parent->getChannelsDatabase().access(m_argv[0]);

			/* Check if channel exist */
			if (channel) {

				/* Check if channel is protected */
				if (channel->isPrivate()) {

					/* Check password */
					if (m_argc == 2 && channel->getPassword() == m_argv[1]) {

						/* Check if channel is on invitation */
						if (channel->isInviteOnly()) {

							/* Check if user is invited to join the channel */
							if (channel->isInvited(
									m_parent->shared_from_this())) {

								/* Check if channel is full */
								if (channel->getUsersCount()
										== channel->getUsersLimit()) {
									m_reply.addPrefix(
											m_parent->getServername());
									m_reply.ERR_CHANNELISFULL(m_argv[0]); /* no more place */
									m_parent->write(m_reply.toString());

								} else { /* Can join the channel */

									/* Join the channel */
									m_reply.addPrefix(m_parent->getNickname());
									channel->addJoin(
											m_parent->shared_from_this());
									m_parent->addJoin(channel);
									m_reply.CMD_JOIN(m_argv[0], m_argv[1]);
									channel->writeToAll(m_reply.toString());
								}

							} else { /* User is not invited to join the channel */
								m_reply.addPrefix(m_parent->getServername());
								m_reply.ERR_INVITEONLYCHAN(m_argv[0]);
								m_parent->write(m_reply.toString());
							}

						} else { /* Channel don't require invitation */

							/* Check if channel is full */
							if (channel->getUsersCount()
									== channel->getUsersLimit()) {
								m_reply.addPrefix(m_parent->getServername());
								m_reply.ERR_CHANNELISFULL(m_argv[0]); /* no more place */
								m_parent->write(m_reply.toString());

							} else { /* Can join the channel */

								/* Join the channel */
								m_reply.addPrefix(m_parent->getNickname());
								channel->addJoin(
										m_parent->shared_from_this());
								m_parent->addJoin(channel);
								m_reply.CMD_JOIN(m_argv[0], m_argv[1]);
								channel->writeToAll(m_reply.toString());
							}
						}

					} else { /* Password don't match */
						m_reply.addPrefix(m_parent->getServername());
						m_reply.ERR_PASSWDMISMATCH();
						m_parent->write(m_reply.toString());
					}

				} else { /* Channel is not password protected */

					/* Check if channel is on invitation */
					if (channel->isInviteOnly()) {

						/* Check if user is invited to join the channel */
						if (channel->isInvited(
								m_parent->shared_from_this())) {

							/* Check if channel is full */ // TODO useless (channel created = user nb = 0)
							if (channel->getUsersCount()
									== channel->getUsersLimit()) {
								m_reply.addPrefix(m_parent->getServername());
								m_reply.ERR_CHANNELISFULL(m_argv[0]); /* no more place */
								m_parent->write(m_reply.toString());

							} else { /* Can join the channel */

								/* Join the channel */
								m_reply.addPrefix(m_parent->getNickname());
								channel->addJoin(
										m_parent->shared_from_this());
								m_parent->addJoin(channel);
								m_reply.CMD_JOIN(m_argv[0], m_argv[1]);
								channel->writeToAll(m_reply.toString());
							}
						} else { /* User is not invited to join the channel */
							m_reply.addPrefix(m_parent->getServername());
							m_reply.ERR_INVITEONLYCHAN(m_argv[0]);
							m_parent->write(m_reply.toString());
						}

					} else { /* Channel don't require invitation */

						/* Check if channel is full */
						if (channel->getUsersCount()
								== channel->getUsersLimit()) {
							m_reply.addPrefix(m_parent->getServername());
							m_reply.ERR_CHANNELISFULL(m_argv[0]); /* no more place */
							m_parent->write(m_reply.toString());

						} else { /* Can join the channel */

							/* Join the channel */
							m_reply.addPrefix(m_parent->getNickname());
							channel->addJoin(m_parent->shared_from_this());
							m_parent->addJoin(channel);
							m_reply.CMD_JOIN(m_argv[0], m_argv[1]);
							channel->writeToAll(m_reply.toString());
						}
					}
				}

			} else { /* Channel don't exist */

				/* Add channel */
				if (m_parent->getChannelsDatabase().getChannelsCount()
						== m_parent->getChannelsDatabase().getChannelsCountLimit()) {

					/* Limit reached */
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_TOOMANYCHANNELS(m_argv[0]);
					m_parent->write(m_reply.toString());

				} else { /* Create channel */

					m_reply.addPrefix(m_parent->getNickname());
					m_parent->getChannelsDatabase().add(m_argv[0],
							m_parent->shared_from_this());
					m_parent->addJoin(
							m_parent->getChannelsDatabase().access(m_argv[0]));
					m_reply.CMD_JOIN(m_argv[0], m_argv[1]);
					m_parent->write(m_reply.toString());
				}
			}

		} else { /* Arguments error */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NEEDMOREPARAMS("JOIN");
			m_parent->write(m_reply.toString());
		}
	}
}

void irc::Request_handler::handle_PART(void) {
	/* Check state */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Check arguments count */
		if (m_argc == 1) {

			/* Search channel */
			boost::shared_ptr<Channel_info> channel =
					m_parent->getChannelsDatabase().access(m_argv[0]);

			/* Check if channel exist */
			if (channel) {

				/* Check if user as join the channel */
				if (m_parent->asJoin(channel)) {

					/* Notice users on channel of PART */
					m_reply.addPrefix(m_parent->getNickname());
					m_reply.CMD_PART(m_argv[0]);
					channel->writeToAll(m_reply.toString());

					/* Remove user from channel */
					m_parent->removeJoin(channel);
					channel->removeJoin(m_parent->shared_from_this());

				} else { /* User as not join the channel before PART */
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_NOTONCHANNEL(m_argv[0]);
					m_parent->write(m_reply.toString());
				}

			} else { /* Channel don't exist */
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_NOSUCHCHANNEL(m_argv[0]);
				m_parent->write(m_reply.toString());
			}

		} else { /* Arguments error */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NEEDMOREPARAMS("PART");
			m_parent->write(m_reply.toString());
		}
	}
}

void irc::Request_handler::handle_MODE(void) {
	// TODO
}

void irc::Request_handler::handle_TOPIC(void) {
	/* Check status */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Check arguments */
		if (m_argc == 1) { /* Read topic */

			/* Search channel */
			boost::shared_ptr<Channel_info> channel =
					m_parent->getChannelsDatabase().access(m_argv[0]);

			/* Check if channel exist */
			if (channel) {

				/* Check if topic is set */
				if (channel->getTopic() != "") {
					m_reply.RPL_TOPIC(m_argv[0], channel->getTopic());
					m_parent->write(m_reply.toString());

				} else {/* Topic is not set */
					m_reply.RPL_NOTOPIC(m_argv[0]);
					m_parent->write(m_reply.toString());
				}

			} else { /* Channel don't exist */
				m_reply.ERR_NOSUCHCHANNEL(m_argv[0]);
				m_parent->write(m_reply.toString());
			}

		} else if (m_argc == 2) { /* Set topic */

			/* Search channel */
			boost::shared_ptr<Channel_info> channel =
					m_parent->getChannelsDatabase().access(m_argv[0]);

			/* Check if channel exist */
			if (channel) {

				/* If topic can be set by non-op */
				if (channel->isTopicSetByOpOnly()) {

					/* Check if user is on the channel */
					if (channel->asJoin(m_parent->shared_from_this())) {

						/* Check if user is op */
						if (channel->access(m_parent->shared_from_this()).isOp()) {

							/* Change topic */
							channel->setTopic(m_argv[1]);
							m_reply.CMD_TOPIC(m_argv[0], m_argv[1]);
							channel->writeToAll(m_reply.toString());

						} else { /* Don't have auth to change topic */
							m_reply.ERR_CHANOPRIVSNEEDED(m_argv[0]);
							m_parent->write(m_reply.toString());
						}

					} else { /* Not on the channel */
						m_reply.ERR_NOTONCHANNEL(m_argv[0]);
						m_parent->write(m_reply.toString());
					}

				} else {

					/* Change topic */
					channel->setTopic(m_argv[1]);
					m_reply.CMD_TOPIC(m_argv[0], m_argv[1]);
					channel->writeToAll(m_reply.toString());
				}

			} else { /* Channel don't exist */
				m_reply.ERR_NOSUCHCHANNEL(m_argv[0]);
				m_parent->write(m_reply.toString());
			}

		} else { /* Arguments error */
			m_reply.ERR_NEEDMOREPARAMS("TOPIC");
			m_parent->write(m_reply.toString());
		}
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
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Check arguments count */
		if (m_argc == 2 || m_argc == 3) {

			/* Search channel */
			boost::shared_ptr<Channel_info> channel =
					m_parent->getChannelsDatabase().access(m_argv[0]);

			/* Check if channel exist */
			if (channel) {

				/* Check is user is on channel */
				if (m_parent->asJoin(channel)) {

					/* Check is user is op on channel */
					if (channel->access(m_parent->shared_from_this()).isOp()) {

						/* Search user */
						boost::shared_ptr<Connection> user =
								m_parent->getUsersDatabase().access(m_argv[1]);

						/* Check if user as join the channel */
						if (user && user->asJoin(channel)) {

							/* Notice users on channel of PART */
							m_reply.addPrefix(m_parent->getNickname());
							m_reply.CMD_KICK(m_argv[0], m_argv[1],
									(m_argc == 3) ? m_argv[2] : "");
							channel->writeToAll(m_reply.toString());

							/* Remove user from channel */
							user->removeJoin(channel);
							channel->removeJoin(user);
						}

					} else { /* User is not op on channel */
						m_reply.addPrefix(m_parent->getServername());
						m_reply.ERR_CHANOPRIVSNEEDED(m_argv[0]);
						m_parent->write(m_reply.toString());
					}

				} else { /* User not on channel */
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_NOTONCHANNEL(m_argv[0]);
					m_parent->write(m_reply.toString());
				}

			} else { /* Channel don't exist */
				m_reply.addPrefix(m_parent->getServername());
				m_reply.ERR_NOSUCHCHANNEL(m_argv[0]);
				m_parent->write(m_reply.toString());
			}

		} else {/* Arguments error */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NEEDMOREPARAMS("KICK");
			m_parent->write(m_reply.toString());
		}
	}
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
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Check arguments count */
		if (m_argc == 2) {

			/* Switch according mask */
			if (m_argv[0][0] == '#') { /* Channel */

				/* Forbidden */
				//TODO
			} else { /* Nickname */

				/* Search user */
				boost::shared_ptr<Connection> user =
						m_parent->getUsersDatabase().access(m_argv[0]);

				/* Check if user exist */
				if (user) {

					/* Craft and send message */
					m_reply.addPrefix(m_parent->getNickname());
					m_reply.CMD_NOTICE(m_argv[0], m_argv[1]);
					user->write(m_reply.toString());
					m_parent->write(m_reply.toString());

				} else { /* User don't exist */
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_NOSUCHNICK(m_argv[0]);
					m_parent->write(m_reply.toString());
				}
			}

		} else if (m_argc == 1) { /* No text argument */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOTEXTTOSEND();
			m_parent->write(m_reply.toString());

		} else { /* Arguments error */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NORECIPIENT("NOTICE");
			m_parent->write(m_reply.toString());
		}
	}
}

void irc::Request_handler::handle_PRIVMSG(void) {
	/* Check state */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Check arguments count */
		if (m_argc == 2) {

			/* Switch according mask */
			if (m_argv[0][0] == '#') { /* Channel */

				/* Search channel */
				boost::shared_ptr<Channel_info> channel =
						m_parent->getChannelsDatabase().access(m_argv[0]);

				/* Check if channel exist */
				if (channel) {

					/* Craft and send message */
					m_reply.addPrefix(m_parent->getNickname());
					m_reply.CMD_PRIVMSG(m_argv[0], m_argv[1]);
					channel->writeToAll(m_reply.toString());

				} else { /* Channel don't exist */
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_NOSUCHCHANNEL(m_argv[0]);
					m_parent->write(m_reply.toString());
				}

			} else { /* Nickname */

				/* Search user */
				boost::shared_ptr<Connection> user =
						m_parent->getUsersDatabase().access(m_argv[0]);

				/* Check if user exist */
				if (user) {

					/* Craft and send message */
					m_reply.addPrefix(m_parent->getNickname());
					m_reply.CMD_PRIVMSG(m_argv[0], m_argv[1]);
					user->write(m_reply.toString());
					m_parent->write(m_reply.toString());

				} else { /* User don't exist */
					m_reply.addPrefix(m_parent->getServername());
					m_reply.ERR_NOSUCHNICK(m_argv[0]);
					m_parent->write(m_reply.toString());
				}
			}

		} else if (m_argc == 1) { /* No text argument */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NOTEXTTOSEND();
			m_parent->write(m_reply.toString());

		} else { /* Arguments error */
			m_reply.addPrefix(m_parent->getServername());
			m_reply.ERR_NORECIPIENT("PRIVMSG");
			m_parent->write(m_reply.toString());
		}
	}
}

void irc::Request_handler::handle_KILL(void) {
	/* Check state */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Add server prefix */
		m_reply.addPrefix(m_parent->getServername());

		/* Check arguments count */
		if (m_argc == 2) {

			/* Check if user is irc op */
			if (m_parent->isIrcOp()) {

				/* Search user */
				boost::shared_ptr<Connection> user =
						m_parent->getUsersDatabase().access(m_argv[0]);

				/* If user is found */
				if (user) {

					/* Force close with comment */
					user->close_because(m_argv[1]);

					/* Back-notice op of KILL success */
					m_reply.CMD_NOTICE(m_parent->getNickname(),
							"KILL success !");

				} else { /* User not found */
					m_reply.ERR_NOSUCHNICK(m_argv[0]);
				}

			} else { /* No auth to do what */
				m_reply.ERR_NOPRIVILEGES();
			}

		} else { /* Arguments error */
			m_reply.ERR_NEEDMOREPARAMS("KILL");
		}

		/* Send answer */
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_PING(void) {
	/* Add server prefix */
	m_reply.addPrefix(m_parent->getServername());

	/* Check arguments count */
	if (m_argc == 1) { /* Only target server is specified */

		/* Check target server */
		if (m_argv[0] == m_parent->getServername()) { /* Current server targeted */
			m_reply.CMD_PONG(m_argv[0]); /* Answer with PONG */

		} else { /* Another server (no server to server communcation in this program) */
			m_reply.ERR_NOSUCHSERVER(m_argv[0]); /* Error server not found */
		}

	} else if (m_argc == 2) { /* Target server and deamon specified */

		/* Check target server */
		if (m_argv[0] == m_parent->getServername()) { /* Current server targeted */
			m_reply.CMD_PONG(m_argv[0], m_argv[1]); /* Answer with PONG */

		} else { /* Another server (no server to server communcation in this program) */
			m_reply.ERR_NOSUCHSERVER(m_argv[0]); /* Error server not found */
		}

	} else { /* Other */
		m_reply.ERR_NOORIGIN(); /* Ping error */
	}

	/* Send answer */
	m_parent->write(m_reply.toString());
}

void irc::Request_handler::handle_PONG(void) {
	/* Add server prefix */
	m_reply.addPrefix(m_parent->getServername());

	/* Check arguments count */
	if (m_argc == 2) { /* PING target + deamon */

		/* Check ping target */
		if (m_argv[0] == m_parent->getServername()) { /* This server */

			/* Check ping deamon */
			if (m_argv[1] == m_parent->getLastPingArg()) {

				/* Restart deadline timer */
				m_parent->restartDeadlineTimer();

			} else { /* PONG with unknwon deamon */
				m_reply.ERR_NOORIGIN(); /* Ping error */
				m_parent->write(m_reply.toString());
			}

		} else { /* Another server */
			m_reply.ERR_NOSUCHSERVER(m_argv[0]); /* Error server not found */
			m_parent->write(m_reply.toString());
		}

	} else { /* No deamon or server specified */
		m_reply.ERR_NOORIGIN(); /* Ping error */
		m_parent->write(m_reply.toString());
	}
}

void irc::Request_handler::handle_ERROR(void) {
	/* Check state */
	if (m_parent->getState() == Connection::READY_FOR_MSG) {

		/* Add server prefix */
		m_reply.addPrefix(m_parent->getServername());

		/* Check arguments count */
		if (m_argc == 1) {

			/* Check if user is irc op */
			if (m_parent->isIrcOp()) {

				/* Output error to server stderr */
				std::cerr << "[ERROR] from " << m_parent->getNickname()
						<< ": " << m_argv[0] << std::endl;

				/* Prepare NOTICE message */
				m_reply.CMD_NOTICE(m_parent->getServername(),
						"ERROR from " + m_parent->getNickname() + ": "
								+ m_argv[0]);

				/* Notice all irc ops with notice */
				m_parent->getUsersDatabase().writeToIrcOp(m_reply.toString());

			} else { /* No auth to do what */
				m_reply.ERR_NOPRIVILEGES();
				m_parent->write(m_reply.toString());
			}

		} else { /* Arguments error */
			m_reply.ERR_NEEDMOREPARAMS("ERROR");
			m_parent->write(m_reply.toString());
		}
	}
}

void irc::Request_handler::handle_AWAY(void) {
	// TODO
}

void irc::Request_handler::handle_REHASH(void) {
	// TODO
}

void irc::Request_handler::handle_RESTART(void) {
	// TODO
}

void irc::Request_handler::handle_SUMMON(void) {
	// TODO
}

void irc::Request_handler::handle_USERS(void) {
	// TODO
}

void irc::Request_handler::handle_WALLOPS(void) {
	// TODO
}

void irc::Request_handler::handle_USERHOST(void) {
	// TODO
}

void irc::Request_handler::handle_ISON(void) {
	// TODO
}
