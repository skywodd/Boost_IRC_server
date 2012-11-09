/**
 * @file Server.hpp
 * @brief Single-threaded TCP server class
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * Please report bug to <skywodd at gmail.com>
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

#ifndef SERVER_H_
#define SERVER_H_

/* Dependencies */
#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include "Users_manager.hpp"
#include "Channels_manager.hpp"

/**
 * @namespace irc
 *
 * Namespace regrouping all IRC features of the program.
 */
namespace irc {

/* Forward declarations */
class Connection;
class Configuration;

/**
 * @class Server
 *
 * This class handle all incoming TCP connection on the specified port and bind address.\n
 * This class does NOT handle any IRC communications, only incoming TCP connections.\n
 */
class Server: private boost::noncopyable {
protected:
	/** IO_service used to perform asynchronous operations */
	boost::asio::io_service m_io_service;

#if BOOST_VERSION > 104700
	/** Signal_set used to handle process quit notifications */
	boost::asio::signal_set m_signals;
#else
#warning "You need Boost 1.47 (at least) to have signals support !"
#endif

	/** Acceptor used to listen of incoming connections */
	boost::asio::ip::tcp::acceptor m_acceptor;

	/** Server configuration */
	const Configuration& m_configuration;

	/** Users database */
	Users_manager m_users_database;

	/** Channels database */
	Channels_manager m_channels_database;

	/** Date / time of the server startup */
	boost::posix_time::ptime m_since;

	/** Server instance */
	static Server* m_instance;

	/**
	 * Start an asynchronous accept operation
	 */
	void start_accept(void);

	/**
	 * Handle completion of an asynchronous accept operation
	 *
	 * @param new_connection Intelligent pointer to the connection used for accept
	 * @param error Error code (if any)
	 */
	void handle_accept(boost::shared_ptr<Connection> new_connection, const boost::system::error_code& error);

public:
	/**
	 * Instantiate a new Server object
	 *
	 * @warning Require Boost 1_47 (at least) to support signals handling !
	 * @remarks If signals support is not available compilation will not fail but signals support will be disable.
	 * @pre This class store a pointer to itself to work ! ONLY ONE instance of Server class MUST be created !
	 *
	 * @param address Address to bind the server on
	 * @param port Port to listen on
	 * @param configuration Server configuration to use
	 */
	explicit Server(const std::string& address, const std::string& port, Configuration& configuration);

	/**
	 * Get the pointer to the server instance
	 *
	 * @return The pointer to the server instance
	 */
	Server* getInstance(void) const;

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Server(void);

	/**
	 * Start IO_service to process incoming connections
	 *
	 * @remark Blocking function
	 */
	void run(void);

	/**
	 * Handle kill request (safe & clean server shutdown)
	 */
	void stop(void);

	/**
	 * Get server startup time
	 *
	 * @return Startup time in Posix_time format
	 */
	boost::posix_time::ptime runSince(void) const;

};

} /* namespace irc */

#endif /* SERVER_H_ */
