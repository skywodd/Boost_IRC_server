/**
 * @file Debug_log.hpp
 * @brief Debug logging utility - template header
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This header is a container for various debug utilities (template functions).\n
 * They have been coded to be generic and easily extensible.\n
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
 * Require a C++ compiler with templates support.\n
 * Use clog as output.
 */

#ifndef DEBUG_LOG_H_
#define DEBUG_LOG_H_

/* Dependency */
#include <iostream>

/**
 * @namespace debug
 */
namespace debug {

/**
 * Output variable to debug log stream (using ostream << operator overload)
 *
 * @pre Target variable MUST define << operator overload to ostream !
 * @remark Use template for genericity
 * @remark Define/undefine DEBUG to enable/disable debug output
 * @param var Target variable to display
 */
#ifdef DEBUG
template<typename T>
inline void DEBUG_LOG(const T& var) {
	std::clog << "[DEBUG] " << var << std::endl;
}
#else
template<typename T>
inline void DEBUG_LOG(const T& var) {
}
#endif

/**
 * Output variable to debug log stream (using ostream << operator overload)
 *
 * @pre Target variable MUST define << operator overload to ostream !
 * @remark Use template for genericity
 * @remark Define/undefine DEBUG to enable/disable debug output
 * @param str Label to display before variable
 * @param var Target variable to display
 */
#ifdef DEBUG
template<typename T>
inline void DEBUG_LOG(const std::string& str, const T& var) {
	std::clog << "[DEBUG] " << str << " : " << var << std::endl;
}
#else
template<typename T>
inline void DEBUG_LOG(const std::string& str, const T& var) {
}
#endif

/**
 * Output variable to debug log stream (using ostream << operator overload)
 *
 * @pre Target variable MUST define << operator overload to ostream !
 * @remark Use template for genericity
 * @remark Define/undefine DEBUG to enable/disable debug output
 * @param str Label to display before variable
 * @param key Target variable key to display
 * @param value Target variable value to display
 */
#ifdef DEBUG
template<typename T>
inline void DEBUG_LOG(const std::string& str, const std::string& key, const T& value) {
	std::clog << "[DEBUG] " << str << " : " << key << " = " << value << std::endl;
}
#else
template<typename T>
inline void DEBUG_LOG(const std::string& str, const std::string& key, const T& value) {
}
#endif

}

#endif /* DEBUG_LOG_H_ */
