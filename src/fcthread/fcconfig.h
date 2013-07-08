/*********************************************************************************************
 * System-dependent configurations
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
#ifndef FC_CONFIG_H_                // dup check
#define FC_CONFIG_H_


#if defined(__linux__)

#define _SYS_LINUX_
#define _FC_OSNAME     "Linux"

#elif defined(__FreeBSD__)

#define _SYS_FREEBSD_
#define _FC_OSNAME     "FreeBSD"

#elif defined(__NetBSD__)

#define _SYS_NETBSD_
#define _FC_OSNAME     "NetBSD"

#elif defined(__OpenBSD__)

#define _SYS_OPENBSD_
#define _FC_OSNAME     "OpenBSD"

#elif defined(__sun__) || defined(__sun)

#define _SYS_SUNOS_
#define _FC_OSNAME     "SunOS"

#elif defined(__hpux)

#define _SYS_HPUX_
#define _FC_OSNAME     "HP-UX"

#elif defined(__osf)

#define _SYS_TRU64_
#define _FC_OSNAME     "Tru64"

#elif defined(_AIX)

#define _SYS_AIX_
#define _FC_OSNAME     "AIX"

#elif defined(__APPLE__) && defined(__MACH__)

#define _SYS_MACOSX_
#define _FC_OSNAME     "Mac OS X"

#elif defined(_MSC_VER)

#define _SYS_MSVC_
#define _FC_OSNAME     "Windows (VC++)"

#elif defined(_WIN32)

#define _SYS_MINGW_
#define _KC_OSNAME     "Windows (MinGW)"

#elif defined(__CYGWIN__)

#define _SYS_CYGWIN_
#define _KC_OSNAME     "Windows (Cygwin)"

#else

#define _SYS_GENERIC_
#define _KC_OSNAME     "Generic"

#endif


/*************************************************************************************************
 * notation of the file system
 *************************************************************************************************/

#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)

#define PATHCHR      '\\'
#define PATHSTR      "\\"
#define EXTCHR       '.'
#define EXTSTR       "."
#define CDIRSTR      "."
#define PDIRSTR      ".."

#else

#define PATHCHR      '/'
#define PATHSTR      "/"
#define EXTCHR       '.'
#define EXTSTR       "."
#define CDIRSTR      "."
#define PDIRSTR      ".."

#endif


/*************************************************************************************************
 * general headers
 *************************************************************************************************/

extern "C" {
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
}


#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)

#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <direct.h>
#include <io.h>
#include <process.h>
#include <assert.h>

#else

extern "C" {
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <dirent.h>
}

extern "C" {
#include <pthread.h>
#include <sched.h>
}

#endif

#endif               // dup check

// END OF FILE