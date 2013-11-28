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

   File:       NCPopupSelection.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupSelection.h"

#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "NCPkgTable.h"
#include "ObjectStatStrategy.h"
#include <zypp/Pattern.h>

#include "YQZypp.h"

#ifdef FIXME
#define LOCALE Y2PM::getPreferredLocale()
#else
#define LOCALE
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::NCPopupSelection
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupSelection::NCPopupSelection( const wpos at,  PackageSelector * pkg, SelType type  )
    : NCPopup( at, false )
    , sel( 0 )
    , okButton( 0 )
    , packager( pkg )
{
    createLayout( YCPString(PkgNames::SelectionLabel()) );

    fillSelectionList( sel, type );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::~NCPopupSelection
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupSelection::~NCPopupSelection()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupSelection::createLayout( const YCPString & label )
{

  YWidgetOpt opt;

  // the vertical split is the (only) child of the dialog
  NCSplit * split = new NCSplit( this, opt, YD_VERT );
  addChild( split );

  opt.notifyMode.setValue( false );

  //the headline
  opt.isHeading.setValue( true );

  NCLabel * head = new NCLabel( split, opt, PkgNames::SelectionLabel() );
  split->addChild( head );

  // add the selection list
  sel = new NCPkgTable( split, opt );
  sel->setPackager( packager );
  // set status strategy
  ObjectStatStrategy * strat = new SelectionStatStrategy();
  sel->setTableType( NCPkgTable::T_Selections, strat );
  sel->fillHeader();
  split->addChild( sel );

  opt.notifyMode.setValue( true );

  NCLabel * help = new NCLabel( split, opt, YCPString(PkgNames::DepsHelpLine()) );
  split->addChild( help );

  split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );

  // add an OK button
  opt.key_Fxx.setValue( 10 );
  okButton = new NCPushButton( split, opt, YCPString(PkgNames::OKLabel()) );
  okButton->setId( PkgNames::OkButton () );

  split->addChild( okButton );

  split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );
}

///////////////////////////////////////////////////////////////////
//
// NCursesEvent & showSelectionPopup ()
//
//
NCursesEvent & NCPopupSelection::showSelectionPopup( )
{
    postevent = NCursesEvent();

    if ( !sel )
	return postevent;

    sel->updateTable();
    sel->setKeyboardFocus();

    // event loop
    do {
	popupDialog();
    } while ( postAgain() );

    popdownDialog();

    if ( !packager )
	return postevent;

    // if OK is clicked get the current item and show the package list
    if ( postevent.detail == NCursesEvent::USERDEF )
    {
	int index = sel->getCurrentItem();
	ZyppObj objPtr = sel->getDataPointer( index );
	if ( objPtr )
	{
	    NCMIL << "Current selection: " << getCurrentLine() << endl;

	    // show the package list
	    std::set<std::string> packages;
            ZyppPattern patPtr = tryCastToZyppPattern (objPtr);
            zypp::Pattern::Contents related ( patPtr->contents() );
            for ( zypp::Pattern::Contents::Selectable_iterator it = related.selectableBegin();
                  it != related.selectableEnd();
                  ++it )
            {
                    ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );
                    if ( zyppPkg )
                    {
                        packages.insert(zyppPkg->name());
                    }
            }

	    packager->showSelPackages( getCurrentLine(), packages );
	    // showDiskSpace() moved to NCPkgTable.cc (show/check diskspace
	    // on every status change)
	    // packager->showDiskSpace();
	}
    }

    return postevent;
}


//////////////////////////////////////////////////////////////////
//
// getCurrentLine()
//
// returns the currently selected list item (may be "really" selected
// or not)
//
string  NCPopupSelection::getCurrentLine( )
{
    if ( !sel )
	return "";

    int index = sel->getCurrentItem();
    ZyppObj selPtr = sel->getDataPointer(index);

    return ( selPtr?selPtr->summary(LOCALE):"" );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupSelection::nicesize(YUIDimension dim)
{
    long vdim;
    if ( NCurses::lines() > 20 )
	vdim = 20;
    else
	vdim = NCurses::lines()-4;
	
    return ( dim == YD_HORIZ ? NCurses::cols()*2/3 : vdim );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupSelection::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupSelection::postAgain( )
{
    if( !postevent.widget )
	return false;

    postevent.detail = NCursesEvent::NODETAIL;

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( !currentId.isNull()
	 && currentId->compare( PkgNames::OkButton () ) == YO_EQUAL )
    {
	postevent.detail = NCursesEvent::USERDEF ;
	// return false means: close the popup
	return false;
    }

    if (postevent == NCursesEvent::cancel)
	return false;

    return true;
}

///////////////////////////////////////////////////////////////////
//
// OrderFuncPattern
//
bool orderPattern( ZyppSel slb1, ZyppSel slb2 )
{
    ZyppPattern ptr1 = tryCastToZyppPattern (slb1->theObj());
    ZyppPattern ptr2 = tryCastToZyppPattern (slb2->theObj());
    if ( !ptr1 || !ptr2 )
	return false;
    else
	return ptr1->order() < ptr2->order();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelection::fillSelectionList
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupSelection::fillSelectionList( NCPkgTable * sel, SelType type  )
{
    if ( !sel )
	return false;

    ZyppPoolIterator i, b, e;
    list<ZyppSel> slbList;
    
    switch ( type )
    {
	case S_Pattern:
	    {
		for ( i = zyppPatternsBegin () ; i != zyppPatternsEnd ();  ++i )
		{
		    ZyppObj resPtr = (*i)->theObj();	    
		    bool show;

		    ZyppPattern patPtr = tryCastToZyppPattern (resPtr);
		    show = patPtr && patPtr->userVisible ();

		    if (show)
		    {
			NCMIL << resPtr->kind () <<": " <<  resPtr->name()
			      << ", initial status: " << (*i)->status() << endl;

			slbList.push_back (*i);
		    }
		}
		slbList.sort( orderPattern );
		break;
	    }
	default:
	    NCERR << "Selecion type not handled: " << type << endl;
    }
    
    list<ZyppSel>::iterator listIt;
    vector<string> pkgLine;
    for ( listIt = slbList.begin(); listIt != slbList.end(); ++listIt )
    {
	ZyppObj resPtr = (*listIt)->theObj();
	pkgLine.clear();
	pkgLine.push_back( resPtr->summary(LOCALE) ); // the description

	sel->addLine( (*listIt)->status(),	// the status
		      pkgLine,
		      resPtr, // ZyppSelection or ZyppPattern
		      (*listIt) ); // ZyppSel
    }
    
    return true;
}
