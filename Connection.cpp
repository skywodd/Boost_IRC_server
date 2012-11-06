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
#include "Connection.hpp"
#include "Configuration.hpp"
#include "Channels_manager.hpp"
#include "Channel_info.hpp"
#include "Users_manager.hpp"
#include "Request_handler.hpp"
#include "Reply_generator.hpp"
#include "Debug_log.hpp"

irc::Connection::Connection(boost::asio::io_service& io_service,
		Users_manager& users_database, Channels_manager& channels_database,
		const Configuration& configuration) :
		User_info(configuration), m_configuration(configuration), m_cycle_ping_timer(
				io_service,
				boost::posix_time::seconds(configuration.ping_refresh_delay)), m_dead_ping_timer(
				io_service,
				boost::posix_time::seconds(configuration.ping_timeout_delay)), m_socket(
				io_service), m_buffer(), m_users_database(users_database), m_channels_database(
				channels_database), m_idle_time(
				boost::posix_time::second_clock::local_time()), m_ping_arg(), m_handler(
				this) {

	/* Instanciate new user */
	debug::DEBUG_LOG(m_nickname, "Instantiate a new Connection object ...");
	debug::DEBUG_LOG(m_nickname, "Ping check (seconds)",
			configuration.ping_refresh_delay);
	debug::DEBUG_LOG(m_nickname, "Ping timeout (seconds)",
			configuration.ping_timeout_delay);

	/* No server-to-server communication in this program */
	m_servername = configuration.svdomain;
}

boost::shared_ptr<irc::Connection> irc::Connection::create(
		boost::asio::io_service& io_service, Users_manager& users_database,
		Channels_manager& channels_database,
		const Configuration& configuration) {

	/* Create a new Connection shared_ptr */
	return boost::shared_ptr<irc::Connection>(
			new Connection(io_service, users_database, channels_database,
					configuration));
}

irc::Connection::~Connection(void) {

	/* Destroy connection object */
	debug::DEBUG_LOG(m_nickname, "Destroying connection ...");
}

void irc::Connection::handle_read(const boost::system::error_code& error,
		const std::size_t bytes_transferred) {

	/* Check for error */
	if (!error) {
		debug::DEBUG_LOG(m_nickname, "Processing read operation ...");

		/* Processing read operation */
		m_handler.handle_request();

		/* Restart callback handler for next read operation */
		debug::DEBUG_LOG(m_nickname, "Waiting for next read operation ...");
		boost::asio::async_read_until(m_socket, m_buffer, "\r\n",
				boost::bind(&Connection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));

	} else {

		/* If error, drop debug message */
		debug::DEBUG_LOG(m_nickname,
				"Error during process of read operation !");

		/* Clean database and notice other users */
		close_because("Connection reset by peer");
	}
}

void irc::Connection::handle_write(const boost::system::error_code& error) {

	/* Check for error */
	if (!error) {
		debug::DEBUG_LOG(m_nickname, "Procesing write operation ...");
	}
}

void irc::Connection::handle_ping_refresh(
		const boost::system::error_code& error) {

	/* Check for error */
	if (!error) {
		debug::DEBUG_LOG(m_nickname, "Sending ping request ...");

		/* Send ping request */
		Reply_generator reply;
		m_ping_arg = Reply_generator::generate_seed();
		reply.addPrefix(m_servername);
		reply.CMD_PING(m_servername, m_ping_arg);
		write(reply.toString());

		/* Restart ping timer */
		debug::DEBUG_LOG(m_nickname, "Restart ping timer ...");
		m_cycle_ping_timer.expires_from_now(
				boost::posix_time::seconds(m_configuration.ping_refresh_delay));
		m_cycle_ping_timer.async_wait(
				boost::bind(&Connection::handle_ping_refresh,
						shared_from_this(), boost::asio::placeholders::error));

	}
}

