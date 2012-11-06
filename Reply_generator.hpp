/**
 * @file Reply_generator.hpp
 * @brief IRC reply string generator
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is designed to craft reply string in IRC format.\n
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

#ifndef REPLY_GENERATOR_H_
#define REPLY_GENERATOR_H_

/* Dependencies */
#include <sstream>
#include <string>

/**
 * @namespace irc
 */
namespace irc {

/**
 * @class Reply_generator
 */
class Reply_generator {
protected:
	/** Output string stream */
	std::ostringstream m_oss;

public:
	/**
	 * Instantiate a new reply generator object
	 */
	Reply_generator();

	/**
	 * Default destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Reply_generator();

	/**
	 * Get the formated string
	 *
	 * @return The formated string
	 */
	std::string toString(void);

	/**
	 * Flush the generator string (for re-use of generator)
	 */
	void flush(void);

	/**
	 * Add prefix to the reply string
	 *
	 * @param prefix Prefix to add before the formated reply string
	 */
	void addPrefix(const std::string& prefix);

	/**
	 * Generate a random ping seed
	 *
	 * @return A random ping seed
	 */
	static std::string generate_seed(void);

	/* ----- IRC COMMANDS ----- */

	void CMD_PASS(const std::string& password);
	void CMD_NICK(const std::string& nickname, const int hopcount = -1);
	void CMD_USER(const std::string& username, const std::string& hostname,
			const std::string& svname, const std::string& realname);
	void CMD_SERVER(const std::string& svname, const int hopcount,
			const std::string& info);
	void CMD_OPER(const std::string& username, const std::string& password);
	void CMD_QUIT(const std::string& message = "");
	void CMD_SQUIT(const std::string& svname, const std::string& comment);
	void CMD_JOIN(const std::string& chan_name,
			const std::string& chan_key = "");
	void CMD_PART(const std::string& chan_name);
	void CMD_MODE(const std::string& target_name, const std::string& mode,
			const int limit = -1, const std::string& user = "",
			const std::string& banmask = "");
	void CMD_TOPIC(const std::string& chan_name, const std::string& topic = "");
	void CMD_NAMES(const std::string& chan_name = "");
	void CMD_LIST(const std::string& chan_name = "", const std::string& svname =
			"");
	void CMD_INVITE(const std::string& nickname, const std::string& chan_name);
	void CMD_KICK(const std::string& chan_name, const std::string& nickname,
			const std::string& comment = "");
	void CMD_VERSION(const std::string& svname = "");
	void CMD_STATS(const std::string& query = "",
			const std::string& svname = "");
	void CMD_LINKS(const std::string& svname = "", const std::string& svmask =
			"");
	void CMD_TIME(const std::string& svname = "");
	void CMD_CONNECT(const std::string& svname, const int port = -1,
			const std::string& remote_svname = "");
	void CMD_TRACE(const std::string& svname = "");
	void CMD_ADMIN(const std::string& svname = "");
	void CMD_INFO(const std::string& svname = "");
	void CMD_NOTICE(const std::string& target_name, const std::string& message);
	void CMD_PRIVMSG(const std::string& nickname, const std::string& message);
	void CMD_WHO(const std::string& nickname = "", const bool only_ops = false);
	void CMD_WHOIS(const std::string& nickname, const std::string& svname = "");
	void CMD_WHOWAS(const std::string& nickname, const int count = -1,
			const std::string& svname = "");
	void CMD_KILL(const std::string& nickname, const std::string& comment);
	void CMD_PING(const std::string& svname1, const std::string& svname2 = "");
	void CMD_PONG(const std::string& deamon1, const std::string& deamon2 = "");
	void CMD_ERROR(const std::string& error_msg);
	void CMD_AWAY(const std::string& message);
	void CMD_REHASH(void);
	void CMD_RESTART(void);
	void CMD_SUMMON(const std::string& nickname,
			const std::string& svname = "");
	void CMD_USERS(const std::string& svname = "");
	void CMD_WALLOPS(const std::string& message);
	void CMD_USERHOST(const std::string& nickname);
	void CMD_ISON(const std::string& nickname);

	/* ----- IRC REPLY ----- */

