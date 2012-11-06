/**
 * @file Server.hpp
 * @brief Single-threaded server class
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is designed to handle incoming connections.\n
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
 *
 * @section other_sec Others notes and compatibility warning
 * @warning Require Boost 1_51 or more recent !
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
 */
namespace irc {

/* Forward declarations */
class Connection;
class Configuration;

/**
 * @class Server
 */
class Server: private boost::noncopyable {
protected:
	/** IO_service used to perform asynchronous operations */
	boost::asio::io_service m_io_service;

	/** Signal_set used to handle process quit notifications */
	boost::asio::signal_set m_signals;

	/** Acceptor used to listen for incoming connections */
	boost::asio::ip::tcp::acceptor m_acceptor;

	/** Server configuration */
	const Configuration& m_configuration;

	/** Users database */
	Users_manager m_users_database;

	/** Channels database */
	Channels_manager m_channels_database;

	/**
	 * Start an asynchronous accept operation
	 */
	void start_accept(void);

	/**
	 * Handle completion of an asynchronous accept operation
	 *
	 * @param new_connection Pointer to the connection used for accept
	 * @param error Error code
	 */
	void handle_accept(boost::shared_ptr<Connection> new_connection, const boost::system::error_code& error);

public:
	/**
	 * Instanciate a new Server object
	 *
	 * @param address Address to bind the server on
	 * @param port Port to listen on
	 * @param configuration Server configuration to use
	 */
	explicit Server(const std::string& address, const std::string& port, const Configuration& configuration);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Server(void);

	/**
	 * Start IO_service loops to process incoming connections
	 *
	 * @remark Blocking function
	 */
	void run(void);

	/**
	 * Handle kill request (shutdown server)
	 */
	void stop(void);

	/**
	 * Get server startup time
	 *
	 * @return Startup time in Posix_time format
	 */
	boost::posix_time::ptime runSince(void) const;
};

}

#endif /* SERVER_H_ */