void irc::Connection::handle_ping_deadline(
		const boost::system::error_code& error) {

	/* Check for error */
	if (!error) {

		/* Close connection */
		close_because("Ping timeout");
	}
}

boost::asio::ip::tcp::socket& irc::Connection::socket(void) {

	/* Return the associated socket */
	return m_socket;
}

boost::asio::streambuf& irc::Connection::getBuffer(void) {

	/* Return the associated buffer */
	return m_buffer;
}

void irc::Connection::start(void) {

	/* Resolving remote domain name */
	debug::DEBUG_LOG(m_nickname, "Resolving remote domain name ...");
	boost::asio::ip::tcp::endpoint remote_ep = m_socket.remote_endpoint();
	boost::asio::ip::address remote_ad = remote_ep.address();
	m_hostname = remote_ad.to_string();
	debug::DEBUG_LOG(m_nickname, "Remote domain", m_hostname);

	/* Starting connection */
	debug::DEBUG_LOG(m_nickname, "Starting connection ...");
	boost::asio::async_read_until(m_socket, m_buffer, "\r\n",
			boost::bind(&Connection::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

	/* Start ping timer */
	debug::DEBUG_LOG(m_nickname, "Start ping timer ...");
	m_cycle_ping_timer.expires_from_now(
			boost::posix_time::seconds(m_configuration.ping_refresh_delay));
	m_cycle_ping_timer.async_wait(
			boost::bind(&Connection::handle_ping_refresh, shared_from_this(),
					boost::asio::placeholders::error));

	/* Start deadline timer */
	debug::DEBUG_LOG(m_nickname, "Start deadline timer ...");
	m_dead_ping_timer.expires_from_now(
			boost::posix_time::seconds(m_configuration.ping_timeout_delay));
	m_dead_ping_timer.async_wait(
			boost::bind(&Connection::handle_ping_deadline, shared_from_this(),
					boost::asio::placeholders::error));
}

void irc::Connection::write(const std::string& buffer) {

	/* Send message */
	debug::DEBUG_LOG(m_nickname, "[RAW OUT]", buffer);
	boost::asio::async_write(m_socket, boost::asio::buffer(buffer),
			boost::bind(&Connection::handle_write, shared_from_this(),
					boost::asio::placeholders::error));

}

void irc::Connection::writeToChannels(const std::string& buffer) {

	/* Send message to all joined channel */
	std::for_each(m_channels_joined.begin(), m_channels_joined.end(),
			boost::bind(&Channel_info::writeToAll, _1, buffer));
}

void irc::Connection::close(void) {

	/* Cancel ping timers */
	m_cycle_ping_timer.cancel();
	m_dead_ping_timer.cancel();

	/* Closing connection */
	debug::DEBUG_LOG(m_nickname, "Closing socket ...");
	boost::system::error_code ignored_ec;
	m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	m_socket.close();
}

void irc::Connection::close_because(const std::string& reason) {

	/* Clean database and close socket */
	if (!m_socket.is_open())
		return;
	debug::DEBUG_LOG(m_nickname, "Closing connection", reason);

	/* Craft QUIT reply */
	Reply_generator reply;
	reply.addPrefix(m_nickname);
	reply.CMD_QUIT(reason);

	/* Send reply to all joined channels */
	writeToChannels(reply.toString());

	/* Quit all joined channels */
	std::for_each(m_channels_joined.begin(), m_channels_joined.end(),
			boost::bind(&Channel_info::removeJoin, _1, shared_from_this()));

	/* Remove itself from database */
	m_users_database.remove(shared_from_this());

	/* Close socket */
	close();
}

void irc::Connection::updateIdleTime(void) {

	/* Reset the last msg timestamp to now */
	m_idle_time = boost::posix_time::second_clock::local_time();
}

long irc::Connection::getIdleTime(void) const {

	/* Compute the number of seconds elapsed between the last msg of user and now */
	return boost::posix_time::second_clock::local_time() - m_idle_time;
}
