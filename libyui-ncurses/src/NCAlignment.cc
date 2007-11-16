/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCAlignment.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCAlignment.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAlignment::NCAlignment
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCAlignment::NCAlignment( YWidget * parent,
			  YAlignmentType halign,
			  YAlignmentType valign )
    : YAlignment( parent, halign, valign )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAlignment::~NCAlignment
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCAlignment::~NCAlignment()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAlignment::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCAlignment::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YAlignment::setSize( newwidth, newheight);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAlignment::setEnabled
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCAlignment::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YAlignment::setEnabled( do_bv );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAlignment::moveChild
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCAlignment::moveChild( YWidget * child, int newx, int newy )
{
  NCWidget * cw = dynamic_cast<NCWidget*>(child);

  if ( ! ( cw && IsParentOf( *cw ) ) ) {
    NCINT << DLOC << cw << " is not my child" << endl;
    return;
  }

  wMoveChildTo( *cw, wpos( newy, newx ) );
}

