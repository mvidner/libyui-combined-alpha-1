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
 ////                                      ***  NCurses plugin  ***                                       ////
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

   File:       NCTreePad.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTreePad.h"



NCTreePad::NCTreePad( int lines, int cols, const NCWidget & p )
	: NCPad( lines, cols, p )
	, Headpad( 1, 1 )
	, dirtyHead( false )
	, dirtyFormat( false )
	, ItemStyle( p )
	, Headline( 0 )
	, Items( 0 )
	, visItems( 0 )
	, citem( 0 )
{
}



NCTreePad::~NCTreePad()
{
    ClearTable();
}



void NCTreePad::assertLine( unsigned idx )
{
    if ( idx >= Lines() )
	SetLines( idx + 1 );
}



void NCTreePad::SetLines( unsigned idx )
{
    if ( idx == Lines() )
	return;

    unsigned olines = Lines();

    if ( idx < Lines() )
    {
	for ( unsigned i = idx; i < Lines(); ++i )
	{
	    delete Items[i];
	}
    }

    Items.resize( idx, 0 );

    for ( unsigned i = olines; i < Lines(); ++i )
    {
	if ( !Items[i] )
	    Items[i] = new NCTableLine( 0 );
    }

    DirtyFormat();
}



void NCTreePad::SetLines( vector<NCTableLine*> & nItems )
{
    SetLines( 0 );
    Items = nItems;

    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( !Items[i] )
	    Items[i] = new NCTableLine( 0 );
    }

    DirtyFormat();
}



void NCTreePad::AddLine( unsigned idx, NCTableLine * item )
{
    assertLine( idx );
    delete Items[idx];
    Items[idx] = item ? item : new NCTableLine( 0 );

    DirtyFormat();
}



void NCTreePad::DelLine( unsigned idx )
{
    if ( idx < Lines() )
    {
	Items[idx]->ClearLine();
	DirtyFormat();
    }
}



const NCTableLine * NCTreePad::GetCurrentLine() const
{
    if ( citem.L >= 0 && ( unsigned )citem.L < visLines() )
	return visItems[citem.L];

    return 0;
}



NCTableLine * NCTreePad::ModifyLine( unsigned idx )
{
    if ( idx < Lines() )
    {
	DirtyFormat();
	return Items[idx];
    }

    return 0;
}



const NCTableLine * NCTreePad::GetLine( unsigned idx ) const
{
    if ( idx < Lines() )
	return Items[idx];

    return 0;
}



bool NCTreePad::SetHeadline( const vector<NCstring> & head )
{
    bool hascontent = ItemStyle.SetStyleFrom( head );
    DirtyFormat();
    update();
    return hascontent;
}



void NCTreePad::Destwin( NCursesWindow * dwin )
{
    NCPad::Destwin( dwin );

    if ( destwin )
    {
	maxspos.L = visLines() > ( unsigned )srect.Sze.H ? visLines() - srect.Sze.H : 0;
    }
}



void NCTreePad::wRecoded()
{
    DirtyFormat();
    update();
}



wpos NCTreePad::CurPos() const
{
    citem.C = srect.Pos.C;
    return citem;
}



void NCTreePad::ShowItem( const NCTableLine * item )
{
    if ( !item )
	return;

    if ( const_cast<NCTableLine *>( item )->ChangeToVisible() || dirtyFormat )
	UpdateFormat();

    for ( unsigned l = 0; l < visLines(); ++l )
    {
	if ( visItems[l] == item )
	{
	    setpos( wpos( l, srect.Pos.C ) );
	    break;
	}
    }
}



wsze NCTreePad::UpdateFormat()
{
    dirty = true;
    dirtyFormat = false;
    visItems.clear();
    ItemStyle.ResetToMinCols();

    for ( unsigned l = 0; l < Lines(); ++l )
    {
	Items[l]->UpdateFormat( ItemStyle );

	if ( Items[l]->isVisible() )
	    visItems.push_back( Items[l] );
    }

    maxspos.L = visLines() > ( unsigned )srect.Sze.H ? visLines() - srect.Sze.H : 0;

    resize( wsze( visLines(), ItemStyle.TableWidth() ) );
    return wsze( visLines(), ItemStyle.TableWidth() );
}



