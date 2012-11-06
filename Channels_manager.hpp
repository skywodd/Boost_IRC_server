/**
 * @file Channels_manager.hpp
 * @brief Channels database manager
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This class is designed to manage channels database.\n
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

#ifndef CHANNELS_MANAGER_H_
#define CHANNELS_MANAGER_H_

/* Dependencies */
#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>

/**
 * @namespace irc
 */
namespace irc {

/* Forward declarations */
class Configuration;
class Channel_info;
class Connection;

/**
 * @class Channels_manager
 */
class Channels_manager: private boost::noncopyable {
protected:
	/** Channel database: channel name as keys, channel informations as values */
	std::map<std::string, boost::shared_ptr<Channel_info> > m_database;

	/** Number of channels currently in the database */
	int m_nb_channels;

	/** Maximum number of channels of the database */
	int m_nb_channels_limit;

	/** Server configuration */
	const Configuration& m_configuration;

public:
	/**
	 * Instantiate a new Channels_manager object
	 *
	 * @param configuration Configuration of database
	 */
	explicit Channels_manager(const Configuration& configuration);

	/**
	 * Destructor
	 *
	 * @remarks Ready for overload
	 */
	~Channels_manager(void);

	/**
	 * Get the number of channels currently in the database
	 *
	 * @return The number of channels in the database
	 */
	int getChannelsCount(void) const;

	/**
	 * Get the maximum number of channels of the database
	 *
	 * @return The maximum number of channels of the database
	 */
	int getChannelsCountLimit(void) const;

	/**
	 * Add a new channel into the database
	 *
	 * @param name Name of the new channel
	 */
	boost::shared_ptr<Channel_info> add(const std::string& name);

	/**
	 * Remove channel from the database
	 *
	 * @param name Name of the channel to remove
	 */
	void remove(const std::string& name);

	/**
	 * Access to a channel in the database
	 *
	 * @param name Name of the channel to access
	 * @return Pointer to the channel informations, or NULL if not exist
	 */
	boost::shared_ptr<Channel_info> access(const std::string& name);
};

}

#endif /* CHANNELS_MANAGER_H_ */
