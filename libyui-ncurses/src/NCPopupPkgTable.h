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

   File:       NCPopupPkgTable.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupPkgTable_h
#define NCPopupPkgTable_h

#include <iosfwd>

#include <vector>
#include <string>
#include <algorithm>

#include "NCPopup.h"

class NCPkgTable;
class NCPushButton;
class PackageSelector;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupPkgTable
//
//	DESCRIPTION :
//
class NCPopupPkgTable : public NCPopup {

    NCPopupPkgTable & operator=( const NCPopupPkgTable & );
    NCPopupPkgTable            ( const NCPopupPkgTable & );

private:

    NCPkgTable * pkgTable;
    NCPushButton * okButton;
    NCPushButton * cancelButton;
    PackageSelector * packager;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPopupPkgTable( const wpos at, PackageSelector * pkger );
    
    virtual ~NCPopupPkgTable();

    virtual long nicesize(YUIDimension dim);

    bool fillAutoChanges( NCPkgTable * pkgTable );

    void createLayout( );

    NCursesEvent showInfoPopup( );

};

///////////////////////////////////////////////////////////////////


#endif // NCPopupPkgTable_h
