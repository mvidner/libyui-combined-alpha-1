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

   File:       NCPopupInfo.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#ifndef NCPopupInfo_h
#define NCPopupInfo_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCComboBox.h"
#include "NCLabel.h"
#include "NCRichText.h"
#include "NCCheckBox.h"
#include "NCPushButton.h"
#include "NCi18n.h"


/*
  Textdomain "ncurses"
 */



class NCPopupInfo : public NCPopup
{
private:

    NCPopupInfo & operator=( const NCPopupInfo & );
    NCPopupInfo( const NCPopupInfo & );


    NCRichText * helpText;
    NCPushButton * okButton;
    NCPushButton * cancelButton;

    int hDim;
    int vDim;
    bool visible;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

public:

    NCPopupInfo( const wpos at,
		 const string & headline,
		 const string & text,
		 // the label of an OK button
		 string okButtonLabel = _( "&OK" ),
		 string cancelButtonLabel = "" );

    virtual ~NCPopupInfo();

    virtual int preferredWidth();
    virtual int preferredHeight();

    void createLayout( const string & headline,
		       const string & text,
		       string okButtonLabel,
		       string cancelButtonLabel );

    NCursesEvent & showInfoPopup( );

    void popup();

    void popdown();

    bool isVisible() { return visible; }

    void setPreferredSize( int horiz, int vert ) { hDim = horiz; vDim = vert; }

    void focusOkButton() { okButton->setKeyboardFocus(); }

    void focusCancelButton() { cancelButton->setKeyboardFocus(); }

};



#endif // NCPopupInfo_h
