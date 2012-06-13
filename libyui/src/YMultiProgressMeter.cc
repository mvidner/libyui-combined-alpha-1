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



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                       ***  core library  ***                                        ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-/

  File:		YMultiProgressMeter.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YMultiProgressMeter.h"


struct YMultiProgressMeterPrivate
{
    YMultiProgressMeterPrivate( YUIDimension			dim,
				const std::vector<float> &	maxValues )
	: dim( dim )
	, maxValues( maxValues )
    {
	// Make currentValues as large as maxValues
	// and initialize each element with 0
	currentValues = std::vector<float>( maxValues.size(), 0.0 );
    }


    YUIDimension	dim;
    std::vector<float>	maxValues;
    std::vector<float>	currentValues;
};




YMultiProgressMeter::YMultiProgressMeter( YWidget *			parent,
					  YUIDimension			dim,
					  const std::vector<float> &	maxValues )
    : YWidget( parent )
    , priv( new YMultiProgressMeterPrivate( dim, maxValues ) )
{
    YUI_CHECK_NEW( priv );

    setDefaultStretchable( YD_HORIZ, dim == YD_HORIZ );
    setDefaultStretchable( YD_VERT,  dim == YD_VERT  );
}


YMultiProgressMeter::~YMultiProgressMeter()
{
    // NOP
}


YUIDimension
YMultiProgressMeter::dimension() const
{
    return priv->dim;
}


bool YMultiProgressMeter::horizontal() const
{
    return priv->dim == YD_HORIZ;
}


bool YMultiProgressMeter::vertical() const
{
    return priv->dim == YD_VERT;
}


int YMultiProgressMeter::segments() const
{
    return (int) priv->maxValues.size();
}


float YMultiProgressMeter::maxValue( int segment ) const
{
    YUI_CHECK_INDEX( segment, 0, (int) priv->maxValues.size() );

    return priv->maxValues[ segment ];
}


float YMultiProgressMeter::currentValue( int segment ) const
{
    YUI_CHECK_INDEX( segment, 0, (int) priv->currentValues.size() );

    return priv->currentValues[ segment ];
}


void YMultiProgressMeter::setCurrentValue( int segment, float value )
{
    YUI_CHECK_INDEX( segment, 0, (int) priv->currentValues.size() );

    if ( value < 0.0 )			// Allow -1 etc. as marker values
	value = 0.0;

    if ( value > maxValue( segment ) ) // Don't complain (beware of rounding errors)
	value = maxValue( segment );

    priv->currentValues[ segment ] = value;
}


void YMultiProgressMeter::setCurrentValues( const std::vector<float> & values )
{
    for ( int i=0; i < (int) values.size(); i++ )
    {
	setCurrentValue( i, values[i] );
    }

    doUpdate();
}


const YPropertySet &
YMultiProgressMeter::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property list<integer> Values the current values for all segments
	 */
	propSet.add( YProperty( YUIProperty_Values,	YOtherProperty	) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YMultiProgressMeter::setProperty( const std::string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if ( propertyName == YUIProperty_Values )	return false; // need special processing
    else
    {
	YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special handling necessary
}


YPropertyValue
YMultiProgressMeter::getProperty( const std::string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if ( propertyName == YUIProperty_Values ) 	return YPropertyValue( YOtherProperty );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
