/**
 * @file Configuration.hpp
 * @brief Runtime configuration structure
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This structure is designed to store default configuration of server runtime.\n
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

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

/* Dependencies */
#include <cstddef>
#include <string>
#include <set>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 * @namespace irc
 */
namespace irc {

/**
 * @struct Configuration
 */
struct Configuration {
	/* ----- Server ----- */
	/** Server domain name */
	std::string svdomain;

	/** True if server is password protected */
	bool is_password_protected;

	/** Server password */
	std::set<std::string> server_password;

	/** IRC op user / password */
	std::map<std::string, std::string> server_ircop;

	/** Send MOTD after connection */
	bool send_motd;

	/** Send stats after connection */
	bool send_stats;

	/** MOTD file path */
	std::string motd_filename;

	/* ----- Connection ----- */
	/** Delay between two ping request in seconds */
	int ping_refresh_delay;

	/** Ping request timeout in seconds */
	int ping_timeout_delay;

	/* ----- Users manager ----- */
	/** Maximum number of users stored in the database */
	int nb_users_limit;

	/* ----- Channels manager ----- */
	/** Maximum number of channel stored in the database */
	int nb_channels_limit;

	/* ----- User info ----- */
	/** Limit of joined channel */
	int nb_join_limit;

	/** True if user is IRC op by default */
	bool is_ircop;

	/** True if user can receive wallops messages by default */
	bool is_receiving_wallops;

	/** True if user can receive notice messages by default */
	bool is_receiving_notices;

	/** True if the user is invisible on the channel */
	bool is_invisible;

	/** True if the user is away by default */
	bool is_away;

	/** Default away message */
	std::string away_message;

	/* ----- Channel info ----- */
	/** Maximum number of users on the channel by default */
	int users_limit_per_channel;

	/** True if the channel is private by default */
	bool is_private;

	/** True if the channel is secret by default */
	bool is_secret;

	/** True if the channel is on invitation only by default */
	bool is_invite_only;

	/** True if the topic can be set by op only by default */
	bool topic_setby_op_only;

	/** True if messages outside from the channel cannot be sent by default */
	bool no_outside_msg;

	/** True if the channel is moderated by default */
	bool is_moderated;

	/* ----- Others ----- */
	/** Date/time of server startup */
	boost::posix_time::ptime m_since;
};

}

#endif /* CONFIGURATION_H_ */
