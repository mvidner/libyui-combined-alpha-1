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

   File:	YSquash.cc

   Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#include "YSquash.h"
#include "YBothDim.h"


struct YSquashPrivate
{
    /**
     * Constructor.
     **/
    YSquashPrivate( bool horSquash, bool vertSquash )
    {
	squash.hor  = horSquash;
	squash.vert = vertSquash;
    }

    YBothDim<bool> squash;
};


YSquash::YSquash( YWidget * parent, bool horSquash, bool vertSquash )
    : YSingleChildContainerWidget( parent )
    , priv( new YSquashPrivate( horSquash, vertSquash ) )
{
    YUI_CHECK_NEW( priv );
}


YSquash::~YSquash()
{
    // NOP
}


bool YSquash::horSquash() const
{
    return priv->squash.hor;
}


bool YSquash::vertSquash() const
{
    return priv->squash.vert;
}


bool YSquash::stretchable( YUIDimension dim ) const
{
    if ( ! hasChildren() )
	return false;

    return ! priv->squash[ dim ] && firstChild()->stretchable( dim );
}


const char *
YSquash::widgetClass() const
{
    if      ( priv->squash.hor && priv->squash.vert )	return "YHVSquash";
    else if ( priv->squash.hor  )			return "YHSquash";
    else if ( priv->squash.vert )			return "YVSquash";
    else						return "YSquash_NoSquash";
}
