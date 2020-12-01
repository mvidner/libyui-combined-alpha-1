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

  File:	      YQDumbTab.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQDumbTab_h
#define YQDumbTab_h

#include <qwidget.h>
#include "YDumbTab.h"

class QTabBar;
class QPaintEvent;


class YQDumbTab : public QWidget, public YDumbTab
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQDumbTab( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQDumbTab();

    /**
     * Add an item (a tab page).
     *
     * Reimplemented from YDumbTab.
     **/
    virtual void addItem( YItem * item );

    /**
     * Select or deselect an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem * item, bool selected = true );

    /**
     * Deselect all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deselectAllItems();

    /**
     * Delete all items (all tab pages).
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();
    
    /**
     * Notification that some shortcut was changed.
     *
     * Reimplemented from YDumbTab.
     **/
    virtual void shortcutChanged();
    
    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

public slots:

    /**
     * Send an event that the tab with the specified index is selected.
     **/
    void slotSelected( int index );

protected:

    QTabBar * _tabBar;
};

#endif // YQDumbTab_h
