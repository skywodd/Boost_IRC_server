/**
 * @file Connection.hpp
 * @brief Socket connection class
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is designed to handle TCP connection (read, write, ...).\n
 * Relation with other class are made using shared_ptr wrapper.\n
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

#ifndef CONNECTION_H_
#define CONNECTION_H_

/* Dependencies */
#include <list>
#include <string>
#include <cstddef>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "User_info.hpp"
#include "Request_handler.hpp"

/**
 * @namespace irc
 */
namespace irc {

/* Forward declaration */
class Configuration;
class Users_manager;
class Channels_manager;

/**
 * @class Connection
 */
class Connection: public boost::enable_shared_from_this<Connection>,
		public User_info {
protected:
	/** Server configuration */
	const Configuration& m_configuration;

	/** Timer for refreshing ping status */
	boost::asio::deadline_timer m_cycle_ping_timer;

	/** Timer for dead connection (no ping answer in delay) */
	boost::asio::deadline_timer m_dead_ping_timer;

	/** Socket for the connection */
	boost::asio::ip::tcp::socket m_socket;

	/** Stream buffer for incoming data */
	boost::asio::streambuf m_buffer;

	/** Users database pointer */
	Users_manager& m_users_database;

	/** Channels database pointer */
	Channels_manager& m_channels_database;

	/** Idle time */
	boost::posix_time::ptime m_idle_time;

	/** Last ping argument */
	std::string m_ping_arg;

	/** Request handler */
	Request_handler m_handler;

	/**
	 * Handle completion of a read operation
	 *
	 * @param error Error code
	 * @param bytes_transferred Number of bytes written to the buffer
	 */
	void handle_read(const boost::system::error_code& error,
			const std::size_t bytes_transferred);

	/**
	 * Handle completion of a write operation
	 *
	 * @param error Error code
	 */
	void handle_write(const boost::system::error_code& error);

	/**
	 * Handle ping refresh timer
	 *
	 * @param error Error code
	 */
	void handle_ping_refresh(const boost::system::error_code& error);

	/**
	 * Handle ping deadline timer
	 *
	 * @param error Error code
	 */
	void handle_ping_deadline(const boost::system::error_code& error);

	/**
	 * Instanciate a new connection
	 *
	 * @param io_service IO_service to use for callback management
	 * @param users_database Pointer to the users database
	 * @param channels_database Pointer to the channels database
	 * @param configuration Default connection configuration
	 */
	explicit Connection(boost::asio::io_service& io_service,
			Users_manager& users_database, Channels_manager& channels_database,
			const Configuration& configuration);

public:
	/**
	 * Instanciate a new connection
	 *
	 * @param io_service IO_service to use for callback management
	 * @param users_database Pointer to the users database
	 * @param channels_database Pointer to the channels database
	 * @param configuration Default connection configuration
	 */
	static boost::shared_ptr<Connection> create(
			boost::asio::io_service& io_service, Users_manager& users_database,
			Channels_manager& channels_database,
			const Configuration& configuration);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Connection(void);

	/**
	 * Get the socket associated with the connection
	 */
	boost::asio::ip::tcp::socket& socket(void);

	/**
	 * Get the buffer associated with the connection
	 */
	boost::asio::streambuf& getBuffer(void);

	/**
	 * Initiate the first read operation
	 */
	void start(void);

	/**
	 * Write data over the socket
	 *
	 * @param buffer Data to send over the socket
	 */
	void write(const std::string& buffer);

	/**
	 * Write data to all joined channel
	 *
	 * @param buffer Data to send to all joined channel
	 */
	void writeToChannels(const std::string& buffer);

	/**
	 * Close the socket
	 */
	void close(void);

	/**
	 * Close the connection and clean user database
	 */
	void close_because(const std::string& reason);

	/**
	 * Update the IDLE time of user
	 */
	void updateIdleTime(void);

	/**
	 * Get IDLE time
	 *
	 * @return The IDLE time of the user
	 */
	long getIdleTime(void) const;

};

}

#endif /* CONNECTION_H_ */
