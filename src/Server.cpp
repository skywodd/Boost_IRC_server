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
#include <vector>
#include <boost/bind.hpp>
#include <boost/version.hpp>
#include "Server.hpp"
#include "Connection.hpp"
#include "Configuration.hpp"
#include "Channel_info.hpp"
#include "Reply_generator.hpp"
#include "Debug_log.hpp"

/* Instance initialization */
irc::Server* irc::Server::m_instance = NULL;

irc::Server::Server(const std::string& address, const std::string& port,
		Configuration& configuration) :
		m_io_service(), m_signals(m_io_service), m_acceptor(m_io_service), m_configuration(
				configuration), m_users_database(configuration), m_channels_database(
				configuration), m_since(
				boost::posix_time::second_clock::local_time()) {

	/* Creating a new server instance */
	m_instance = this; // Avoid segfault by storing instance pointer before any usage
	debug::DEBUG_LOG(m_configuration.svdomain, "Creating server instance ...");
	debug::DEBUG_LOG("Startup time",
			boost::posix_time::to_simple_string(m_since));

	/* Register all signals that indicate when the server should exit */
#if BOOST_VERSION > 104700
	debug::DEBUG_LOG(m_configuration.svdomain,
			"Registering signals handler ...");
	m_signals.add(SIGINT);
	m_signals.add(SIGTERM);
#ifdef SIGQUIT
	m_signals.add(SIGQUIT);
#endif
	m_signals.async_wait(boost::bind(&Server::stop, this));
#endif

	/* Resolve and bind TCP address / port for incoming connection accept */
	debug::DEBUG_LOG(m_configuration.svdomain, "Binding address and port ...");
	debug::DEBUG_LOG(m_configuration.svdomain, "Bind address", address);
	debug::DEBUG_LOG(m_configuration.svdomain, "Bind port", port);
	boost::asio::ip::tcp::resolver resolver(m_io_service);
	boost::asio::ip::tcp::resolver::query query(address, port);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	m_acceptor.bind(endpoint);
	m_acceptor.listen();

	/* Start accepting incoming connections */
	debug::DEBUG_LOG(m_configuration.svdomain, "Start listening ...");
	start_accept();
}

irc::Server::~Server(void) {

	/* Destroying server instance */
	debug::DEBUG_LOG(m_configuration.svdomain,
			"Destroying server instance ...");
}

irc::Server* irc::Server::createInstance(const std::string& address,
		const std::string& port, Configuration& configuration) {

	/* Check if another instance already exist */
	if (m_instance == NULL) {

		/* Create a new instance */
		m_instance = new Server(address, port, configuration);

	} else { /* Instance overwrite */

		/* Drop error message */
		std::cerr << "[WARNING] Trying to overwrite Server instance !"
				<< std::endl;
	}

	/* Return the pointer to the current instance */
	return m_instance;
}

irc::Server* irc::Server::getInstance(void) {

	/* Return the pointer to the current instance */
	return m_instance;
}

void irc::Server::killInstance(void) {

	/* Check if instance exist */
	if (m_instance != NULL) {

		/* Delete instance */
		delete m_instance;
		m_instance = NULL;
	}
}

void irc::Server::start_accept(void) {

	/* Create a new connection pointer to handle accept at callback */
	debug::DEBUG_LOG(m_configuration.svdomain,
			"Waiting for incoming connection ...");
	boost::shared_ptr<Connection> new_connection = Connection::create(
			m_io_service);

	/* Register callback on accept */
	m_acceptor.async_accept(new_connection->socket(),
			boost::bind(&Server::handle_accept, this, new_connection,
					boost::asio::placeholders::error));
}

void irc::Server::handle_accept(boost::shared_ptr<Connection> new_connection,
		const boost::system::error_code& error) {

	/* Process the new incoming connection */
	if (!error) {

		/* If no error, start connection */
		new_connection->start();

		/* Check for users count limit */
		if (m_users_database.getUsersCount()
				> m_users_database.getUsersCountLimit()) {

			/* No more place in the users database */
			debug::DEBUG_LOG(m_configuration.svdomain,
					"Incoming connection dropped, no more place");
			new_connection->close();

		} else { /* Users database is not full */

			/* Add connection to the users database */
			debug::DEBUG_LOG(m_configuration.svdomain,
					"Incoming connection added to database");
			m_users_database.add(new_connection);
		}

	} else {

		/* If error, drop debug message */
		debug::DEBUG_LOG(m_configuration.svdomain,
				"Error during processing of incoming connection !");
	}

	/* Start accepting another new connection */
	start_accept();
}

void irc::Server::run(void) {

	/* Starting server */
	debug::DEBUG_LOG(m_configuration.svdomain, "Starting server ...");
	m_io_service.run();
}

void irc::Server::stop(void) {

	/* Stopping io_service */
	debug::DEBUG_LOG(m_configuration.svdomain, "Stopping server ...");

	/* Notice all users of server shutdown */
	Reply_generator notice;
	notice.addPrefix(m_configuration.svdomain);
	notice.CMD_NOTICE(m_configuration.svdomain,
			"WARNING: SERVER IS SHUTTING DOWN NOW !");
	m_users_database.writeToAll(notice.toString());

	/* Stop IO_service manager */
	m_io_service.stop();
}

boost::posix_time::ptime irc::Server::runSince(void) const {

	/* Return the server startup time */
	return m_since;
}

const irc::Configuration& irc::Server::getConfiguration(void) const {

	/* Return a reference to the server configuration */
	return m_configuration;
}

irc::Users_manager& irc::Server::getUsersDatabase(void) {

	/* Return a reference to the users database */
	return m_users_database;
}

irc::Channels_manager& irc::Server::getChannelsDatabase(void) {

	/* Return a reference to the channels database */
	return m_channels_database;
}