int NCTreePad::DoRedraw()
{
    if ( !Destwin() )
    {
	dirty = true;
	return OK;
    }

    if ( dirtyFormat )
	UpdateFormat();

    bkgdset( ItemStyle.getBG() );

    clear();

    wsze lSze( 1, width() );

    for ( unsigned l = 0; l < visLines(); ++l )
    {
	visItems[l]->DrawAt( *this, wrect( wpos( l, 0 ), lSze ),
			     ItemStyle, ( l == ( unsigned )citem.L ) );
    }

    if ( Headpad.width() != width() )
	Headpad.resize( 1, width() );

    Headpad.clear();

    ItemStyle.Headline().DrawAt( Headpad, wrect( wpos( 0, 0 ), lSze ),
				 ItemStyle, false );

    SendHead();

    dirty = false;

    return update();
}



int NCTreePad::setpos( const wpos & newpos )
{
    if ( !visLines() )
    {
	if ( dirty )
	    return DoRedraw();

	return OK;
    }

    if ( dirtyFormat )
	UpdateFormat();

    // save old values
    int oitem = citem.L;

    int opos  = srect.Pos.C;

    // calc new values
    citem.L = newpos.L < 0 ? 0 : newpos.L;

    if (( unsigned )citem.L >= visLines() )
	citem.L = visLines() - 1;

    srect.Pos = wpos( citem.L - ( drect.Sze.H - 1 ) / 2, newpos.C ).between( 0, maxspos );

    if ( citem.L != oitem )
    {
	unsigned at  = 0;
	unsigned len = 0;

	if ( citem.L >= 0 && visItems[citem.L] )
	    len = visItems[citem.L]->Hotspot( at );
	else
	    return ERR;

	if ( len )
	{
	    if (( int )at < srect.Pos.C )
	    {
		srect.Pos.C = at;
	    }
	    else if (( int )( at + len - srect.Pos.C ) > drect.Sze.W )
	    {
		srect.Pos.C = ( int )at < maxspos.C ? at : maxspos.C;
	    }
	}
    }

    if ( dirty )
    {
	return DoRedraw();
    }

    // adjust only
    if ( citem.L != oitem )
    {
	visItems[oitem]->DrawAt( *this, wrect( wpos( oitem, 0 ), wsze( 1, width() ) ),
				 ItemStyle, false );
    }

    visItems[citem.L]->DrawAt( *this, wrect( wpos( citem.L, 0 ), wsze( 1, width() ) ),

			       ItemStyle, true );

    if ( srect.Pos.C != opos )
	SendHead();

    return update();
}



void NCTreePad::updateScrollHint()
{
    NCPad::updateScrollHint();
}



bool NCTreePad::handleInput( wint_t key )
{
    bool handled = true;

    if ( !GetCurrentLine() )
	return false;

    switch ( key )
    {
	case KEY_UP:
	case KEY_PPAGE:
	case KEY_DOWN:
	case KEY_NPAGE:
	    //handle these in compatible way with other widgets (#251180)
	    //jump to the first/last item

	case KEY_HOME:
	case KEY_END:
	    //scroll horizontally

	case KEY_RIGHT:
	case KEY_LEFT:
	    handled = NCPad::handleInput( key );
	    break;

	    //use these for toggling pack/unpack the tree

	case '+':
	case '-':
	case KEY_IC:
	case KEY_DC:
	case KEY_SPACE:
        //  case KEY_RETURN: - see bug 67350

	    if ( visItems[citem.L]->handleInput( key ) )
	    {
		UpdateFormat();
		setpos( wpos( citem.L, srect.Pos.C ) );
	    }

	    break;

	default:
	    handled = false;
    }

    return handled;
}


std::ostream & operator<<( std::ostream & STREAM, const NCTreePad & OBJ )
{
    STREAM << "TreePad: lines " << OBJ.Lines() << std::endl;

    for ( unsigned idx = 0; idx < OBJ.Lines(); ++idx )
    {
	STREAM << idx << " " << *OBJ.GetLine( idx );
    }

    return STREAM;
}

