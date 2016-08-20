/*
 * utils.h
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#ifndef UTILS_H
#define UTILS_H
/* To avoid this error:
 *      undefined reference to `boost::system::generic_category()'
 */
#define BOOST_SYSTEM_NO_DEPRECATED

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdint.h>
#include <sys/time.h>

#include <boost/thread.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

#endif
