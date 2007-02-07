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

   File:       NCComboBox.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include <climits>

#include "Y2Log.h"
#include "NCurses.h"
#include "NCComboBox.h"
#include "NCPopupList.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::NCComboBox
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCComboBox::NCComboBox( NCWidget * parent, const YWidgetOpt & opt,
			const YCPString & nlabel )
    : YComboBox( opt, nlabel )
    , NCWidget( parent )
    , mayedit( opt.isEditable.value() )
    , text( "" )
    , lwin( 0 )
    , twin( 0 )
    , fldstart ( 0 )
    , fldlength( 0 )
    , curpos( 0 )
    , index( -1 )
    , InputMaxLength( -1 )
{
  WIDDBG << endl;
  setLabel( nlabel );
  hotlabel = &label;
  setValue( YCPString( string("") ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::~NCComboBox
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCComboBox::~NCComboBox()
{
  delete lwin;
  delete twin;
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCComboBox::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YComboBox::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::setDefsze
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::setDefsze()
{
  defsze = wsze( label.height() + 1,
		 label.width() < 5 ? 5 : label.width() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::wCreate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::wCreate( const wrect & newrect )
{
  NCWidget::wCreate( newrect );

  if ( !win )
     return;

  wrect lrect( 0, wsze::min( newrect.Sze,
			     wsze( label.height(), newrect.Sze.W ) ) );
  wrect trect( 0, wsze( 1, newrect.Sze.W ) );

  if ( lrect.Sze.H == newrect.Sze.H )
    lrect.Sze.H -= 1;

  trect.Pos.L = lrect.Sze.H > 0 ? lrect.Sze.H : 0;

  lwin = new NCursesWindow( *win,
			    lrect.Sze.H, lrect.Sze.W,
			    lrect.Pos.L, lrect.Pos.C,
			    'r' );
  twin = new NCursesWindow( *win,
			    trect.Sze.H, trect.Sze.W,
			    trect.Pos.L, trect.Pos.C,
			    'r' );
  fldlength = trect.Sze.W ? trect.Sze.W - 1 : 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::wDelete
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::wDelete()
{
  delete lwin;
  delete twin;
  lwin = 0;
  twin = 0;
  NCWidget::wDelete();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::itemAdded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::itemAdded( const YCPString & ntext,
			    int idx,
			    bool selected )
{
  deflist.push_back( ntext );
  if ( selected || index == -1 ) {
    setValue( ntext );
    index = idx;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::setLabel( const YCPString & nlabel )
{
  label = NCstring( nlabel );
  label.stripHotkey();
  setDefsze();
  YComboBox::setLabel( nlabel );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::setCurrentItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::setCurrentItem( int nindex )
{
  int idx = 0;
  list<YCPString>::const_iterator entry;
  for ( entry = deflist.begin(); entry != deflist.end(); ++entry, ++idx ) {
    if ( idx == nindex ) {
      string strip( (*entry)->value() );
      string::size_type h = strip.find( '&' );
      if ( h != string::npos )
	strip.erase( h, 1 );
      setValue( YCPString( strip ) );
      index = idx;
      break;
    }
  }
  Redraw();
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::getCurrentItem
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCComboBox::getCurrentItem() const
{
  return index;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::setValue
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::setValue( const YCPString & ntext )
{
  text     = ntext;
  buffer   = text.str();
  modified = false;
  fldstart = 0;
  curpos   = mayedit ? buffer.length() : 0;
  index = -1;
  setDefsze();
  tUpdate();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::getValue
//	METHOD TYPE : YCPString
//
//	DESCRIPTION :
//
YCPString NCComboBox::getValue() const
{
  if ( modified )
    return NCstring( buffer ).YCPstr();

  if ( index != -1 ) {
    int idx = 0;
    list<YCPString>::const_iterator entry;
    for ( entry = deflist.begin(); entry != deflist.end(); ++entry, ++idx ) {
      if ( idx == index ) {
	return *entry;
      }
    }
  }

  return text.YCPstr();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::setValidChars
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::setValidChars( const YCPString & validchars )
{
  validChars = validchars;
  YComboBox::setValidChars( validchars );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::validKey
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCComboBox::validKey( wint_t key ) const
{
  // private: NCstring validChars;
  const wstring vwch( validChars.str() );

  if ( vwch.empty() )		// usually empty -> return true
    return true;

  if ( key < 0 || WCHAR_MAX < key )
    return false;

  return( vwch.find( (wchar_t)key ) != wstring::npos );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::wRecoded()
{
  if ( modified ) {
    text     = NCstring( buffer );
    modified = false;
  }
  buffer = text.str();
  wRedraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::wRedraw()
{
  if ( !win )
    return;

  // label
  const NCstyle::StWidget & style( widgetStyle( true ) );
  lwin->bkgd( style.plain );
  lwin->clear();
  label.drawAt( *lwin, style );
  tUpdate();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::tUpdate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCComboBox::tUpdate()
{
  if ( !win )
    return;

  const wstring & str( buffer );

  if ( curpos > str.length() ) {
    curpos = str.length();
  }

  // adjust fldstart that cursor is visible
  if ( str.length() >= fldlength ) {
    if ( curpos <= fldstart ) {
      fldstart = curpos ? curpos - 1 : 0;
    }
    if ( curpos >= fldstart + fldlength - 1 ) {
      fldstart = curpos + (curpos == str.length() ? 1 : 2) - fldlength;
    }
  } else if ( fldstart ) {
    fldstart = 0;
  }

  const NCstyle::StWidget & style( widgetStyle() );
  twin->bkgd( widgetStyle( true ).plain );
  twin->move( 0, 0 );

  if ( fldlength ) {
      unsigned i      = 0;
      unsigned end    = fldlength;
      const wchar_t * cp = str.data() + fldstart;
      
      // draw left scrollhint if
      if ( *cp && fldstart ) {
	  twin->bkgdset( style.scrl );
	  twin->addch( ACS_LARROW );
	  ++i;
	  ++cp;
      }

      // check for right scrollhint
      if ( fldstart + fldlength <= str.length() ) {
	  --end;
      }

      // draw field
      twin->bkgdset( style.data );
      for ( /*adjusted i*/; *cp && i < end; ++i )
      {
	  twin->addwstr( cp, 1 );
	  cp++;
      }

      twin->bkgdset( style.plain );
      for ( /*adjusted i*/; i < end; ++i )
      {
	  twin->addch( ACS_CKBOARD );
      }

      // draw right scrollhints
      twin->bkgdset( style.scrl );
      if ( end < fldlength )
      {
	  twin->addch( ACS_RARROW );
      }
  }
  twin->addch( 0, twin->maxx(), ACS_DARROW );

  if ( mayedit && GetState() == NC::WSactive )
  {
    twin->move( 0, curpos - fldstart );
    twin->bkgdset( wStyle().cursor );
    if (  curpos < buffer.length() )
	twin->add_attr_char( );
    else
	twin->addch( ACS_CKBOARD );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCComboBox::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  bool   beep   = false;
  bool   update = true;
  wstring oval = buffer;

  switch ( key ) {

  case KEY_BACKSPACE:
    if ( mayedit && curpos ) {
      buffer.erase( --curpos, 1 );
      modified = true;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_DC:
    if ( mayedit && curpos < buffer.length() ) {
      buffer.erase( curpos, 1 );
      modified = true;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_HOME:
    if ( curpos && ( mayedit || fldstart ) ) {
      curpos = 0;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_END:
    if ( curpos < buffer.length() && ( mayedit || fldstart + fldlength <= buffer.length() ) ) {
      curpos = buffer.length();
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_LEFT:
    if ( curpos ) {
      if ( mayedit )
	--curpos;
      else if ( fldstart )
	curpos = fldstart - 1;
      else {
	update = false;
	beep   = true;
      }
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_RIGHT:
    if ( mayedit && curpos < buffer.length() ) {
      ++curpos;
    } else if ( fldstart + fldlength <= buffer.length() ) {
      curpos = fldstart + fldlength;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_HOTKEY:
    if ( mayedit )
      break;
    // else fallthrough
  case KEY_DOWN:
    listPopup();
    break;

  default:
    bool is_special = false;
    if ( key > 0xFFFF )
    {
       is_special = true;
       key -= 0xFFFF;
    }
    
    if ( !mayedit || !validKey( key )
	 ||
	 ( !is_special && KEY_MIN < key && KEY_MAX > key )
	 ||
	 !iswprint( key )
	 ||
	 // if we are at limit of input
	 (InputMaxLength >= 0 && InputMaxLength <= (int)buffer.length()) )
    {
	    update = false;
	    beep   = true;
    } else {
      buffer.insert( curpos, 1, key );
      modified = true;
      ++curpos;
    }
    break;
  }

  if ( update )
    tUpdate();

  if ( beep )
    ::beep();

  if ( getNotify() && oval != buffer )
    ret = NCursesEvent::ValueChanged;

  return ret;

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::listPopup
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCComboBox::listPopup()
{
  if (!deflist.empty()) {
    wpos        at( ScreenPos() + wpos( win->height(), -1 ) );
    NCPopupList dialog( at, YCPString(""), deflist, index );
    int         idx = dialog.post();
    if ( idx != -1 )
      setCurrentItem( idx );
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCComboBox::deleteAllItems
//	METHOD TYPE : void
//
//	DESCRIPTION : cleares the table and the lists holding
//                    the values
//
void NCComboBox::deleteAllItems() {
	YComboBox::deleteAllItems();
	deflist.clear();
	setValue( YCPString( string("") ) );
}


void NCComboBox::setInputMaxLength( const YCPInteger & numberOfChars)
{
    int nr = numberOfChars->asInteger()->value();

    // if there is more text then the maximum number of chars,
    // truncate the text and update the buffer
    if ( nr >= 0 && (int)buffer.length() > nr ) {
	buffer.erase( nr, buffer.length() - nr );
	tUpdate();
	curpos = buffer.length();
    }

    InputMaxLength;
#warning FIXME: Either throw this out or do something properly with it!
}