	void RPL_NONE(void);
	void RPL_USERHOST(const std::string& reply);
	void RPL_ISON(const std::string& nickname);
	void RPL_AWAY(const std::string& nickname, const std::string& away_msg);
	void RPL_UNAWAY(void);
	void RPL_NOWAWAY(void);
	void RPL_WHOISUSER(const std::string& nickname, const std::string& username,
			const std::string& hostname, const std::string& realname);
	void RPL_WHOISSERVER(const std::string& nickname, const std::string& svname,
			const std::string& svinfo);
	void RPL_WHOISOPERATOR(const std::string& nickname);
	void RPL_WHOISIDLE(const std::string& nickname, const int idle_time);
	void RPL_ENDOFWHOIS(const std::string& nickname);
	void RPL_WHOISCHANNELS(const std::string& nickname, const bool is_op,
			const bool can_speak, const std::string& chan_name);
	void RPL_WHOWASUSER(const std::string& nickname,
			const std::string& username, const std::string& hostname,
			const std::string& realname);
	void RPL_ENDOFWHOAS(const std::string& nickname);
	void RPL_LISTSTART(void); // WTF no params ?
	void RPL_LIST(const std::string& chan_name, const bool is_visible,
			const std::string& topic); // WTF is <# visible>
	void RPL_LISTEND(void);
	void RPL_CHANNELMODEIS(const std::string& chan_name,
			const std::string& mode, const std::string& params);
	void RPL_NOTOPIC(const std::string& chan_name);
	void RPL_TOPIC(const std::string& chan_name, const std::string& topic);
	void RPL_INVITING(const std::string& chan_name,
			const std::string& nickname);
	void RPL_SUMMONING(const std::string& nickname);
	void RPL_VERSION(const std::string& version, const std::string& debug_level,
			const std::string& svname, const std::string& comments);
	void RPL_WHOREPLY(const std::string& chan_name, const std::string& username,
			const std::string& hostname, const std::string& svname,
			const std::string& nickname, const std::string& mode,
			const int hopcount, const std::string& realname);
	void RPL_ENDOFWHO(const std::string& nickname);
	void RPL_NAMREPLY(const std::string& chan_name, const bool is_op,
			const bool can_speak, const std::string& nickname);
	void RPL_ENDOFNAMES(const std::string& chan_name);
	void RPL_LINKS(const std::string& mask, const std::string& svname,
			const int hopcount, const std::string& svinfo);
	void RPL_ENDOFLINKS(const std::string& mask);
	void RPL_BANLIST(const std::string& chan_name, const std::string& banmask);
	void RPL_ENDOFBANLIST(const std::string& chan_name);
	void RPL_INFO(const std::string& str);
	void RPL_ENDOFINFO(void);
	void RPL_MOTDSTART(const std::string& svname);
	void RPL_MOTD(const std::string& str);
	void RPL_ENDOFMOTD(void);
	void RPL_YOUREOPER(void);
	void RPL_REHASHING(const std::string& conf_file);
	void RPL_TIME(const std::string& svname, const std::string& time_str);
	void RPL_USERSSTART(void);
	void RPL_USERS(void);
	void RPL_ENDOFUSERS(void);
	void RPL_NOUSERS(void);
	// RPL_STATSxxxx
	void RPL_UMODEIS(const std::string& mode);
	void RPL_LUSERCLIENT(const int users_count, const int users_invisible);
	void RPL_LUSEROP(const int op_cout);
	void RPL_LUSERUNKNOWN(const int conn_count);
	void RPL_LUSERCHANNELS(const int channels_count);
	void RPL_LUSERME(const int clients_count, const int servers_count);
	void RPL_ADMINME(const std::string& svname);
	void RPL_ADMINLOC1(const std::string& admin_location);
	void RPL_ADMINLOC2(const std::string& admin_location);
	void RPL_ADMINMAIL(const std::string& admin_mail);

	/* ----- IRC ERRORS ----- */

	void ERR_NOSUCHNICK(const std::string& nickname);
	void ERR_NOSUCHSERVER(const std::string& svname);
	void ERR_NOSUCHCHANNEL(const std::string& chan_name);
	void ERR_CANNOTSENDTOCHAN(const std::string& chan_name);
	void ERR_TOOMANYCHANNELS(const std::string& chan_name);
	void ERR_WASNOSUCHNICK(const std::string& nickname);
	void ERR_TOOMANYTARGETS(const std::string& target);
	void ERR_NOORIGIN(void);
	void ERR_NORECIPIENT(const std::string& command);
	void ERR_NOTEXTTOSEND(void);
	void ERR_NOTOPLEVEL(const std::string& mask);
	void ERR_WILDTOPLEVEL(const std::string& mask);
	void ERR_UNKNOWNCOMMAND(const std::string& command);
	void ERR_NOMOTD(void);
	void ERR_NOADMININFO(const std::string& svname);
	void ERR_FILEERROR(const std::string& fileop, const std::string& filename);
	void ERR_NONICKNAMEGIVEN(void);
	void ERR_ERRONEUSNICKNAME(const std::string& nickname);
	void ERR_NICKNAMEINUSE(const std::string& nickname);
	void ERR_NICKCOLLISION(const std::string& nickname);
	void ERR_USERNOTINCHANNEL(const std::string& nickname,
			const std::string& chan_name);
	void ERR_NOTONCHANNEL(const std::string& chan_name);
	void ERR_USERONCHANNEL(const std::string& nickname,
			const std::string& chan_name);
	void ERR_NOLOGIN(const std::string& nickname);
	void ERR_SUMMONDISABLED(void);
	void ERR_USERSDISABLED(void);
	void ERR_NOTREGISTERED(void);
	void ERR_NEEDMOREPARAMS(const std::string& command);
	void ERR_ALREADYREGISTRED(void);
	void ERR_NOPERMFORHOST(void);
	void ERR_PASSWDMISMATCH(void);
	void ERR_YOUREBANNEDCREEP(void);
	void ERR_KEYSET(const std::string& chan_name);
	void ERR_CHANNELISFULL(const std::string& chan_name);
	void ERR_UNKNOWNMODE(const char chr_mode);
	void ERR_INVITEONLYCHAN(const std::string& chan_name);
	void ERR_BANNEDFROMCHAN(const std::string& chan_name);
	void ERR_BADCHANNELKEY(const std::string& chan_name);
	void ERR_NOPRIVILEGES(void);
	void ERR_CHANOPRIVSNEEDED(const std::string& chan_name);
	void ERR_CANNTKILLSERVER(void);
	void ERR_NOOPERHOST(void);
	void ERR_UMODUUNKNOWNFLAG(void);
	void ERR_USERSDONTMATCH(void);

	/* ----- SPECIAL ----- */
	void RPL_CUSTOM(const std::string& number, const std::string& payload);
};

} /* namespace irc */

#endif /* REPLY_GENERATOR_HPP_ */
