/**
 * @file main.cpp
 * @brief Simple but powerful IRC server written in C++ using Boost library
 * @mainpage 
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This program is a simple but powerful IRC server written in C++ using Boost libraries.\n
 * The code in fully portable between linux, mac and windows !\n
 * \n
 * Please keep in mind that this project is a school project, not a final product !\n
 * Some of the IRC features are not implemented, and will not be (code & forget project).\n
 * \n
 * Please report bug to <skywodd at gmail.com>
 *
 * @remarks This implementation of IRC server does NOT support server-to-server communications.
 * @remarks Take a look at the README.md file for details about implemented IRC commands.
 *
 * @section licence_sec License
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

/* Dependencies */
#include <iostream>
#include "Server.hpp"
#include "Configuration.hpp"

/**
 * Program entry point
 *
 * @param argc CLI arguments count
 * @param argv CLI arguments array
 * @return Error code
 */
int main(int argc, char** argv) {

	/* Try to start the server */
	try {

		/* Check command line arguments */
		if (argc != 3) {

			/* Drop usage message */
			std::cerr << "Usage: Boost_IRC_server <address> <port>\n"
					<< std::endl;

			/* Return with error */
			return 1;
		}

		/* Fill configuration structure */
		// TODO Use Boost configuration library
		irc::Configuration configuration;
		configuration.svdomain = "irc.local";
		configuration.is_password_protected = false;
		configuration.server_ircop["root"] = "toor";
		configuration.send_motd = true;
		configuration.send_stats = true;
		configuration.motd_filename = "motd.txt";
		configuration.ping_refresh_delay = 60;
		configuration.ping_timeout_delay = 120;
		configuration.nb_users_limit = 100;
		configuration.nb_channels_limit = 100;
		configuration.nb_join_limit = 10;
		configuration.is_ircop = false;
		configuration.is_receiving_wallops = true;
		configuration.is_receiving_notices = true;
		configuration.is_invisible = false;
		configuration.is_away = false;
		configuration.users_limit_per_channel = 10;
		configuration.is_private = false;
		configuration.is_secret = false;
		configuration.is_invite_only = false;
		configuration.topic_setby_op_only = true;
		configuration.no_outside_msg = true;
		configuration.is_moderated = false;

		/* Initialize the server */
		irc::Server* server = irc::Server::createInstance(argv[1], argv[2], configuration);

		/* Start the server */
		server->run();

		/* Kill the server */
		server->killInstance();

	} catch (std::exception& e) { /* Catch exception */
		std::cerr << "[ERROR] Got exception : " << e.what() << std::endl;
	}

	/* Exit without error */
	return 0;
}
