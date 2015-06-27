/*
 *  Header file for cuRNN error functions.
 *
 *  Copyright (C) 2015 Rob Clucas robclu1818@gmail.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT AN_size.y WARRANTY; without even the implied warranty of
 *  MERCHANTABILIT_size.y or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  _size.you should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation,
 *	Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef __CURNN_UTIL_ERRORS__
#define __CURNN_UTIL_ERRORS__

#include <iostream>

// To convert variable names to strings
#define stringify( name ) varname( #name )
inline const char* varname( const char* name ) { return name; }

namespace currn {
	namespace util {
		namespace err {
			/*
			 * ==============================================================================================
			 * Function		: allocError
			 *
			 * Description	: Prints an error message of allocation failed
			 *
			 * Inputs		: varname	: The name of the variable that could not be allocated
			 * ==============================================================================================
			 */
			void allocError( const char * varname );	

			/*
			 * ==============================================================================================
			 * Function		: copyError
			 *
			 * Description	: Prints an error message if host to device, or device to host copy failed
			 *
			 * Inputs		: varname	: The name of the variable that could not be copied to/from
			 * ==============================================================================================
			 */
			void copyError( const char* varname );}
	}
}

#endif
