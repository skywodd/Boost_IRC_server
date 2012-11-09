/**
 * @file Connection.hpp
 * @brief Socket connection class
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
 *
 * Namespace regrouping all IRC features of the program.
 */
namespace irc {

/**
 * @class Connection
 *
 * This class is designed to handle all READ / WRITE operations of user.\n
 * This class also handle ping timeout, IDLE time, channels broadcast and socket closing.\n
 * \n
 * All read / write operations are made using asynchronous functions from Boost.\n
 * The IRC request parser is called when a \\r\\n footprint is received.
 */
class Connection: public boost::enable_shared_from_this<Connection>,
		public User_info {
protected:
	/** Timer for refreshing ping status */
	boost::asio::deadline_timer m_cycle_ping_timer;

	/** Timer for dead connection (no ping answer in delay) */
	boost::asio::deadline_timer m_dead_ping_timer;

	/** Socket for the connection */
	boost::asio::ip::tcp::socket m_socket;

	/** Stream buffer for storing incoming data */
	boost::asio::streambuf m_buffer;

	/** Idle time in posix time */
	boost::posix_time::ptime m_idle_time;

	/** Last ping seed arguments */
	std::string m_ping_arg;

	/** Request handler object */
	Request_handler m_handler;

	/**
	 * Functor : Handle QUIT of a channel (with channel deletion if necessary)
	 *
	 * @param user Pointer to the user who quit
	 * @param channel Pointer to the channel to quit cleanly
	 */
	static void quit_channel(boost::shared_ptr<Connection> user, boost::shared_ptr<Channel_info> channel);

	/**
	 * Handle completion of a read operation
	 *
	 * @param error Error code (if any)
	 * @param bytes_transferred Number of bytes written to the buffer
	 */
	void handle_read(const boost::system::error_code& error,
			const std::size_t bytes_transferred);

	/**
	 * Handle completion of a write operation
	 *
	 * @param error Error code (if any)
	 */
	void handle_write(const boost::system::error_code& error);

	/**
	 * Handle ping refresh
	 *
	 * @param error Error code (if any)
	 */
	void handle_ping_refresh(const boost::system::error_code& error);

	/**
	 * Handle ping deadline
	 *
	 * @param error Error code (if any)
	 */
	void handle_ping_deadline(const boost::system::error_code& error);

	/**
	 * Instantiate a new connection
	 *
	 * @param io_service IO_service to use for callback management
	 */
	explicit Connection(boost::asio::io_service& io_service);

public:
	/**
	 * Instantiate a new connection
	 *
	 * @param io_service IO_service to use for callback management
	 */
	static boost::shared_ptr<Connection> create(
			boost::asio::io_service& io_service);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	virtual ~Connection(void);

	/**
	 * Get the socket associated with the connection
	 *
	 * @return The socket associated with the connection
	 */
	boost::asio::ip::tcp::socket& socket(void);

	/**
	 * Get the stream buffer associated with the connection
	 *
	 * @return The stream buffer associated with the connection
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
	 * Close the connection and clean the user database
	 *
	 * @param reason Reason of the close
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

	/**
	 * Get the last PING arguments sent to the client
	 *
	 * @return The last PING arguments sent to the client
	 */
	const std::string& getLastPingArg(void) const;

	/**
	 * Restart the dead line timer
	 */
	void restartDeadlineTimer(void);

};

} /* namespace irc */

#endif /* CONNECTION_H_ */
