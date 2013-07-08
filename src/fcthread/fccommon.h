/*********************************************************************************************
 * Common symbols for the library
 *                                                           Copyright (C) 2009-2011 MyFC Labs
 *
 * This file is part of foundation class library
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, either version
 * 3 of the License, or any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
**********************************************************************************************/

#ifndef FC_COMMON_H_
#define FC_COMMON_H_


#include <stdexcept>
#include <exception>
#include <limits>
#include <new>
#include <typeinfo>

#include <utility>
#include <functional>
#include <memory>
#include <iterator>
#include <algorithm>
#include <locale>

#include <string>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <map>
#include <set>

#include <ios>
#include <iostream>
#include <streambuf>
#include <fstream>
#include <sstream>

#ifdef _MSC_VER
// MSVC 2003 and below is not c99 compitable, need to define the types
typedef __int8           int8_t;
typedef unsigned __int8  uint8_t; 
typedef __int16          int16_t;
typedef unsigned __int16 uint16_t; 
typedef __int32          int32_t; 
typedef unsigned __int32 uint32_t; 
typedef __int64          int64_t; 
typedef unsigned __int64 uint64_t; 

#else
extern "C" {
#include <stdint.h>
}  
#endif

namespace fc {
const int8_t INT8MAX = (std::numeric_limits<int8_t>::max)();
const int16_t INT16MAX = (std::numeric_limits<int16_t>::max)();
const int32_t INT32MAX = (std::numeric_limits<int32_t>::max)();
const int64_t INT64MAX = (std::numeric_limits<int64_t>::max)();
const int8_t INT8MIN = (std::numeric_limits<int8_t>::min)();
const int16_t INT16MIN = (std::numeric_limits<int16_t>::min)();
const int32_t INT32MIN = (std::numeric_limits<int32_t>::min)();
const int64_t INT64MIN = (std::numeric_limits<int64_t>::min)();

const uint8_t UINT8MAX = (std::numeric_limits<uint8_t>::max)();
const uint16_t UINT16MAX = (std::numeric_limits<uint16_t>::max)();
const uint32_t UINT32MAX = (std::numeric_limits<uint32_t>::max)();
const uint64_t UINT64MAX = (std::numeric_limits<uint64_t>::max)();

const int32_t CLOCKTICK = 100;

}

#endif