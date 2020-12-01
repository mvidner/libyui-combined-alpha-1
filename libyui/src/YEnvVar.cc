/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:		YEnvVar.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#include <stdlib.h>	// getenv()
#include <string.h>	// strcmp(), strcasecmp()
#include <ctype.h>	// tolower()

#define YUILogComponent "ui"
#include "YUILog.h"

#include <YEnvVar.h>


YEnvVar::YEnvVar( const string & name )
    : _name( name )
    , _isSet( false )
{
    if ( ! _name.empty() )
    {
	const char * val = getenv( _name.c_str() );

	if ( val )
	{
	    _isSet = true;
	    _value = val;
	}
    }
}


bool
YEnvVar::isEqual( const string & str, bool caseSensitive ) const
{
    if ( ! _isSet )
	return false;

    if ( caseSensitive )
	return strcmp( _value.c_str(), str.c_str() ) == 0;
    else
	return strcasecmp( _value.c_str(), str.c_str() ) == 0;
}

bool
YEnvVar::contains( const string & str, bool caseSensitive ) const
{
    if ( ! _isSet )
	return false;

    if ( caseSensitive )
    {
	return _value.find( str ) != string::npos;
    }
    else
    {
	return tolower( _value ).find( tolower( str ) ) != string::npos;
    }
}


string tolower( const string & str )
{
    string lowStr;
    lowStr.reserve( str.size() );

    for ( string::const_iterator it = str.begin();
	  it != str.end();
	  ++it )
    {
	lowStr += ::tolower( *it );
    }

    return lowStr;
}


std::ostream &
operator<<( std::ostream & stream, const YEnvVar env )
{
    if ( env.name().empty() )
    {
	stream << "<unnamed environment variable>";
    }
    else
    {
	if ( env.isSet() )
	    stream << "$" << env.name() << "=\"" << env.value() << "\"";
	else
	    stream << "$" << env.name() << ": <not set>";
    }
    
    return stream;
}
