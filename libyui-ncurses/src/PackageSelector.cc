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

   File:       PackageSelector.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCPushButton.h"
#include "NCTable.h"
#include "NCPkgTable.h"
#include "NCRichText.h"
#include "NCLabel.h"
#include "NCPopupSearch.h"
#include "NCPopupInfo.h"
#include "NCSelectionBox.h"
#include "NCPopupTree.h"
#include "NCMenuButton.h"
#include "NCPopupSelection.h"
#include "NCPopupDeps.h"
#include "PackageSelector.h"
#include "YSelectionBox.h"

#include <iomanip>
#include <list>
#include <string>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMYouPatchManager.h>

#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/YCPParser.h>
#include <ycp/YCPBlock.h>

using namespace std;


///////////////////////////////////////////////////////////////////
//
// CompFunc 
//
bool sortByName( PMSelectablePtr ptr1, PMSelectablePtr ptr2 )
{
    if ( ptr1->name() < ptr2->name() )
    {
	return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////
//
// ignore case compare  
//
bool ic_compare ( char c1, char c2 )
{
    return ( toupper( c1 ) == toupper( c2 ) );
}

///////////////////////////////////////////////////////////////////
//
// Constructor
//
PackageSelector::PackageSelector( Y2NCursesUI * ui, YWidgetOpt & opt )
    : y2ui( ui )
      , visibleInfo( YCPNull() )
      , filterPopup( 0 )
      , depsPopup( 0 )
      , selectionPopup( 0 )
      , youMode( false )
      , updateMode( false )
      , autoCheck( true )
{
    // Fill the handler map
    eventHandlerMap[ PkgNames::Packages()->toString() ]	= &PackageSelector::PackageListHandler;
    eventHandlerMap[ PkgNames::Cancel()->toString() ] 	= &PackageSelector::CancelHandler;
    eventHandlerMap[ PkgNames::OkButton()->toString() ]	= &PackageSelector::OkButtonHandler;
    eventHandlerMap[ PkgNames::Search()->toString() ] 	= &PackageSelector::SearchHandler;
    eventHandlerMap[ PkgNames::Diskspace()->toString() ] = &PackageSelector::DiskspaceHandler;
    // Filter menu
    eventHandlerMap[ PkgNames::RpmGroups()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::Selections()->toString() ] = &PackageSelector::FilterHandler;

    // YOU filter
    eventHandlerMap[ PkgNames::Recommended()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::Security()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::InstalledPatches()->toString() ] = &PackageSelector::FilterHandler;
    eventHandlerMap[ PkgNames::AllPatches()->toString() ] = &PackageSelector::FilterHandler;

    // Information menu
    eventHandlerMap[ PkgNames::Files()->toString() ]   	= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::PkgInfo()->toString() ] 	= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::LongDescr()->toString() ]= &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::Versions()->toString() ] = &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::Relations()->toString() ] = &PackageSelector::InformationHandler;

    // YOU inforamtion 
    eventHandlerMap[ PkgNames::PatchDescr()->toString() ] = &PackageSelector::InformationHandler;
    eventHandlerMap[ PkgNames::PatchPackages()->toString() ] = &PackageSelector::InformationHandler;
    
    // Action menu
    eventHandlerMap[ PkgNames::Toggle()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Select()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Delete()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::Update()->toString() ] 	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::TabooOn()->toString() ]	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::TabooOff()->toString() ]	= &PackageSelector::StatusHandler;
    eventHandlerMap[ PkgNames::ToggleSource()->toString() ] = &PackageSelector::StatusHandler;
    // Etc. menu
    eventHandlerMap[ PkgNames::ShowDeps()->toString() ] = &PackageSelector::DependencyHandler;
    eventHandlerMap[ PkgNames::AutoDeps()->toString() ] = &PackageSelector::DependencyHandler;
    // help menu
    eventHandlerMap[ PkgNames::GeneralHelp()->toString() ] = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::StatusHelp()->toString() ]  = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::FilterHelp()->toString() ]  = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::UpdateHelp()->toString() ] = &PackageSelector::HelpHandler;
    eventHandlerMap[ PkgNames::PatchHelp()->toString() ]  = &PackageSelector::YouHelpHandler;
   
    if ( opt.youMode.value() )
	youMode = true;

    if ( opt.updateMode.value() )
	updateMode = true;

    // NCMIL << "Number of packages: " << Y2PM::packageManager().size() << endl;

    // read test source information
    if ( opt.testMode.value() )
    {
	if ( youMode )
	{
	    Url url( "dir:///8.1-patches" );
	    Y2PM::youPatchManager().instYou().retrievePatchInfo( url, false );
	    Y2PM::youPatchManager().instYou().selectPatches( PMYouPatch::kind_recommended |
							     PMYouPatch::kind_security     );
	    NCMIL <<  "Fake YOU patches initialized" << endl;	
	}
	else
	{
	    Y2PM y2pm;
	    InstSrcManager& MGR = y2pm.instSrcManager();

	    Url url( "dir:///space/instTest" );

	    InstSrcManager::ISrcIdList nids;
	    PMError err = MGR.scanMedia( nids, url );

	    if ( nids.size() )
	    {
		err = MGR.enableSource( *nids.begin() );
		NCMIL << "Fake source enabled: " << err << endl;
	    }
	}
    }    

    if ( !youMode )
    {
	// create the selections popup
	selectionPopup = new NCPopupSelection( wpos( 1, 1 ),
					       this );
	// create the filter popup
	filterPopup = new NCPopupTree( wpos( 1, 1 ),	// position
				       this );	 

    }
    
    depsPopup = new NCPopupDeps( wpos( 1, 1 ), this );
    
    //NCPopupInfo info( wpos( 5, 5 ), YCPString( "Warning" ), text.YCPstr() );
    //info.setNiceSize( 50, 20 );
    //info.showInfoPopup( );

}



