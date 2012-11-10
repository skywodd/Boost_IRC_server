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
#include <cstdlib>
#include "Reply_generator.hpp"

irc::Reply_generator::Reply_generator() :
		m_oss() {
}

irc::Reply_generator::~Reply_generator() {
}

std::string irc::Reply_generator::toString(void) {

	/* Return the formated string */
	return m_oss.str();
}

void irc::Reply_generator::flush(void) {

	/* Flush the string buffer */
	m_oss.clear();
	m_oss.str("");
}

void irc::Reply_generator::addPrefix(const std::string& prefix) {

	/* Add prefix to the generator string */
	m_oss << ":" << prefix << " ";
}

std::string irc::Reply_generator::generate_seed(void) {

	/* Craft ping seed from random value */
	std::ostringstream oss;
	oss << "ping_" << std::hex << rand();

	/* Return ping seed */
	return oss.str();
}

/* ----- CMD ----- */

void irc::Reply_generator::CMD_PASS(const std::string& password) {
	m_oss << "PASS :" << password << "\r\n";
}

void irc::Reply_generator::CMD_NICK(const std::string& nickname,
		const int hopcount) {
	m_oss << "NICK " << nickname;
	if (hopcount >= 0)
		m_oss << " " << hopcount;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_USER(const std::string& username,
		const std::string& hostname, const std::string& svname,
		const std::string& realname) {
	m_oss << "USER " << username << " " << hostname << " " << svname << " :"
			<< realname << "\r\n";
}

void irc::Reply_generator::CMD_SERVER(const std::string& svname,
		const int hopcount, const std::string& info) {
	m_oss << "SERVER " << svname << " " << hopcount << " :" << info << "\r\n";
}

void irc::Reply_generator::CMD_OPER(const std::string& username,
		const std::string& password) {
	m_oss << "OPER " << username << " :" << password << "\r\n";
}

void irc::Reply_generator::CMD_QUIT(const std::string& message) {
	m_oss << "QUIT";
	if (message != "")
		m_oss << " :" << message;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_SQUIT(const std::string& svname,
		const std::string& comment) {
	m_oss << "SQUIT " << svname << " :" << comment << "\r\n";
}

void irc::Reply_generator::CMD_JOIN(const std::string& chan_name,
		const std::string& chan_key) {
	m_oss << "JOIN " << chan_name;
	if (chan_key != "")
		m_oss << " " << chan_key;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_PART(const std::string& chan_name) {
	m_oss << "PART " << chan_name << "\r\n";
}

void irc::Reply_generator::CMD_MODE(const std::string& target_name,
		const std::string& mode, const int limit, const std::string& user,
		const std::string& banmask) {
	m_oss << "MODE " << target_name << " " << mode;
	if (limit >= 0)
		m_oss << " " << limit;
	if (user != "")
		m_oss << " " << user;
	if (banmask != "")
		m_oss << " " << banmask;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_TOPIC(const std::string& chan_name,
		const std::string& topic) {
	m_oss << "TOPIC " << chan_name;
	if (topic != "")
		m_oss << " :" << topic;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_NAMES(const std::string& chan_name) {
	m_oss << "NAMES";
	if (chan_name != "")
		m_oss << " " << chan_name;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_LIST(const std::string& chan_name,
		const std::string& svname) {
	m_oss << "LIST";
	if (chan_name != "") {
		m_oss << " " << chan_name;
		if (svname != "")
			m_oss << " " << svname;
	}
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_INVITE(const std::string& nickname,
		const std::string& chan_name) {
	m_oss << "INVITE " << nickname << " " << chan_name << "\r\n";
}

void irc::Reply_generator::CMD_KICK(const std::string& chan_name,
		const std::string& nickname, const std::string& comment) {
	m_oss << "KICK " << chan_name << " " << nickname;
	if (comment != "")
		m_oss << " :" << comment;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_VERSION(const std::string& svname) {
	m_oss << "VERSION";
	if (svname != "")
		m_oss << " " << svname;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_STATS(const std::string& query,
		const std::string& svname) {
	m_oss << "STATS";
	if (query != "") {
		m_oss << " " << query;
		if (svname != "")
			m_oss << " " << svname;
	}
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_LINKS(const std::string& svname,
		const std::string& svmask) {
	m_oss << "LINKS";
	if (svname != "") {
		m_oss << " " << svname;
		if (svmask != "")
			m_oss << " " << svmask;
	}
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_TIME(const std::string& svname) {
	m_oss << "TIME";
	if (svname != "")
		m_oss << " " << svname;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_CONNECT(const std::string& svname,
		const int port, const std::string& remote_svname) {
	m_oss << "CONNECT " << svname;
	if (port >= 0) {
		m_oss << " " << port;
		if (remote_svname != "")
			m_oss << " " << remote_svname;
	}
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_TRACE(const std::string& svname) {
	m_oss << "TRACE";
	if (svname != "")
		m_oss << " " << svname;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_ADMIN(const std::string& svname) {
	m_oss << "ADMIN";
	if (svname != "")
		m_oss << " " << svname;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_INFO(const std::string& svname) {
	m_oss << "INFO";
	if (svname != "")
		m_oss << " " << svname;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_PRIVMSG(const std::string& target_name,
		const std::string& message) {
	m_oss << "PRIVMSG " << target_name << " :" << message << "\r\n";
}

void irc::Reply_generator::CMD_NOTICE(const std::string& nickname,
		const std::string& message) {
	m_oss << "NOTICE " << nickname << " :" << message << "\r\n";
}

void irc::Reply_generator::CMD_WHO(const std::string& nickname,
		const bool only_ops) {
	m_oss << "WHO";
	if (nickname != "") {
		m_oss << " " << nickname;
		if (only_ops)
			m_oss << " o";
	}
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_WHOIS(const std::string& nickname,
		const std::string& svname) {
	m_oss << "WHOIS";
	if (svname != "")
		m_oss << " " << svname;
	m_oss << " " << nickname << "\r\n";
}

void irc::Reply_generator::CMD_WHOWAS(const std::string& nickname,
		const int count, const std::string& svname) {
	m_oss << "WHOAS " << nickname;
	if (count >= 0) {
		m_oss << " " << count;
		if (svname != "")
			m_oss << " " << svname;
	}
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_KILL(const std::string& nickname,
		const std::string& comment) {
	m_oss << "KILL " << nickname << " :" << comment << "\r\n";
}

void irc::Reply_generator::CMD_PING(const std::string& svname1,
		const std::string& svname2) {
	m_oss << "PING " << svname1;
	if (svname2 != "")
		m_oss << " " << svname2;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_PONG(const std::string& deamon1,
		const std::string& deamon2) {
	m_oss << "PONG " << deamon1;
	if (deamon2 != "")
		m_oss << " " << deamon2;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_ERROR(const std::string& error_msg) {
	m_oss << "ERROR :" << error_msg << "\r\n";
}

void irc::Reply_generator::CMD_AWAY(const std::string& message) {
	m_oss << "AWAY";
	if (message != "")
		m_oss << " :" << message;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_REHASH(void) {
	m_oss << "REHASH" << "\r\n";
}

void irc::Reply_generator::CMD_RESTART(void) {
	m_oss << "RESTART" << "\r\n";
}

void irc::Reply_generator::CMD_SUMMON(const std::string& nickname,
		const std::string& svname) {
	m_oss << "SUMON " << nickname;
	if (svname != "")
		m_oss << " " << svname;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_USERS(const std::string& svname) {
	m_oss << "USERS";
	if (svname != "")
		m_oss << " " << svname;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_WALLOPS(const std::string& message) {
	m_oss << "WALLOPS :" << message << "\r\n";
}

void irc::Reply_generator::CMD_USERHOST(const std::string& nickname) {
	m_oss << "USERHOST";
	if (nickname != "")
		m_oss << " " << nickname;
	m_oss << "\r\n";
}

void irc::Reply_generator::CMD_ISON(const std::string& nickname) {
	m_oss << "ISON";
	if (nickname != "")
		m_oss << " " << nickname;
	m_oss << "\r\n";
}

/* ----- RPL ----- */

void irc::Reply_generator::RPL_NONE(void) {
	m_oss << "300" << "\r\n";
}

void irc::Reply_generator::RPL_USERHOST(const std::string& reply) {
	m_oss << "302 :" << reply << "\r\n";
}

void irc::Reply_generator::RPL_ISON(const std::string& nickname) {
	m_oss << "303 :" << nickname << "\r\n";
}

void irc::Reply_generator::RPL_AWAY(const std::string& nickname,
		const std::string& away_msg) {
	m_oss << "301 " << nickname << " :" << away_msg << "\r\n";
}

void irc::Reply_generator::RPL_UNAWAY(void) {
	m_oss << "305 :You are no longer marked as being away" << "\r\n";
}

void irc::Reply_generator::RPL_NOWAWAY(void) {
	m_oss << "306 :You have been marked as being away" << "\r\n";
}

void irc::Reply_generator::RPL_WHOISUSER(const std::string& nickname,
		const std::string& username, const std::string& hostname,
		const std::string& realname) {
	m_oss << "311 " << nickname << " " << username << " " << hostname << " * :"
			<< realname << "\r\n";
}

void irc::Reply_generator::RPL_WHOISSERVER(const std::string& nickname,
		const std::string& svname, const std::string& svinfo) {
	m_oss << "312 " << nickname << " " << svname << " :" << svinfo << "\r\n";
}

void irc::Reply_generator::RPL_WHOISOPERATOR(const std::string& nickname) {
	m_oss << "313 " << nickname << " :is an IRC operator" << "\r\n";
}

void irc::Reply_generator::RPL_WHOISIDLE(const std::string& nickname,
		const int idle_time) {
	m_oss << "317 " << nickname << " " << idle_time << " :seconds idle"
			<< "\r\n";
}

void irc::Reply_generator::RPL_ENDOFWHOIS(const std::string& nickname) {
	m_oss << "318 " << nickname << " :End of /WHOIS list" << "\r\n";
}

void irc::Reply_generator::RPL_WHOISCHANNELS(const std::string& nickname,
		const bool is_op, const bool can_speak, const std::string& chan_name) {
	m_oss << "319 " << nickname << " :";
	if (is_op)
		m_oss << '@';
	else if (can_speak)
		m_oss << '+';
	m_oss << chan_name << "\r\n";
}

void irc::Reply_generator::RPL_WHOWASUSER(const std::string& nickname,
		const std::string& username, const std::string& hostname,
		const std::string& realname) {
	m_oss << "314 " << nickname << " " << username << " " << hostname << " * :"
			<< realname << "\r\n";
}

void irc::Reply_generator::RPL_ENDOFWHOAS(const std::string& nickname) {
	m_oss << "369 " << nickname << " :End of WHOWAS" << "\r\n";
}

void irc::Reply_generator::RPL_LISTSTART(void) {
	m_oss << "321 " << "Channel :Users  Name" << "\r\n";
}

void irc::Reply_generator::RPL_LIST(const std::string& chan_name,
		int visible_users, const std::string& topic) {
	m_oss << "322 " << chan_name << " " << visible_users << " :" << topic
			<< "\r\n";
}

void irc::Reply_generator::RPL_LISTEND(void) {
	m_oss << "323 :End of /LIST" << "\r\n";
}

void irc::Reply_generator::RPL_CHANNELMODEIS(const std::string& chan_name,
		const std::string& mode, const std::string& params) {
	m_oss << "324 " << chan_name << " " << mode << " " << params << "\r\n";
}

void irc::Reply_generator::RPL_NOTOPIC(const std::string& chan_name) {
	m_oss << "331 " << chan_name << " :No topic is set" << "\r\n";
}

void irc::Reply_generator::RPL_TOPIC(const std::string& chan_name,
		const std::string& topic) {
	m_oss << "332 " << chan_name << " :" << topic << "\r\n";
}

void irc::Reply_generator::RPL_INVITING(const std::string& chan_name,
		const std::string& nickname) {
	m_oss << "341 " << chan_name << " " << nickname << "\r\n";
}

void irc::Reply_generator::RPL_SUMMONING(const std::string& nickname) {
	m_oss << "342 " << nickname << " :Summoning user to IRC" << "\r\n";
}

void irc::Reply_generator::RPL_VERSION(const std::string& version,
		const std::string& debug_level, const std::string& svname,
		const std::string& comments) {
	m_oss << "351 " << version << "." << debug_level << " " << svname << " :"
			<< comments << "\r\n";
}

void irc::Reply_generator::RPL_WHOREPLY(const std::string& chan_name,
		const std::string& username, const std::string& hostname,
		const std::string& svname, const std::string& nickname,
		const std::string& mode, const int hopcount,
		const std::string& realname) {
	m_oss << "352 " << chan_name << " " << username << " " << hostname << " "
			<< svname << " " << nickname << " " << mode << " :" << hopcount
			<< " " << realname << "\r\n";
}

void irc::Reply_generator::RPL_ENDOFWHO(const std::string& nickname) {
	m_oss << "315 " << nickname << " :End of /WHO list" << "\r\n";
}

void irc::Reply_generator::RPL_NAMREPLY(const std::string& chan_name,
		const bool is_op, const bool can_speak, const std::string& nickname) {
	m_oss << "353 " << chan_name << " :";
	if (is_op)
		m_oss << '@';
	else if (can_speak)
		m_oss << '+';
	m_oss << nickname << "\r\n";
}

void irc::Reply_generator::RPL_ENDOFNAMES(const std::string& chan_name) {
	m_oss << "366 " << chan_name << " :End of /NAMES list" << "\r\n";
}

void irc::Reply_generator::RPL_LINKS(const std::string& mask,
		const std::string& svname, const int hopcount,
		const std::string& svinfo) {
	m_oss << "364 " << mask << " " << svname << " :" << hopcount << " "
			<< svinfo << "\r\n";
}

void irc::Reply_generator::RPL_ENDOFLINKS(const std::string& mask) {
	m_oss << "367 " << mask << " :End of /LINKS list" << "\r\n";
}

void irc::Reply_generator::RPL_BANLIST(const std::string& chan_name,
		const std::string& banmask) {
	m_oss << "367 " << chan_name << " " << banmask << "\r\n";
}

void irc::Reply_generator::RPL_ENDOFBANLIST(const std::string& chan_name) {
	m_oss << "368 " << chan_name << " :End of channel ban list" << "\r\n";
}

void irc::Reply_generator::RPL_INFO(const std::string& str) {
	m_oss << "371 : " << str << "\r\n";
}

void irc::Reply_generator::RPL_ENDOFINFO(void) {
	m_oss << "374 :End of /INFO list" << "\r\n";
}

void irc::Reply_generator::RPL_MOTDSTART(const std::string& svname) {
	m_oss << "375 :- " << svname << " Message of the day -  " << "\r\n";
}

void irc::Reply_generator::RPL_MOTD(const std::string& str) {
	m_oss << "372 :- " << str << "\r\n";
}

void irc::Reply_generator::RPL_ENDOFMOTD(void) {
	m_oss << "376 :End of /MOTD command" << "\r\n";
}

void irc::Reply_generator::RPL_YOUREOPER(void) {
	m_oss << "381 :You are now an IRC operator" << "\r\n";
}

void irc::Reply_generator::RPL_REHASHING(const std::string& conf_file) {
	m_oss << "382 " << conf_file << " :Rehashing" << "\r\n";
}

void irc::Reply_generator::RPL_TIME(const std::string& svname,
		const std::string& time_str) {
	m_oss << "391 " << svname << " :" << time_str << "\r\n";
}

void irc::Reply_generator::RPL_USERSSTART(void) {
	m_oss << "392 :UserID   Terminal  Host" << "\r\n";
}

void irc::Reply_generator::RPL_USERS(void) {
	m_oss << "393 :%-8s %-9s %-8s" << "\r\n";
}

void irc::Reply_generator::RPL_ENDOFUSERS(void) {
	m_oss << "394 :End of users" << "\r\n";
}

void irc::Reply_generator::RPL_NOUSERS(void) {
	m_oss << "395 :Nobody logged in" << "\r\n";
}

// RPL_TRACExxxxx

void irc::Reply_generator::RPL_UMODEIS(const std::string& mode) {
	m_oss << "221 " << mode << "\r\n";
}

void irc::Reply_generator::RPL_LUSERCLIENT(const int users_count,
		const int users_invisible, const int servers_count) {
	m_oss << "251 :There are " << users_count << " users and " << users_invisible
			<< " invisible on " << servers_count << " servers" << "\r\n";
}

void irc::Reply_generator::RPL_LUSEROP(const int op_cout) {
	m_oss << "252 " << op_cout << " :operator(s) online" << "\r\n";
}

void irc::Reply_generator::RPL_LUSERUNKNOWN(const int conn_count) {
	m_oss << "253 " << conn_count << " :unknown connection(s)" << "\r\n";
}

void irc::Reply_generator::RPL_LUSERCHANNELS(const int channels_count) {
	m_oss << "254 " << channels_count << " :channels formed" << "\r\n";
}

void irc::Reply_generator::RPL_LUSERME(const int clients_count,
		const int servers_count) {
	m_oss << "255 :I have " << clients_count << " clients and " << servers_count
			<< " servers" << "\r\n";
}

void irc::Reply_generator::RPL_ADMINME(const std::string& svname) {
	m_oss << "256 " << svname << " :Administrative info" << "\r\n";
}

void irc::Reply_generator::RPL_ADMINLOC1(const std::string& admin_location) {
	m_oss << "257 :" << admin_location << "\r\n";
}

void irc::Reply_generator::RPL_ADMINLOC2(const std::string& admin_location) {
	m_oss << "258 :" << admin_location << "\r\n";
}

void irc::Reply_generator::RPL_ADMINMAIL(const std::string& admin_mail) {
	m_oss << "259 :" << admin_mail << "\r\n";
}

/* ----- ERR ----- */

void irc::Reply_generator::ERR_NOSUCHNICK(const std::string& nickname) {
	m_oss << "401 " << nickname << " :No such nick/channel" << "\r\n";
}

void irc::Reply_generator::ERR_NOSUCHSERVER(const std::string& svname) {
	m_oss << "402 " << svname << " :No such server" << "\r\n";
}

void irc::Reply_generator::ERR_NOSUCHCHANNEL(const std::string& chan_name) {
	m_oss << "403 " << chan_name << " :No such channel" << "\r\n";
}

void irc::Reply_generator::ERR_CANNOTSENDTOCHAN(const std::string& chan_name) {
	m_oss << "404 " << chan_name << " :Cannot send to channel" << "\r\n";
}

void irc::Reply_generator::ERR_TOOMANYCHANNELS(const std::string& chan_name) {
	m_oss << "405 " << chan_name << " :You have joined too many channels"
			<< "\r\n";
}

void irc::Reply_generator::ERR_WASNOSUCHNICK(const std::string& nickname) {
	m_oss << "406 " << nickname << " :There was no such nickname" << "\r\n";
}

void irc::Reply_generator::ERR_TOOMANYTARGETS(const std::string& target) {
	m_oss << "407 " << target << " :Duplicate recipients. No message delivered"
			<< "\r\n";
}

void irc::Reply_generator::ERR_NOORIGIN(void) {
	m_oss << "409 :No origin specified" << "\r\n";
}

void irc::Reply_generator::ERR_NORECIPIENT(const std::string& command) {
	m_oss << "411 :No recipient given (" << command << ")" << "\r\n";
}

void irc::Reply_generator::ERR_NOTEXTTOSEND(void) {
	m_oss << "412 :No text to send" << "\r\n";
}

void irc::Reply_generator::ERR_NOTOPLEVEL(const std::string& mask) {
	m_oss << "413 " << mask << " :No toplevel domain specified" << "\r\n";
}

void irc::Reply_generator::ERR_WILDTOPLEVEL(const std::string& mask) {
	m_oss << "414 " << mask << " :Wildcard in toplevel domain" << "\r\n";
}

void irc::Reply_generator::ERR_UNKNOWNCOMMAND(const std::string& command) {
	m_oss << "421 " << command << " :Unknown command" << "\r\n";
}

void irc::Reply_generator::ERR_NOMOTD(void) {
	m_oss << "422 :MOTD File is missing" << "\r\n";
}

void irc::Reply_generator::ERR_NOADMININFO(const std::string& svname) {
	m_oss << "423 " << svname << " :No administrative info available" << "\r\n";
}

void irc::Reply_generator::ERR_FILEERROR(const std::string& fileop,
		const std::string& filename) {
	m_oss << "424 :File error doing " << fileop << " on " << filename << "\r\n";
}

void irc::Reply_generator::ERR_NONICKNAMEGIVEN(void) {
	m_oss << "431 :No nickname given" << "\r\n";
}

void irc::Reply_generator::ERR_ERRONEUSNICKNAME(const std::string& nickname) {
	m_oss << "432 " << nickname << " :Erroneus nickname" << "\r\n";
}

void irc::Reply_generator::ERR_NICKNAMEINUSE(const std::string& nickname) {
	m_oss << "433 " << nickname << " :Nickname is already in use" << "\r\n";
}

void irc::Reply_generator::ERR_NICKCOLLISION(const std::string& nickname) {
	m_oss << "436 " << nickname << " :Nickname collision KILL" << "\r\n";
}

void irc::Reply_generator::ERR_USERNOTINCHANNEL(const std::string& nickname,
		const std::string& chan_name) {
	m_oss << "441 " << nickname << " " << chan_name
			<< " :They aren't on that channel" << "\r\n";
}

void irc::Reply_generator::ERR_NOTONCHANNEL(const std::string& chan_name) {
	m_oss << "442 " << chan_name << " :You're not on that channel" << "\r\n";
}

void irc::Reply_generator::ERR_USERONCHANNEL(const std::string& nickname,
		const std::string& chan_name) {
	m_oss << "443 " << nickname << " " << chan_name << " :is already on channel"
			<< "\r\n";
}

void irc::Reply_generator::ERR_NOLOGIN(const std::string& nickname) {
	m_oss << "444 " << nickname << " :User not logged in" << "\r\n";
}

void irc::Reply_generator::ERR_SUMMONDISABLED(void) {
	m_oss << "445 :SUMMON has been disabled" << "\r\n";
}

void irc::Reply_generator::ERR_USERSDISABLED(void) {
	m_oss << "446 :USERS has been disabled" << "\r\n";
}

void irc::Reply_generator::ERR_NOTREGISTERED(void) {
	m_oss << "451 :You have not registered" << "\r\n";
}

void irc::Reply_generator::ERR_NEEDMOREPARAMS(const std::string& command) {
	m_oss << "461 " << command << " :Not enough parameters" << "\r\n";
}

void irc::Reply_generator::ERR_ALREADYREGISTRED(void) {
	m_oss << "462 :You may not reregister" << "\r\n";
}

void irc::Reply_generator::ERR_NOPERMFORHOST(void) {
	m_oss << "463 :Your host isn't among the privileged" << "\r\n";
}

void irc::Reply_generator::ERR_PASSWDMISMATCH(void) {
	m_oss << "464 :Password incorrect" << "\r\n";
}

void irc::Reply_generator::ERR_YOUREBANNEDCREEP(void) {
	m_oss << "465 :You are banned from this server" << "\r\n";
}

void irc::Reply_generator::ERR_KEYSET(const std::string& chan_name) {
	m_oss << "467 " << chan_name << " :Channel key already set" << "\r\n";
}

void irc::Reply_generator::ERR_CHANNELISFULL(const std::string& chan_name) {
	m_oss << "471 " << chan_name << " :Cannot join channel (+l)" << "\r\n";
}

void irc::Reply_generator::ERR_UNKNOWNMODE(const char chr_mode) {
	m_oss << "472 " << chr_mode << " :is unknown mode char to me" << "\r\n";
}

void irc::Reply_generator::ERR_INVITEONLYCHAN(const std::string& chan_name) {
	m_oss << "473 " << chan_name << " :Cannot join channel (+i)" << "\r\n";
}

void irc::Reply_generator::ERR_BANNEDFROMCHAN(const std::string& chan_name) {
	m_oss << "474 " << chan_name << " :Cannot join channel (+b)" << "\r\n";
}

void irc::Reply_generator::ERR_BADCHANNELKEY(const std::string& chan_name) {
	m_oss << "475 " << chan_name << " :Cannot join channel (+k)" << "\r\n";
}

void irc::Reply_generator::ERR_NOPRIVILEGES(void) {
	m_oss << "481 :Permission Denied- You're not an IRC operator" << "\r\n";
}

void irc::Reply_generator::ERR_CHANOPRIVSNEEDED(const std::string& chan_name) {
	m_oss << "482 " << chan_name << " :You're not channel operator" << "\r\n";
}

void irc::Reply_generator::ERR_CANNTKILLSERVER(void) {
	m_oss << "483 :You cant kill a server!" << "\r\n";
}

void irc::Reply_generator::ERR_NOOPERHOST(void) {
	m_oss << "491 :No O-lines for your host" << "\r\n";
}

void irc::Reply_generator::ERR_UMODUUNKNOWNFLAG(void) {
	m_oss << "501 :Unknown MODE flag" << "\r\n";
}

void irc::Reply_generator::ERR_USERSDONTMATCH(void) {
	m_oss << "502 :Cant change mode for other users" << "\r\n";
}

/* ----- Others ----- */

void irc::Reply_generator::RPL_CUSTOM(const std::string& number,
		const std::string& payload) {
	m_oss << number << " " << payload << "\r\n";
}
