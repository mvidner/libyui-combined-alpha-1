/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/


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

   File:       NCPkgPopupDescr.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPkgPopupDescr_h
#define NCPkgPopupDescr_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"

class NCPkgTable;
class NCPushButton;
class NCRichText;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupDescr
//
//	DESCRIPTION :
//
class NCPkgPopupDescr : public NCPopup
{
    NCPkgPopupDescr & operator=( const NCPkgPopupDescr & );
    NCPkgPopupDescr            ( const NCPkgPopupDescr & );

private:

    NCPkgTable * pkgTable;
    NCPushButton * okButton;
    NCRichText *descrText;
    NCLabel *headline;

    NCPackageSelector * packager;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

public:

    NCPkgPopupDescr( const wpos at, NCPackageSelector * pkger );

    virtual ~NCPkgPopupDescr();

    virtual long nicesize(YUIDimension dim);

    bool fillData( ZyppPkg pkgPtr, ZyppSel slbPtr );

    void createLayout();

    NCursesEvent showInfoPopup( ZyppPkg pkgPtr, ZyppSel slbPtr );

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupDescr_h