///////////////////////////////////////////////////////////////////
//
// Destructor
//
PackageSelector::~PackageSelector()
{
    if ( filterPopup )
    {
	delete filterPopup;
    }
    if ( selectionPopup )
    {
	delete selectionPopup;
    }
    if ( depsPopup )
    {
	delete depsPopup;	
    }
}

///////////////////////////////////////////////////////////////////
//
// handleEvent
//
// Handle event, calls corresponding handler-function
//
bool PackageSelector::handleEvent ( const NCursesEvent&   event )
{
    bool retVal = false;
    YCPValue currentId = YCPNull();

    if ( event == NCursesEvent::handled )
	return false;
    
    // Get the id of the widget which is affected
    if ( event == NCursesEvent::button )
    {
	currentId =  dynamic_cast<YWidget *>(event.widget)->id();
	UIMIL <<  "Selected widget id: " << currentId->toString() << endl;

    }
    else if ( event == NCursesEvent::menu )
    {
	currentId =  event.selection;
	UIMIL <<  "Selected menu item: " << currentId->toString() << endl;
    }
    
    // Find the handler-function for the given widget-nameId
    if ( ! currentId.isNull() )
    {
	tHandlerMap::iterator it = eventHandlerMap.find ( currentId->toString() );
    
	if (it != eventHandlerMap.end())    // if currentId found in map
	{ 
	    tHandlerFctPtr pFct = (*it).second;
	    retVal = (this->*pFct) ( event );    // call handler
	}
	else
	{
	    UIERR <<  "Unhandled event for widget-Id: " << currentId->toString() << endl;
	    
	    // return true means: don't leave the event loop in runPkgSelection  
	    retVal = true;
	}
    }
    else
    {
	UIERR << "Unknown event or id not valid" << endl;
    }

    return retVal;
}


///////////////////////////////////////////////////////////////////
//
// setVisibleInfo
//
// currently displayed package information
//
void PackageSelector::setVisibleInfo( const YCPValue & info )
{
    visibleInfo = info;
}

