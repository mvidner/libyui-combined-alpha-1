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



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                        ***  Qt4 plugin  ***                                         ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-/

  File:	      QY2ComboTabWidget.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/


#include <QComboBox>
#include <QLabel>
#include <QStackedWidget>
#include <QHBoxLayout>

#include <QFrame>

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include "QY2ComboTabWidget.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget



QY2ComboTabWidget::QY2ComboTabWidget( const QString &	label,
				      QWidget *		parent,
				      const char *	name )
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin( 0 );

    QHBoxLayout *hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
//     hbox->setFrameStyle( QFrame::Panel | QFrame::Raised );
//     hbox->setLineWidth(2);
//     hbox->setMidLineWidth(2);
    hbox->setSpacing( 0 );
    hbox->setMargin ( 0  );

    vbox->addLayout(hbox);
    //this->setSpacing( SPACING );
    this->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred ) ); // hor/vert

    combo_label = new QLabel(label);
    hbox->addWidget(combo_label);
    Q_CHECK_PTR( combo_label );

    combo_box = new QComboBox( this );
    Q_CHECK_PTR( combo_box );
    hbox->addWidget(combo_box);
    combo_label->setBuddy( combo_box );
    combo_box->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert
    connect( combo_box, SIGNAL( activated( int ) ),
	     this,	SLOT  ( showPageIndex ( int ) ) );

    widget_stack = new QStackedWidget( this );
    Q_CHECK_PTR( widget_stack );
    vbox->addWidget(widget_stack);
}



QY2ComboTabWidget::~QY2ComboTabWidget()
{

}


void
QY2ComboTabWidget::addPage( const QString & page_label, QWidget * new_page )
{
    pages.insert( combo_box->count(), new_page );
    combo_box->addItem( page_label );
    widget_stack->addWidget( new_page );

    if ( ! widget_stack->currentWidget() )
	widget_stack->setCurrentWidget( new_page );
}


void
QY2ComboTabWidget::showPageIndex( int index )
{
    if ( pages.contains(index) )
    {
        QWidget * page = pages[ index ];
	widget_stack->setCurrentWidget( page );
	// yuiDebug() << "Changing current page" << std::endl;
	emit currentChanged( page );
    }
    else
    {
	qWarning( "QY2ComboTabWidget: Page #%d not found", index );
	return;
    }
}


void
QY2ComboTabWidget::showPage( QWidget * page )
{
    widget_stack->setCurrentWidget( page );

    if ( page == pages[ combo_box->currentIndex() ] )
    {
          // Shortcut: If the requested page is the one that belongs to the item
          // currently selected in the combo box, don't bother searching the
          // correct combo box item.
          return;
    }

    // Search the dict for this page

    QHashIterator<int, QWidget *> it( pages );

    while ( it.hasNext() )
    {
        it.next();
	if ( page == it.value() )
	{
	    combo_box->setCurrentIndex( it.key() );
	    return;
	}
    }

    // If we come this far, that page isn't present in the dict.

    qWarning( "QY2ComboTabWidget: Page not found" );
}



#include "QY2ComboTabWidget.moc"
