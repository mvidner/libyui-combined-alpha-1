/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQPkgInstSrcList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <Y2PM.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcData.h>
#include <y2pm/PMSelectable.h>
#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgInstSrcList.h"

using std::list;
using std::set;


YQPkgInstSrcList::YQPkgInstSrcList( QWidget * parent )
    : QY2ListView( parent )
{
    y2debug( "Creating inst source list" );

    _nameCol	= -1;
    _urlCol	= -1;

    int numCol = 0;

    // Column headers for installation source list
    addColumn( _( "Name"	) );	_nameCol	= numCol++;
    addColumn( _( "URL"		) );	_urlCol		= numCol++;

    setAllColumnsShowFocus( true );
    setSelectionMode( QListView::Extended );	// allow multi-selection with Ctrl-mouse

    connect( this, 	SIGNAL( selectionChanged() ),
	     this, 	SLOT  ( filter()           ) );

    fillList();
    selectSomething();

    y2debug( "Creating inst source list done" );
}


YQPkgInstSrcList::~YQPkgInstSrcList()
{
    // NOP
}


void
YQPkgInstSrcList::fillList()
{
    clear();
    y2debug( "Filling inst source list" );

    InstSrcManager::ISrcIdList activeSources( Y2PM::instSrcManager().getSources( true ) ); // enabled only
    InstSrcManager::ISrcIdList::const_iterator it = activeSources.begin();

    while ( it != activeSources.end() )
    {
	InstSrcManager::ISrcId instSrcId = (*it);
	
	if ( instSrcId &&
	     instSrcId->enabled() )	// should always be true if querying only enabled sources
	{
	    addInstSrc( instSrcId );
	}

	++it;
    }

    y2debug( "Inst source list filled" );
}


void
YQPkgInstSrcList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgInstSrcList::filter()
{
    emit filterStart();


    //
    // Collect matching selectables for all selected inst sources
    // and store them in a set to avoid duplicates in the resulting list
    //
    
    set<PMSelectablePtr> matches;		// avoid duplicates in list
    QListViewItem * item = firstChild();	// take multi selection into account
    
    while ( item )
    {
	if ( item->isSelected() )
	{
	    YQPkgInstSrcListItem * instSrcItem = dynamic_cast<YQPkgInstSrcListItem *> (item);

	    if ( instSrcItem )
	    {
		InstSrcManager::ISrcId instSrc = instSrcItem->instSrcId();
		const list<PMPackagePtr> &packages = instSrc->data()->getPackages();
		list<PMPackagePtr>::const_iterator pkg_it = packages.begin();

		while ( pkg_it != packages.end() )
		{
		    if ( (*pkg_it)->hasSelectable() ) // might not be in manager (incompatible arch)
			matches.insert( (*pkg_it)->getSelectable() );

		    ++pkg_it;
		}
	    }
	}
	    
	item = item->nextSibling();
    }


    //
    // Send all members of the resulting set to the list
    // (emit a filterMatch signal for each one)
    //

    set<PMSelectablePtr>::const_iterator sel_it = matches.begin();

    while ( sel_it != matches.end() )
    {
	emit filterMatch( (*sel_it)->theObject() );
	++sel_it;
    }
    

    emit filterFinished();
}


void
YQPkgInstSrcList::addInstSrc( InstSrcManager::ISrcId instSrcId )
{
    if ( ! instSrcId )
    {
	y2error( "NULL instSrcId !" );
	return;
    }

    new YQPkgInstSrcListItem( this, instSrcId );
}


YQPkgInstSrcListItem *
YQPkgInstSrcList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgInstSrcListItem *> (item);
}






YQPkgInstSrcListItem::YQPkgInstSrcListItem( YQPkgInstSrcList *		instSrcList,
					    InstSrcManager::ISrcId 	instSrcId )
    : QY2ListViewItem( instSrcList )
    , _instSrcList( instSrcList )
    , _instSrcId( instSrcId )
{
    if ( ! instSrcId )
    {
	y2error( "Null instSrcId" );
	return;
    }

    if ( instSrcDescr() )
    {
	if ( nameCol() >= 0 )
	{
	    setText( nameCol(), instSrcDescr()->shortlabel().c_str() );
	}
    
	if ( urlCol() >= 0 )
	{
	    setText( urlCol(), instSrcDescr()->url().asString( true, false, false ).c_str() );
	}
    }
}



YQPkgInstSrcListItem::~YQPkgInstSrcListItem()
{
    // NOP
}


#include "YQPkgInstSrcList.moc"
