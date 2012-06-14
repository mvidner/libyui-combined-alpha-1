/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/


/*-/

  File:		YTimezoneSelector.cc

  Author:	Stephan Kulow <coolo@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YTimezoneSelector.h"


class YTimezoneSelectorPrivate
{
   bool dummy;
};




YTimezoneSelector::YTimezoneSelector( YWidget *					parent,
                                      const std::string				&pixmap,
                                      const std::map<std::string, std::string>	&timezones )
    : YWidget( parent )
{
}


YTimezoneSelector::~YTimezoneSelector()
{
    // NOP
}


const YPropertySet &
YTimezoneSelector::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
        propSet.add( YProperty( YUIProperty_Value, YStringProperty ) );
        propSet.add( YProperty( YUIProperty_CurrentItem, YStringProperty ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YTimezoneSelector::setProperty( const std::string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if ( propertyName == YUIProperty_Value )
    {
        setCurrentZone( val.stringVal(), true );
        return true; // success -- no special handling necessary
    }
    if ( propertyName == YUIProperty_CurrentItem )
    {
        setCurrentZone( val.stringVal(), false );
        return true; // success -- no special handling necessary
    }
    return YWidget::setProperty( propertyName, val );
}


YPropertyValue
YTimezoneSelector::getProperty( const std::string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if ( propertyName == YUIProperty_Value ) 	return YPropertyValue( currentZone() );
    if ( propertyName == YUIProperty_CurrentItem )    return YPropertyValue( currentZone() );
    return YWidget::getProperty( propertyName );
}
