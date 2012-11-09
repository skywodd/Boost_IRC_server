/**
 * @file Channels_manager.hpp
 * @brief Channels database manager
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
 *
 * Namespace regrouping all IRC features of the program.
 */
namespace irc {

/* Forward declarations */
class Configuration;
class Channel_info;
class Connection;

/**
 * @class Channels_manager
 *
 * This class is designed to manage channels database.\n
 * This class allow developer to add, remove and access to channels informations in the database.\n
 * \n
 * Add, remove and access operations are made using channel name.\n
 * Remove operations can also be made using intelligent pointer.\n
 * \n
 * This class keep up-to-date a counter of channels currently into the database.\n
 * Each channel name are associated to a channel informations intelligent pointer.\n
 * \n
 * Channels broadcast operation are NOT handled by this class (see Channel_info instead).
 */
class Channels_manager: private boost::noncopyable {
protected:
	/** Channels database: channel name as keys, channel informations as values */
	std::map<std::string, boost::shared_ptr<Channel_info> > m_database;

	/** Number of channels currently into the database */
	int m_nb_channels;

	/** Maximum number of channels of the database */
	int m_nb_channels_limit;

	/** Default channels configuration */
	const Configuration& m_configuration;

	/**
	 * Functor : Search channel in the database by pointer
	 *
	 * @param channel Pointer to the channel to search
	 * @param input Pair retrieved from the database to check against "channel"
	 */
	static bool search_channel(boost::shared_ptr<Channel_info> channel,
			std::pair<std::string, boost::shared_ptr<Channel_info> > input);

public:
	/**
	 * Instantiate a new channels database
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
	 * Get the number of channels currently into the database
	 *
	 * @return The number of channels into the database
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
	 * Remove a channel from the database
	 *
	 * @param name Name of the channel to remove
	 */
	void remove(const std::string& name);

	/**
	 * Remove a channel from the database
	 *
	 * @param channel Pointer to the channel to remove
	 */
	void remove(boost::shared_ptr<Channel_info> channel);

	/**
	 * Access to a channel in the database
	 *
	 * @param name Name of the channel to access
	 * @return Intelligent pointer to the channel informations or NULL if not found
	 */
	boost::shared_ptr<Channel_info> access(const std::string& name);

};

} /* namespace irc */

#endif /* CHANNELS_MANAGER_H_ */
