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
#include "Channels_manager.hpp"
#include "Configuration.hpp"
#include "Channel_info.hpp"
#include "Connection.hpp"
#include "Debug_log.hpp"

irc::Channels_manager::Channels_manager(const Configuration& configuration) :
		m_database(), m_nb_channels(0), m_nb_channels_limit(
				configuration.nb_channels_limit), m_configuration(configuration) {

	/* Creating a new database */
	debug::DEBUG_LOG("Channels database", "Creating database ...");
	debug::DEBUG_LOG("Channels database, max number of channels",
			m_nb_channels_limit);
}

irc::Channels_manager::~Channels_manager(void) {

	/* Destroying database */
	debug::DEBUG_LOG("Channels database", "Destroying database ...");
}

int irc::Channels_manager::getChannelsCount(void) const {

	/* Return the number of channels in the database */
	return m_nb_channels;
}

int irc::Channels_manager::getChannelsCountLimit(void) const {

	/* Return the maximum number of channels of the database */
	return m_nb_channels_limit;
}

boost::shared_ptr<irc::Channel_info> irc::Channels_manager::add(
		const std::string& name) {

	/* Add a channel into the database */
	debug::DEBUG_LOG("Add channel to the database", name);
	boost::shared_ptr<Channel_info> new_chan = Channel_info::create(
			m_configuration);
	m_database[name] = new_chan;
	++m_nb_channels; /* Update channels count */

	/* Return freshly created channel */
	return new_chan;
}

void irc::Channels_manager::remove(const std::string& name) {

	/* Remove channel from database */
	debug::DEBUG_LOG("Remove channel from database", name);
	if (m_database.erase(name))
		--m_nb_channels; /* Update channels count */
}

boost::shared_ptr<irc::Channel_info> irc::Channels_manager::access(
		const std::string& name) {

	/* Acces to channel informations */
	std::map<std::string, boost::shared_ptr<Channel_info> >::iterator index;
	index = m_database.find(name); /* search channel in database */

	/* Check if channel exist or not */
	if (index != m_database.end())
		return (*index).second; /* Channel exist, return channel informations */
	else
		/* Channel don't exist, return null pointer */
		return boost::shared_ptr<irc::Channel_info>();
}
