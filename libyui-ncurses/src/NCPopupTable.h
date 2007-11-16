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

   File:       NCPopupTable.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupTable_h
#define NCPopupTable_h

#include <iosfwd>

#include <vector>
#include <string>
using namespace std;

#include "NCPopup.h"
#include "YTableItem.h"

class NCTable;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupTable
//
//	DESCRIPTION :
//
class NCPopupTable : public NCPopup {

  NCPopupTable & operator=( const NCPopupTable & );
  NCPopupTable            ( const NCPopupTable & );

  private:

    NCTable * sellist;
    
  protected:

    void createList( vector<string> & row );

    void addItem( YItem *yitem );

    void setCurrentItem( int index );
    int  getCurrentItem() const;
    YItem * getCurrentItemPointer( ) const;
    
    virtual NCursesEvent wHandleHotkey( wint_t ch );

  protected:

    NCPopupTable( const wpos at );
    virtual ~NCPopupTable();

    virtual bool postAgain();
    
public:
  void stripHotkeys();
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupTable_h