///////////////////////////////////////////////////////////////////
//
// fillAvailableList
//
// Fills the package table (on bottom ) with the list of available packages 
//
bool PackageSelector::fillAvailableList( NCPkgTable * pkgTable, PMObjectPtr pkgPtr )
{
    vector<string> pkgLine (5);
    unsigned int i = 0;

    if ( !pkgTable )
    {
	NCERR << "No table widget for availbale packages existing" << endl;
	return false;
    }

    if ( !pkgPtr->hasSelectable() )
    {
	NCERR << "Package pointer not valid" << endl;
	return false;
    }
    
    // clear the package table
    pkgTable->itemsCleared ();
    
    // get the selectable
    PMSelectablePtr selectable = pkgPtr->getSelectable();
    NCMIL << "Number of available packages: " << selectable->availableObjs() << endl;
    
    std::list<PMObjectPtr>::const_iterator it = selectable->av_begin();

    // show all availables
    while ( it != selectable->av_end() )
    {
	pkgLine[0] = selectable->name();	// package name
	pkgLine[1] = (*it)->version();		// the version
	pkgLine[2] = (*it)->summary();  	// short description
	FSize size = (*it)->size();     	// installed size
	pkgLine[3] = size.asString();

	pkgTable->addLine( pkgTable->getAvailableStatus(*it), // get the package status
			   pkgLine,
			   i,		 // the index
			   (*it) );	 // the corresponding package pointer
	i++;
	++it;
    }

    // show the package list
    pkgTable->drawList();
    
    if ( pkgTable->getNumLines() > 0 )
    {
	pkgTable->setCurrentItem( 0 );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// showSelPackages
//
// Fills the package table with the list of packages of the given selection
//
bool PackageSelector::showSelPackages( const YCPString & label,  PMSelectionPtr sel )
{
    list<PMSelectablePtr> slcList;
    list<PMSelectablePtr>::iterator listIt;
    unsigned int i;

    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();
    
    if ( sel )
    {
	slcList = sel->inspacks_ptrs ( );
    }

    for ( i = 0, listIt = slcList.begin(); listIt != slcList.end();  ++listIt, i++ )    
    {
	createListEntry( packageList, (*listIt)->theObject(), i );
    }

    // show the package table
    packageList->drawList();
    
    if ( !label.isNull() )
    {
	NCMIL << "  Label: " << label->toString() << endl;

        // show the selected filter label
	YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
	if ( filterLabel )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( label );
	}
    }
   
    return true;

}

///////////////////////////////////////////////////////////////////
//
// fillSearchList
//
// Fills the package table
//
bool PackageSelector::fillSearchList( const YCPString & expr,
				      bool ignoreCase,
				      bool checkDescr )
{
    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList
	 || expr.isNull() )
    {
	return false;
    }

    // clear the package table
    packageList->itemsCleared ();

    PMManager::PMSelectableVec::const_iterator listIt = Y2PM::packageManager().begin();

    // fill the package table
    unsigned int i = 0;
    PMPackagePtr pkgPtr;

    string searchExpr = expr->value();

    while ( listIt != Y2PM::packageManager().end() )
    {
	searchPackage( (*listIt)->theObject(), searchExpr, ignoreCase, checkDescr, i );

	++listIt;
	i++;
    }

    // show the package list
    packageList->drawList();
    
    // set filter label to 'Search'
    YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( PkgNames::SearchResults() );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPatchList
//
// Fills the package table with the list of YOU patches
//
bool PackageSelector::fillPatchList( string filter )
{
    NCPkgTable * packageList = getPackageList();
     
    if ( !packageList )
    {
	UIERR << "No valid NCPkgTable widget" << endl;
    	return false;
    }
    
    // clear the package table
    packageList->itemsCleared ();

    PMManager::PMSelectableVec::const_iterator it = Y2PM::youPatchManager().begin();

    // fill the package table
    unsigned int i;
    PMYouPatchPtr patchPtr;    

    for ( i = 0, it = Y2PM::youPatchManager().begin() ;
	  it != Y2PM::youPatchManager().end();
	  ++it, i++ )
    {
	PMSelectablePtr selectable = *it;

	if ( selectable->installedObj() )
	    patchPtr = selectable->installedObj();
	else if ( selectable->candidateObj() )
	    patchPtr = selectable->candidateObj();
	else
	    patchPtr = selectable->theObject();

	checkPatch( patchPtr, filter, i );
			  
    }

    // show the patches
    packageList->drawList();
    
    // show the selected filter label
    YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( PkgNames::YOUPatches() );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillUpdateList
//
//
bool PackageSelector::fillUpdateList( )
{
    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();

    PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().updateBegin();
    unsigned int i = 0;

    while ( it != Y2PM::packageManager().updateEnd() )
    {
	PMSelectablePtr selectable = *it;
	createListEntry( packageList,
			(*it)->theObject(),
			i );
	    
	++it;
	i++;
    }

    // show the list
    packageList->drawList();
    
    // show the selected filter label
    YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
    if ( filterLabel )
    {
	static_cast<NCLabel *>(filterLabel)->setLabel( PkgNames::UpdateProblem() );
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPatchPackages
//
//
bool PackageSelector::fillPatchPackages ( NCPkgTable * pkgTable, PMObjectPtr objPtr )
{
    PMYouPatchPtr patchPtr = objPtr;

    if ( !pkgTable || !patchPtr )
	return false;

    pkgTable->itemsCleared ();
     
    list<PMPackagePtr> packages = patchPtr->packages();
    list<PMPackagePtr>::const_iterator listIt;
    NCMIL << "Number of patch packages: " << packages.size() << endl;
	
    unsigned int i;
    for ( i = 0, listIt = packages.begin(); listIt != packages.end();  ++listIt, i++ )    
    {
	createListEntry( pkgTable, (*listIt), i );
    }

    // show the list
    pkgTable->drawList();
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// fillPackageList
//
// Fills the package table with the list of packages matching
// the selected filter
//
bool PackageSelector::fillPackageList( const YCPString & label, YStringTreeItem * rpmGroup )
{
     NCPkgTable * packageList = getPackageList();
     
    if ( !packageList )
    {
	UIERR << "No valid NCPkgTable widget" << endl;
    	return false;
    }
    
    // clear the package table
    packageList->itemsCleared ();

    // get the package list and sort it
    list<PMSelectablePtr> pkgList( Y2PM::packageManager().begin(), Y2PM::packageManager().end() );
    pkgList.sort( sortByName );

    // fill the package table
    list<PMSelectablePtr>::iterator listIt;
    unsigned int i;
    PMPackagePtr pkgPtr;


    for ( i = 0, listIt = pkgList.begin(); listIt != pkgList.end();  ++listIt, i++ )
    {
	PMSelectablePtr selectable = *listIt;
	    
	// Multiple instances of this package may or may not be in the same
	// RPM group, so let's check both the installed version (if there
	// is any) and the candidate version.
	//
	// Make sure we emit only one filterMatch() signal if both exist
	// and both are in the same RPM group. We don't want multiple list
	// entries for the same package!
	    
	bool match =
	    checkPackage( selectable->installedObj(), rpmGroup, i ) || 
	    checkPackage( selectable->candidateObj(), rpmGroup, i );  

	// If there is neither an installed nor a candidate package, check
	// any other instance.  
	    
	if ( ! match			&&
	     ! selectable->installedObj()	&&
	     ! selectable->candidateObj()     )
	    checkPackage( selectable->theObject(), rpmGroup, i );

    }

    // show the package list
    packageList->drawList();
    
    if ( !label.isNull() )
    {
	NCMIL <<  "Label: " <<  label->toString() << endl;

	// show the selected filter label
	YWidget * filterLabel = y2ui->widgetWithId( PkgNames::Filter(), true );
	if ( filterLabel )
	{
	    static_cast<NCLabel *>(filterLabel)->setLabel( label );
	}
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// search
//
//
bool PackageSelector::searchPackage( PMPackagePtr pkg,
				     string searchExpr,
				     bool ignoreCase,
				     bool checkDescr,
				     unsigned int index )
{
    bool ret = false;
    
    if ( ! pkg )
	return false;

    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }
    
    string name = pkg->name().asString();

#if 0
    if ( name.find( searchExpr ) == std::string::npos )
    {
	return false;
    }
#endif

    if ( match( name, searchExpr, ignoreCase ) )
    {
	// search sucessful
	createListEntry( packageList, pkg, index );
	ret = true;
    }

    if ( checkDescr )
    {
	string summary = pkg->summary();	
	// additionally search in summary
	if ( match( summary, searchExpr, ignoreCase ) )
	{
	    // search sucessful
	    createListEntry( packageList, pkg, index );
	    ret = true;
	}
    }
    
    return ret;
}

///////////////////////////////////////////////////////////////////
//
// match
//

bool PackageSelector::match ( string s1, string s2, bool ignoreCase )
{
    string::iterator pos;

    if ( ignoreCase )
    {
	pos = search( s1.begin(), s1.end(),
		      s2.begin(), s2.end(),
		      ic_compare );
    }
    else
    {
	pos = search( s1.begin(), s1.end(),
		      s2.begin(), s2.end() );
    }

    return ( pos != s1.end() );
}

///////////////////////////////////////////////////////////////////
//
// checkPackage
//
//
bool PackageSelector::checkPackage( PMPackagePtr pkg,
				    YStringTreeItem * rpmGroup,
				    unsigned int index )
{
    if ( ! pkg || ! rpmGroup )
	return false;

    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }
    
    if ( pkg->group_ptr() == 0 )
    {
	NCERR <<  "NULL pointer in group_ptr()!" << endl;
	return false;
    }

    if ( pkg->group_ptr()->isChildOf( rpmGroup ) )
    {
	createListEntry( packageList, pkg, index );
	
	return true;
    }
    else
    {
	return false;
    }
}

bool PackageSelector::checkPatch( PMYouPatchPtr patchPtr,
				  string filter,
				  unsigned int index )
{

    NCPkgTable * packageList = getPackageList();
    
    if ( !packageList || !patchPtr )
    {
	UIERR << "Widget is not a valid NCPkgTable widget" << endl;
    	return false;
    }

    if ( filter == "all"
	 || filter == patchPtr->kindLabel( patchPtr->kind() )
	 || ( filter == "installed" && patchPtr->getSelectable()->status() == PMSelectable::S_KeepInstalled ) 
	 )
    {
	createPatchEntry( packageList, patchPtr, index );
	return true;
    }
    else
    {
	return false;
    }
}

///////////////////////////////////////////////////////////////////
//
// createListEntry
//
//
bool PackageSelector::createListEntry ( NCPkgTable *pkgTable,
					PMPackagePtr pkgPtr,
					unsigned int index )
{
    vector<string> pkgLine;
    pkgLine.reserve(5);

    if ( !pkgPtr || !pkgTable )
    {
	NCERR << "No valid package available" << endl;
	return false;
    }
    int installedPkgs = Y2PM::instTarget().numPackages();
    if ( installedPkgs > 0 )
    {
	string instVersion = "";
	string version = "";

	pkgLine.push_back( pkgPtr->getSelectable()->name() );	// package name

	if ( pkgPtr->hasInstalledObj() )
	{
	    instVersion = pkgPtr->getInstalledObj()->version();

	    if ( pkgPtr->hasCandidateObj() )
	    {
		version = pkgPtr->getCandidateObj()->version();
	    }
	}
	else
	{
	    version = pkgPtr->version();
	}

	pkgLine.push_back( version );		// available version
	pkgLine.push_back( instVersion );	// installed version

	pkgLine.push_back( pkgPtr->summary() );  // short description
	FSize size = pkgPtr->size();     	// installed size
	pkgLine.push_back( size.asString() );
    }
    else
    {
	pkgLine.push_back( pkgPtr->getSelectable()->name() );	// package name
	pkgLine.push_back( pkgPtr->version() );	// version
	pkgLine.push_back( pkgPtr->summary() );  	// short description
	FSize size = pkgPtr->size();     	// installed size
	pkgLine.push_back( size.asString() );	
    }
    pkgTable->addLine( pkgPtr->getSelectable()->status(), //  get the package status
		       pkgLine,
		       index,		// the index
		       pkgPtr );	// the corresponding package pointer

    return true;
}

///////////////////////////////////////////////////////////////////
//
// createPatchEntry
//
//
bool PackageSelector::createPatchEntry ( NCPkgTable *pkgTable,
					 PMYouPatchPtr patchPtr,
					 unsigned int index )
{
    vector<string> pkgLine(5);
    
    if ( !patchPtr || !pkgTable )
    {
	NCERR << "No valid patch available" << endl;
	return false;
    }

    pkgLine[0] = patchPtr->getSelectable()->name();	  // name
    pkgLine[1] = patchPtr->kindLabel( patchPtr->kind() ); // patch kind
    pkgLine[2] = patchPtr->summary();  	// short description
    FSize size = patchPtr->size();     	// installed size
    pkgLine[3] = size.asString();
    
    pkgTable->addLine( patchPtr->getSelectable()->status(), //  get the status
		       pkgLine,
		       index,		// the index
		       patchPtr );	// the corresponding pointer

    return true;
}

///////////////////////////////////////////////////////////////////
//
// SearchHandler
//
// Opens popup for package search and asks the package manager
//
bool PackageSelector::SearchHandler( const NCursesEvent& event)
{
     NCPkgTable * packageList = getPackageList();
    
    if ( !packageList )
    {
	return false;
    }
    
    // open the search poup and get the search expression
    NCPopupSearch pkgSearch( wpos( 1, 1 ) );

    NCursesEvent retEvent = pkgSearch.showSearchPopup();

	
    if ( !retEvent.result.isNull() )
    {
	bool ignoreCase = true;
	bool checkDescr = false;
	
	YCPString searchExpr = retEvent.result->asString();

	if ( !retEvent.selection.isNull() )
	{
	    YCPBoolean ignore = retEvent.selection->asBoolean();
	    if ( !ignore.isNull()  && (ignore->toString() == "false") )
	    {
		ignoreCase = false;
	    }
	}
	if ( retEvent.detail == NCursesEvent::USERDEF )
	{
	    checkDescr = true;
	}

	NCMIL << "Searching for: " <<  searchExpr->toString() << endl;
	NCMIL << "Ignore Case: " << (ignoreCase?"true":"false") << endl;
	NCMIL << "Check description: " << (checkDescr?"true":"false") << endl;

	// fill the package list with packages matching the search expression
	fillSearchList ( searchExpr,
			 ignoreCase,
			 checkDescr );
	
	showPackageInformation( packageList->getDataPointer( packageList->getCurrentItem() ) );
    }
    else
    {
	NCMIL << "Search is canceled"  << endl;
    }

    packageList->setKeyboardFocus();
    
    return true;
}




///////////////////////////////////////////////////////////////////
//
// InformationHandler
//
// Sets the member variable visibleInfo. If required,
// replaces the currently active package information widget
// ( e.g. by the list of available packages ).
//
bool PackageSelector::InformationHandler( const NCursesEvent&  event )
{
     NCPkgTable * packageList = getPackageList();
     
    if ( !packageList
	 || event.selection.isNull()
	 || visibleInfo.isNull() )
    {
	return false;
    }
    
    if ( visibleInfo->compare( event.selection ) == YO_EQUAL )
    {
	// information selection has not changed
	return true;
    }

    // set visibleInfo
    visibleInfo = event.selection;

    if ( visibleInfo->compare( PkgNames::Versions() ) == YO_EQUAL )
    {
	// show the package table
	const char * tableLayout = "`ReplacePoint( `id(`replaceinfo), `PkgSpecial( `id(`availpkgs), `opt(`notify), \"pkgTable\" ) )"; 
	YCPParser parser( tableLayout );
	YCPValue layout = parser.parse();
	
	y2ui->evaluateReplaceWidget( layout->asTerm() );

	NCPkgTable * pkgAvail = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::AvailPkgs(), true));

	if ( pkgAvail )
	{
	    // set the connection to the PackageSelector !!!!
	    pkgAvail->setPackager( this );
	    // set status strategy
	    ObjectStatStrategy * strategy = new AvailableStatStrategy();
	    pkgAvail->setTableType( NCPkgTable::T_Availables, strategy );
	    // fill the header
	    // pkgAvail->fillHeader( );
	    fillAvailableList( pkgAvail, packageList->getDataPointer( packageList->getCurrentItem() ) );
	}
    }
    else if ( visibleInfo->compare( PkgNames::PatchPackages() ) == YO_EQUAL )
    {
        // show the package table
	const char * tableLayout = "`ReplacePoint( `id(`replaceinfo), `PkgSpecial( `id(`patchpkgs), `opt(`notify), \"pkgTable\" ) )"; 
	YCPParser parser( tableLayout );
	YCPValue layout = parser.parse();
	
	y2ui->evaluateReplaceWidget( layout->asTerm() );

	NCPkgTable * patchPkgs = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::PatchPkgs(), true));

	if ( patchPkgs )
	{
	    // set the connection to the PackageSelector !!!!
	    patchPkgs->setPackager( this );
	    // set status strategy
	    ObjectStatStrategy * strategy = new PatchPkgStatStrategy();
	    patchPkgs->setTableType( NCPkgTable::T_PatchPkgs, strategy );
	    // fill the header
	    //patchPkgs->fillHeader( );
	    fillPatchPackages( patchPkgs, packageList->getDataPointer( packageList->getCurrentItem() ) );
	}	
    }
    else
    {
	// show the rich text widget
	const char * textLayout = "`ReplacePoint( `id(`replaceinfo), `RichText( `id(`description), \" \") )"; 
	YCPParser parser( textLayout );
	YCPValue layout = parser.parse();
	
	y2ui->evaluateReplaceWidget( layout->asTerm() );
    
	showPackageInformation( packageList->getDataPointer( packageList->getCurrentItem() ) );
    }

    packageList->setKeyboardFocus();
    
    UIMIL << "Change package info to: " << visibleInfo->toString() << endl;
    return true;
}

///////////////////////////////////////////////////////////////////
//
// Dependency Handler
//
//
bool PackageSelector::DependencyHandler( const NCursesEvent&  event )
{
    if ( event.selection.isNull() )
    {
	return false;
    }

    if ( event.selection->compare( PkgNames::ShowDeps() ) == YO_EQUAL )
    {
	// show the dependency popup
	showDependencies( true ); 	// do the check
    }
    else if ( event.selection->compare( PkgNames::AutoDeps() ) == YO_EQUAL )
    {
	char menu[300];

	if ( autoCheck )
	{
	    sprintf ( menu,
		      "`ReplacePoint( `id(`replacemenu), `MenuButton( \"&Etc.\", [`item( `id(`showdeps), \"%s\" ), `item( `id(`autodeps), \"%s\" ) ] ) )",
		      PkgNames::MenuCheckDeps().str().c_str(),
		      PkgNames::MenuNoAutoDeps().str().c_str() ); 

	    YCPParser parser( menu );
	    YCPValue layout = parser.parse();
	
	    y2ui->evaluateReplaceWidget( layout->asTerm() );
	    autoCheck = false;
	}
	else
	{
	    sprintf ( menu,
		      "`ReplacePoint( `id(`replacemenu), `MenuButton( \"&Etc.\", [`item( `id(`showdeps), \"%s\" ), `item( `id(`autodeps), \"%s\" ) ] ) )",
		      PkgNames::MenuCheckDeps().str().c_str(),
		      PkgNames::MenuAutoDeps().str().c_str() );  

	    YCPParser parser( menu );
	    YCPValue layout = parser.parse();
	
	    y2ui->evaluateReplaceWidget( layout->asTerm() );	
	    autoCheck = true;	
	}
    }

    NCPkgTable * packageList = getPackageList();
    
    if ( packageList )
    {
	packageList->setKeyboardFocus();
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// FilterHandler
//
// Opens the popup with rpm group tree, selections ... and display the
// corresponding package list
//
bool PackageSelector::FilterHandler( const NCursesEvent&  event )
{
    NCursesEvent retEvent;
    NCPkgTable * packageList = getPackageList();
 
    if ( !packageList
	 || event.selection.isNull() )
    {
	return false;
    }

    if ( event.selection->compare( PkgNames::RpmGroups() ) == YO_EQUAL )
    {
	if ( filterPopup )
	{
	    // show the filter popup (fills the package list) 
	    retEvent = filterPopup->showFilterPopup( );
	}
    }
    else if ( event.selection->compare( PkgNames::Selections() ) == YO_EQUAL )
    {
	if ( selectionPopup )
	{
	    // show the selection popup
	    retEvent = selectionPopup->showSelectionPopup( );
	}
    }
    else if ( event.selection->compare( PkgNames::Recommended() ) ==  YO_EQUAL )
    {
	fillPatchList( "Recommended" );
    }
    else if ( event.selection->compare( PkgNames::Security() )  ==  YO_EQUAL )
    {
	fillPatchList( "Security" );
    }
    else if ( event.selection->compare( PkgNames::AllPatches() )  ==  YO_EQUAL )
    {
	fillPatchList( "all" );
    }
    else if (  event.selection->compare( PkgNames::InstalledPatches() ) ==  YO_EQUAL )
    {
	fillPatchList( "installed" );
    }

    showPackageInformation( packageList->getDataPointer( packageList->getCurrentItem() ) );
    packageList->setKeyboardFocus();	
    
    // return true means: don't leave the event loop in runPkgSelection 
    return true;
}

///////////////////////////////////////////////////////////////////
//
// StatusHandler
//
// Sets the package status
//
bool PackageSelector::StatusHandler( const NCursesEvent&  event )
{
    NCPkgTable * packageList = getPackageList();
     
    if ( !packageList
	 || event.selection.isNull() )
    {
	return false;
    }
    
    if (  packageList->getNumLines() == 0 )
    {
	// nothing to do
	return true;
    }

    // call the corresponding method of NCPkgTable
    if ( event.selection->compare( PkgNames::Toggle() ) == YO_EQUAL )
    {
	packageList->toggleObjStatus( );
    }
    else if ( event.selection->compare( PkgNames::Select() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '+' );
    }
    else if ( event.selection->compare( PkgNames::Delete() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '-' );
    }
    else if ( event.selection->compare( PkgNames::Update() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( '>' );
    }
    else if ( event.selection->compare( PkgNames::TabooOn() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( KEY_F(4) );	
    }
    else if ( event.selection->compare( PkgNames::TabooOff() ) == YO_EQUAL )
    {
	packageList->changeObjStatus( KEY_F(5) );	
    } 
    else if ( event.selection->compare( PkgNames::ToggleSource() ) == YO_EQUAL )
    {
	packageList->toggleSourceStatus( );	
    }
    
    packageList->setKeyboardFocus();
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// PackageListHandler
//
// Handles actions not directly related to the package list.
// 
bool PackageSelector::PackageListHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// DiskinfoHandler
//
// Opens a popup with disk space information.
// 
bool PackageSelector::DiskinfoHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// DiskspaceHandler
// 
// Show the required disk space
//
bool PackageSelector::DiskspaceHandler( const NCursesEvent&  event )
{
    return true;
}

///////////////////////////////////////////////////////////////////
//
// HelpHandler
// 
// Show the help popup
//
bool PackageSelector::HelpHandler( const NCursesEvent&  event )
{
    NCstring text ( "" );

    if ( event.selection.isNull() )
    {
	return false;
    }
    
    if ( event.selection->compare( PkgNames::GeneralHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpPkgInst1();
	text += PkgNames::HelpPkgInst12();
	text += PkgNames::HelpPkgInst13();
	text += PkgNames::HelpPkgInst2();
	text += PkgNames::HelpPkgInst3();
	text += PkgNames::HelpPkgInst4();
	text += PkgNames::HelpPkgInst5();		
    }
    else if ( event.selection->compare( PkgNames::StatusHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpOnStatus1();
	text += PkgNames::HelpOnStatus2();
	text += PkgNames::HelpOnStatus3();
	text += PkgNames::HelpOnStatus();
    }
    else if ( event.selection->compare( PkgNames::UpdateHelp() ) == YO_EQUAL )
    {
	text += PkgNames::HelpOnUpdate();
    }
    // open the popup with the help text
    NCPopupInfo pkgHelp( wpos( 1, 1 ), PkgNames::PackageHelp(), text.YCPstr() );
    pkgHelp.showInfoPopup( );
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// YOUHelpHandler
// 
// Show the required disk space
//
bool PackageSelector::YouHelpHandler( const NCursesEvent&  event )
{
    NCstring text ( "" );

    text += PkgNames::YouHelp1();
    text += PkgNames::YouHelp2();
    text += PkgNames::YouHelp3();

    // open the popup with the help text
    NCPopupInfo youHelp( wpos( 1, 1 ), PkgNames::YouHelp(), text.YCPstr() );
    youHelp.showInfoPopup( );
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// CancelHandler
//
// Cancel button handler.
// 
bool PackageSelector::CancelHandler( const NCursesEvent&  event )
{
    // FIXME - show a popup and ask the user
    // FIXME - restore state

    NCMIL <<  "Cancel button pressed - leaving package selection" << endl;

    const_cast<NCursesEvent &>(event).result = YCPSymbol("cancel", true);
    
    // return false, which means stop the event loop (see runPkgSelection)
    return false;
}

///////////////////////////////////////////////////////////////////
//
// OkButtonHandler
//
// OK button handler.
// 
bool PackageSelector::OkButtonHandler( const NCursesEvent&  event )
{
    // FIXME - do a final dependency check
    // FIXME - check diskspace

    NCMIL <<  "OK button pressed - leaving package selection, starting installation" << endl;

    const_cast<NCursesEvent &>(event).result = YCPSymbol("accept", true); 

    // return false, leave the package selection
    return false;
}

///////////////////////////////////////////////////////////////////
//
// showPatchInformation
//
// Shows the patch information
//
bool PackageSelector::showPatchInformation ( PMObjectPtr objPtr )
{
    PMYouPatchPtr patchPtr = objPtr;

    if ( !patchPtr )
    {
	NCERR << "Package not valid" << endl;
	return false;
    }

    if ( visibleInfo.isNull() )
    {
	NCERR <<  "Visible package information NOT set" << endl;
	return false;	
    }    

    if (  visibleInfo->compare( PkgNames::PatchDescr() ) == YO_EQUAL )
    {
	//  NCstring text ( patchPtr->shortDescription() );
	NCstring text ( patchPtr->longDescription() );
	
	// show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( text.YCPstr() );
	}	
    }
    else if (  visibleInfo->compare( PkgNames::PatchPackages() ) == YO_EQUAL )
    {
	NCPkgTable *patchPkgList  = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::PatchPkgs(), true));
	if ( patchPkgList )
	{
	    fillPatchPackages ( patchPkgList, objPtr);
	}
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// showConcretelyDependency
//
// Shows the dependency of this package
//
bool PackageSelector::showConcretelyDependency ( int index )
{
    if ( depsPopup )
    {
	depsPopup->concretelyDependency( index );
    }

    return true;
}

///////////////////////////////////////////////////////////////////
//
// showDependencies
//
// Checks and shows the dependencies
//
void PackageSelector::showDependencies ( bool doit )
{
    // check dependencies
    if ( depsPopup
	 && (doit || autoCheck) )
    {
	depsPopup->showDependencies();
    }
}

///////////////////////////////////////////////////////////////////
//
// showPackageInformation
//
// Shows the package information
//
bool PackageSelector::showPackageInformation ( PMObjectPtr pkgPtr )
{
    if ( !pkgPtr )
    {
	NCERR << "Package not valid" << endl;
	return false;
    }

    if ( visibleInfo.isNull() )
    {
	NCERR <<  "Visible package information NOT set" << endl;
	return false;	
    }
    
    if ( visibleInfo->compare( PkgNames::LongDescr() ) == YO_EQUAL )
    {
	// ask the package manager for the description of this package
	list<string> value = pkgPtr->description();
	string descr = createDescrText( value );

        // show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(descr) );
	}
    }
    else if (  visibleInfo->compare( PkgNames::Files() ) == YO_EQUAL )
    {
	PMPackagePtr package = pkgPtr;
	if ( package )
	{
	    // get the file list from the package manager/show the list
	    list<string> fileList = package->filenames();
	
	    // get the widget id 
	    YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );  
	    if ( descrInfo  )
	    {
		NCMIL <<  "Size of the file list: " << fileList.size() << endl;
		static_cast<NCRichText *>(descrInfo)->setText( YCPString(createText(fileList, false)) );
	    }
	}
    }
    else if ( visibleInfo->compare( PkgNames::PkgInfo() ) == YO_EQUAL )
    {
	string instVersion = "";
	string version = "";
	string text = "";
	
	if ( pkgPtr->hasSelectable() )
	{
	    text += pkgPtr->getSelectable()->name();
	    text += " - ";
	}
	
	// the summary is UTF8 encoded -> use a YCPString as argument for NCstring  
	text += pkgPtr->summary();
	text += "<br>";

	if ( pkgPtr->hasInstalledObj() && pkgPtr->hasCandidateObj() )
	{
	    instVersion = pkgPtr->getInstalledObj()->version();
	    version = pkgPtr->getCandidateObj()->version();
	}
	else
	{
	    version = pkgPtr->version();
	}

	text += PkgNames::Version().str();
	text +=  version;
	if ( instVersion != "" )
	{
	    text += "  ";
	    text += PkgNames::InstVersion().str();
	    text += instVersion;
	}

	text +=  "  ";
	
	// show the size
	text += PkgNames::Size().str();
	text += pkgPtr->size().asString();
	
	text += "<br>";

	// show Provides:
	text += PkgNames::Provides().str();
	list<PkgRelation> provides = pkgPtr->provides();	// PMSolvable
	text += createRelLine(provides);
	text += "<br>";

	// show the authors
	text += PkgNames::Authors().str();
	PMPackagePtr package = pkgPtr;
	if ( package )
	{
	    list<string> authors = package->authors(); // PMPackage	
	    text += createText( authors, true );
	}
        // show the description	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString(text) );
	}
    }
    else if (  visibleInfo->compare( PkgNames::Versions() ) == YO_EQUAL )
    {
	NCPkgTable * pkgAvail = dynamic_cast<NCPkgTable *>(y2ui->widgetWithId(PkgNames::AvailPkgs(), true));
	if ( pkgAvail )
	{
	    fillAvailableList( pkgAvail, pkgPtr );
	}
    }
    else if (  visibleInfo->compare( PkgNames::Relations() ) == YO_EQUAL )
    {
	string text = "";
	list<PkgRelation> relations;
	
	if ( pkgPtr->hasSelectable() )
	{
	    text += pkgPtr->getSelectable()->name();
	    text += " - ";
	}

	text += pkgPtr->summary();		// the summary
	text += "<br>";

        // show Requires:
	text += "<b>Requires: </b>";
	relations = pkgPtr->requires();

	text += createRelLine(relations);
	text += "<br>";

	// show Required by:
	text += "<b>Prerequires: </b>";
	relations = pkgPtr->prerequires();

	text += createRelLine(relations);
	text += "<br>";

	// show Conflicts:
	text += "<b>Conflicts: </b>";
	relations = pkgPtr->conflicts();

	text += createRelLine(relations);
	
        // show the package relations	
	YWidget * descrInfo = y2ui->widgetWithId( PkgNames::Description(), true );
	
	if ( descrInfo )
	{
	    static_cast<NCRichText *>(descrInfo)->setText( YCPString( text ) );
	}
    }
 
    
    NCDBG <<  "Showing package information: " << visibleInfo->toString() << endl;
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
// createDescrText
//
string PackageSelector::createDescrText( list<string> value )
{
    string html_text = "";
    
    bool auto_format = true;
    list<string>::const_iterator it = value.begin();

    while ( it != value.end() )
    {
	string line = (*it);

	if ( line.substr( 0, 7 ) ==  "Authors" )
	{
	    line = "<b>" + line + "</b>";
	    auto_format = false;
	}

	if ( auto_format )
	{
	    if ( line.length() == 0 )	// Empty lines mean new paragraph
		html_text += "</p><p>";
	    else
	    {
		html_text += line;
		html_text += " ";
	    }
	}
	else
	{
	    html_text += line + "<br>";
	}	
	
	++it;
    }

    html_text += "</p>";

    return html_text;
}

///////////////////////////////////////////////////////////////////
//
// updatePackageList
//
void PackageSelector::updatePackageList()
{
    NCPkgTable * packageList = getPackageList();

    if ( packageList )
    {
	packageList->updateTable();
    }
}

///////////////////////////////////////////////////////////////////
//
// createProvides
//
string PackageSelector::createRelLine( list<PkgRelation> info )
{
    list<PkgRelation>::iterator it;
    string text = "";
    unsigned int i;
    
    for ( i = 0, it = info.begin(); it != info.end(); ++it, i++ )
    {
	text = text + (*it).asString();
	if ( i < info.size()-1 )
	{
	    text = text + ", ";
	}
    }

    return text;
}


///////////////////////////////////////////////////////////////////
//
// createText
//
string PackageSelector::createText( list<string> info, bool oneline )
{
    list<string>::iterator it;
    string text = "";
    unsigned int i;
    
    for ( i = 0, it = info.begin(); it != info.end(); ++it, i++ )
    {
	text = text + (*it);
	if ( i < info.size()-1 )
	{
	    if ( oneline )
	    {
		text = text + ", ";
	    }
	    else
	    {
		text = text + "<br>";	
	    }
	}
    }

    return text;
}

///////////////////////////////////////////////////////////////////
//
// getPackageList()
//
NCPkgTable * PackageSelector::getPackageList()
{
	YWidget * packages = y2ui->widgetWithId( PkgNames::Packages(), true );
	
	return ( dynamic_cast<NCPkgTable *>(packages) );
}
