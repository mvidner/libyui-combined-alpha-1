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

   File:       NCMultiSelectionBox.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCMultiSelectionBox.h"


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::NCMultiSelectionBox
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCMultiSelectionBox::NCMultiSelectionBox( YWidget * parent,
					  const string & nlabel )
    : YMultiSelectionBox( parent, nlabel )
    , NCPadWidget( parent )
{
  WIDDBG << endl;
  InitPad();
  setLabel( nlabel );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::~NCMultiSelectionBox
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCMultiSelectionBox::~NCMultiSelectionBox()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::preferredWidth
//	METHOD TYPE : void
//
//	DESCRIPTION : Set preferred width
//
int NCMultiSelectionBox::preferredWidth()
{
    return wGetDefsze().W;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::preferredHeight
//	METHOD TYPE : void
//
//	DESCRIPTION : Set preferred height
//
int NCMultiSelectionBox::preferredHeight()
{
    return wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::setEnabled
//	METHOD TYPE : void
//
//	DESCRIPTION : Set widget state (enabled/disabled)
//
void NCMultiSelectionBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YMultiSelectionBox::setEnabled( do_bv );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION : nc
//
void NCMultiSelectionBox::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::currentItem
//	METHOD TYPE : YItem *
//
//	DESCRIPTION : Return pointer to current MsB item
//
YItem * NCMultiSelectionBox::currentItem()
{
    if ( !myPad()->Lines() )
	return 0; 

    int index = myPad()->CurPos().L;

    return itemAt( index );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::setCurrentItem
//	METHOD TYPE : void
//
//	DESCRIPTION : Highlight current item
//
void NCMultiSelectionBox::setCurrentItem( YItem * item )
{
    myPad()->ScrlLine( item->index() );
}

void NCMultiSelectionBox::addItem( YItem * item )
{
    vector<NCTableCol*> Items( 2U, 0 );
    if ( item )
    {
	YMultiSelectionBox::addItem( item );
	Items[0] = new NCTableTag( item, item->selected() );
	//Do not set style to NCTableCol::PLAIN here, otherwise current
        //item will not be highlighted if cursor is not over the widget 
	Items[1] = new NCTableCol( item->label() );
	myPad()->Append( Items );
	DrawPad();	
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::tagCell
//	METHOD TYPE : NCTableTag *
//
//	DESCRIPTION : Return pointer to current line tag
//		      (holds state and yitem pointer)
//
NCTableTag * NCMultiSelectionBox::tagCell( int index )
{
  NCTableLine * cl = myPad()->ModifyLine( index );
  if ( !cl )
    return 0;
  return static_cast<NCTableTag *>(cl->GetCol( 0 ));
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::tagCell
//	METHOD TYPE : const NCTableTag *
//
//	DESCRIPTION : dtto
//
const NCTableTag * NCMultiSelectionBox::tagCell( int index ) const
{
  const NCTableLine * cl = myPad()->GetLine( index );
  if ( !cl )
    return 0;
  return static_cast<const NCTableTag *>(cl->GetCol( 0 ));
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::deleteAllItems
//	METHOD TYPE : void
//
//	DESCRIPTION : Clear the MsB
//
void NCMultiSelectionBox::deleteAllItems()
{
  YMultiSelectionBox::deleteAllItems();
  myPad()->ClearTable();
  DrawPad();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::isItemSelected
//	METHOD TYPE : bool
//
//	DESCRIPTION : Return item status (selected/deselected)
//
bool NCMultiSelectionBox::isItemSelected( YItem *item ) 
{
  return item->selected();
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::selectItem
//	METHOD TYPE : bool
//
//	DESCRIPTION : Mark item as selected
//
void NCMultiSelectionBox::selectItem( YItem *yitem, bool selected )
{
  YMultiSelectionBox::selectItem ( yitem, selected );

  //retrieve pointer to the line tag associated with this item
  NCTableTag * tag = (NCTableTag *)yitem->data(); 
  YUI_CHECK_PTR( tag );

  tag->SetSelected( selected );

  DrawPad();

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::selectItem
//	METHOD TYPE : bool
//
//	DESCRIPTION : Mark all items as deselected
//
void NCMultiSelectionBox::deselectAllItems()
{
  YMultiSelectionBox::deselectAllItems();

  for ( unsigned int i = 0; i < getNumLines(); i++)
  {
     NCTableTag *t = tagCell( i );
     t->SetSelected( false );
  }  
  DrawPad();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::toggleCurrentItem
//	METHOD TYPE : void
//
//	DESCRIPTION : Toggle item from selected -> deselected
//		      and vice versa
//
void NCMultiSelectionBox::toggleCurrentItem()
{
  YItem *it = currentItem();
  selectItem( it, !( it->selected()) );

}

////////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :  nc
//
void NCMultiSelectionBox::setLabel( const string & nlabel )
{
  YMultiSelectionBox::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::CreatePad
//	METHOD TYPE : NCPad *
//
//	DESCRIPTION : Create empty MsB pad
//
NCPad * NCMultiSelectionBox::CreatePad()
{
  wsze psze( defPadSze() );
  NCTablePad * npad = new NCTablePad( psze.H, psze.W, *this );
  npad->bkgd( listStyle().item.plain );
  npad->SetSepChar( ' ' );
  return npad;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiSelectionBox::wRecoded()
{
  NCPadWidget::wRecoded();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION : Handle input :)
//		      (those keys that are not caught by NCPad)
//
NCursesEvent NCMultiSelectionBox::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  bool valueChanged = false;
  YItem *oldCurrentItem = currentItem();

  if ( ! handleInput( key ) )
  {
    YItem *citem = currentItem();

    switch ( key ) {
    case KEY_SPACE:
    case KEY_RETURN:
      toggleCurrentItem();
      valueChanged = true;
      break;
    case '+':
      if ( !isItemSelected( citem ) ) {
	selectItem( citem, true );
        valueChanged = true;
      }
      myPad()->ScrlDown();
      break;
    case '-':
      if ( isItemSelected( citem ) ) {
	selectItem( citem, false);
        valueChanged = true;
      }
      myPad()->ScrlDown();
      break;
    }
  }

  if ( notify() )
  {
    if ( valueChanged )
      ret = NCursesEvent::ValueChanged;
    else if ( oldCurrentItem != currentItem() )
      ret = NCursesEvent::SelectionChanged;
  }

  return ret;
}
