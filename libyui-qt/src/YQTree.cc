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

  File:	      YQTree.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#include <QColorGroup>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QString>
//Added by qt3to4:
#include <QPixmap>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

using std::min;
using std::max;

#include "YQUI.h"
#include "YEvent.h"
#include "utf8.h"
#include "YQTree.h"
#include "YTreeItem.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"

#define VERBOSE_TREE_ITEMS	0


YQTree::YQTree( YWidget * parent, const string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YTree( parent, label )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _nextSerialNo = 0;

    _caption     = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_listView = new QTreeWidget( this );
    YUI_CHECK_NEW( _qt_listView );
    layout->addWidget( _qt_listView );

     //_qt_listView->setHeaderLabel("");
//     _qt_listView->addColumn( "" );
     _qt_listView->header()->hide();
     // _qt_listView->setHeader(0L);
     _qt_listView->setRootIsDecorated ( true );

    _caption->setBuddy ( _qt_listView );

    connect( _qt_listView,	SIGNAL( itemSelectionChanged () ),
	     this, 		SLOT  ( slotSelectionChanged () ) );

    connect( _qt_listView,	SIGNAL( itemActivated	 ( QTreeWidgetItem * , int ) ),
	     this, 		SLOT  ( slotActivated	 ( QTreeWidgetItem *	) ) );

    connect( _qt_listView,	SIGNAL( itemDoubleClicked( QTreeWidgetItem *, int ) ),
	     this, 		SLOT  ( slotActivated	 ( QTreeWidgetItem *	) ) );
}


YQTree::~YQTree()
{
    // NOP
}


void YQTree::setLabel( const string & label )
{
    _caption->setText( label );
    YTree::setLabel( label );
}


void YQTree::rebuildTree()
{
    // yuiDebug() << "Rebuilding tree" << endl;

    YQSignalBlocker sigBlocker( _qt_listView );
    _qt_listView->clear();

    buildDisplayTree( 0, itemsBegin(), itemsEnd() );
}


void YQTree::buildDisplayTree( YQTreeItem * parentItem, YItemIterator begin, YItemIterator end )
{
    for ( YItemIterator it = begin; it < end; ++it )
    {
	YTreeItem * orig = dynamic_cast<YTreeItem *> (*it);
	YUI_CHECK_PTR( orig );

	YQTreeItem * clone;

	if ( parentItem )
	    clone = new YQTreeItem( this, parentItem, orig, _nextSerialNo++ );
	else
	    clone = new YQTreeItem( this, _qt_listView, orig, _nextSerialNo++ );

	YUI_CHECK_NEW( clone );

	if ( orig->hasChildren() )
	    buildDisplayTree( clone, orig->childrenBegin(), orig->childrenEnd() );
    }
}


void YQTree::selectItem( YItem * yItem, bool selected )
{
    YQSignalBlocker sigBlocker( _qt_listView );

    YTreeItem * treeItem = dynamic_cast<YTreeItem *> (yItem);
    YUI_CHECK_PTR( treeItem );

    YQTreeItem * yqTreeItem = (YQTreeItem *) treeItem->data();
    YUI_CHECK_PTR( yqTreeItem );

    if ( ! selected && yqTreeItem == _qt_listView->currentItem() )
    {
	deselectAllItems();
    }
    else
    {
	yqTreeItem->setSelected( true );
	openBranch( yqTreeItem );
	YTree::selectItem( treeItem, selected );
    }
}


void YQTree::selectItem( YQTreeItem * item )
{
    if ( item )
    {
	YQSignalBlocker sigBlocker( _qt_listView );

	item->setSelected( true );
	openBranch( item );
	YTree::selectItem( item->origItem(), true );

	yuiDebug() << "selected item: \"" << item->origItem()->label() << "\"" << endl;
    }
}


void YQTree::openBranch( QTreeWidgetItem * item )
{
    while ( item )
    {
      item->setExpanded( true ); // Takes care of origItem()->setOpen()
      item = item->parent();
    }
}


void YQTree::deselectAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listView );

    YTree::deselectAllItems();
    _qt_listView->clearSelection();
}


void YQTree::deleteAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listView );

    _qt_listView->clear();
    YTree::deleteAllItems();
}


void YQTree::slotSelectionChanged( )
{
    QList<QTreeWidgetItem *> items = _qt_listView->selectedItems ();
    QTreeWidgetItem *qItem = items.first();
    selectItem( dynamic_cast<YQTreeItem *> (qItem) );

    if ( notify() && ! YQUI::ui()->eventPendingFor( this ) )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
}


void YQTree::slotActivated( QTreeWidgetItem * qItem )
{
    selectItem( dynamic_cast<YQTreeItem *> (qItem) );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


int YQTree::preferredWidth()
{
    int hintWidth = !_caption->isHidden() ? _caption->sizeHint().width() : 0;
    return max( 200, hintWidth );
}


int YQTree::preferredHeight()
{
    // 300 is an arbitrary value.  Use a MinSize or MinHeight widget to set a
    // size that is useful for the application.

    int hintHeight = !_caption->isHidden() ? _caption->sizeHint().height() : 0;

    return 300 + hintHeight;
}


void YQTree::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQTree::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_listView->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


bool YQTree::setKeyboardFocus()
{
    _qt_listView->setFocus();

    return true;
}


/*============================================================================*/



YQTreeItem::YQTreeItem( YQTree	*	tree,
			QTreeWidget *	listView,
			YTreeItem *	orig,
			int		serial )
    : QTreeWidgetItem( listView )
{
    init( tree, orig, serial );

#if VERBOSE_TREE_ITEMS
    yuiDebug() << "Creating toplevel tree item \"" << orig->label() << "\"" << endl;
#endif
}


YQTreeItem::YQTreeItem( YQTree	*	tree,
			YQTreeItem *	parentItem,
			YTreeItem *	orig,
			int		serial )
    : QTreeWidgetItem( parentItem )
{
    init( tree, orig, serial );
#if VERBOSE_TREE_ITEMS
    yuiDebug() << "Creating tree item \"" << orig->label()
	       << "\" as child of \"" << parentItem->origItem()->label() << "\""
	       << endl;

#endif
}


void YQTreeItem::init( YQTree *		tree,
		       YTreeItem *	orig,
		       int		serial )
{
    YUI_CHECK_PTR( tree );
    YUI_CHECK_PTR( orig );

    _tree	= tree;
    _serialNo	= serial;
    _origItem	= orig;

    _origItem->setData( this );

    setText( 0, fromUTF8 ( _origItem->label() ) );
    setOpen( _origItem->isOpen() );

    if ( _origItem->hasIconName() )
    {
	string iconName = _tree->iconFullPath( _origItem );
	QPixmap icon( iconName.c_str() );

	if ( icon.isNull() )
	    yuiWarning() << "Can't load icon " << iconName << endl;
	else
	    setData( 0, Qt::DecorationRole, icon );
    }
}


void
YQTreeItem::setOpen( bool open )
{
    QTreeWidgetItem::setExpanded( open );
    _origItem->setOpen( open );
}



QString
YQTreeItem::key( int column, bool ascending ) const
{
    /*
     * Sorting key for QListView internal sorting:
     *
     * Always sort tree items by insertion order. The tree widget
     * cannot maintain a meaningful sorting order of its own: All it
     * could do is sort by names (ASCII sort). Better let the (YCP)
     * application handle this.
     */

    QString strKey;
    strKey.sprintf( "%08d", _serialNo );

    return strKey;
}


#include "YQTree.moc"