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

   File:       NCPkgFilterMain.h

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/

#ifndef NCPkgSearchSettings_h
#define NCPkgSearchSettings_h

#include <string>

#include "NCurses.h"
#include "NCi18n.h"
#include "NCMultiSelectionBox.h"

class NCPkgSearchSettings : public NCMultiSelectionBox {

    NCPkgSearchSettings & operator=( const NCPkgSearchSettings & );
    NCPkgSearchSettings             ( const NCPkgSearchSettings & );

public:

    YItemCollection items;
    YItem *checkName;
    YItem *checkSummary;
    YItem *checkKeywords;
    YItem *checkDescr;
    YItem *checkProvides;
    YItem *checkRequires; 


   NCPkgSearchSettings (YWidget *parent, string label);
   virtual ~NCPkgSearchSettings();   

   void createLayout();

   bool doCheckName();
   bool doCheckSummary();
   bool doCheckKeywords();
   bool doCheckDescr();
   bool doCheckProvides();
   bool doCheckRequires();

};

#endif
