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

  File:		YCheckBoxFrame.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YCheckBoxFrame.h"


struct YCheckBoxFramePrivate
{
    YCheckBoxFramePrivate( const std::string & label )
	: label( label )
	, autoEnable( true )
	, invertAutoEnable( false )
	{}

    std::string	label;
    bool	autoEnable;
    bool	invertAutoEnable;
};




YCheckBoxFrame::YCheckBoxFrame( YWidget *		parent,
				const std::string &	label,
				bool			isChecked )
    : YSingleChildContainerWidget( parent )
    , priv( new YCheckBoxFramePrivate( label ) )
{
    YUI_CHECK_NEW( priv );
}


YCheckBoxFrame::~YCheckBoxFrame()
{
    // NOP
}


std::string YCheckBoxFrame::label() const
{
    return priv->label;
}

void YCheckBoxFrame::setLabel( const std::string & label )
{
    priv->label = label;
}

bool YCheckBoxFrame::autoEnable() const
{
    return priv->autoEnable;
}

void YCheckBoxFrame::setAutoEnable( bool autoEnable )
{
    // yuiDebug() << "Auto enable: " << boolalpha << autoEnable << std::endl;
    priv->autoEnable = autoEnable;
}

bool YCheckBoxFrame::invertAutoEnable() const
{
    return priv->invertAutoEnable;
}

void YCheckBoxFrame::setInvertAutoEnable( bool invertAutoEnable )
{
    // yuiDebug() << "Invert auto enable: ", boolalpha << invertAutoEnable << std::endl;
    priv->invertAutoEnable = invertAutoEnable;
}


void YCheckBoxFrame::handleChildrenEnablement( bool enabled )
{
    if ( autoEnable() )
    {
	if ( invertAutoEnable() )
	    enabled = ! enabled;

	yuiDebug() << ( enabled ? "Enabling" : "Disabling" ) << " child widgets of " << this << std::endl;
	setChildrenEnabled( enabled );
    }
}


const YPropertySet &
YCheckBoxFrame::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property boolean	  Value 	the on/off state of the CheckBoxFrame's check box
	 * @property std::string  Label		the text on the frame
	 */

	propSet.add( YProperty( YUIProperty_Value,	YBoolProperty	) );
	propSet.add( YProperty( YUIProperty_Label,	YStringProperty	) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YCheckBoxFrame::setProperty( const std::string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if      ( propertyName == YUIProperty_Value )	setValue( val.boolVal() );
    else if ( propertyName == YUIProperty_Label )	setLabel( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YCheckBoxFrame::getProperty( const std::string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if      ( propertyName == YUIProperty_Value )	return YPropertyValue( value() );
    else if ( propertyName == YUIProperty_Label )	return YPropertyValue( label() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
