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

  File:		YQPkgChangeLogView.cc

  Author:	Stefan Hundhammer <sh@suse.de>

  Textdomain	"packages-qt"

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qregexp.h>
#include "YQPkgChangeLogView.h"
#include "YQPkgDescriptionDialog.h"
#include "YQi18n.h"
#include "utf8.h"



YQPkgChangeLogView::YQPkgChangeLogView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
}


YQPkgChangeLogView::~YQPkgChangeLogView()
{
    // NOP
}


void
YQPkgChangeLogView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;

    if ( ! selectable )
    {
	clear();
	return;
    }

    QString html = htmlHeading( selectable,
				true );	// showVersion
    
    ZyppPkg installed = tryCastToZyppPkg( selectable->installedObj() );

    if ( installed )
    {
	html += changeLogTable( installed->changelog() );
    }
    else
    {
	html += "<p><i>" + _( "Information only available for installed packages" ) + "</i></p>";
    }

    setTextFormat( Qt::RichText );
    setText( html );
}



QString YQPkgChangeLogView::changeLogTable( const zypp::Changelog & changeLog ) const
{
    QString html;

    for ( zypp::Changelog::const_iterator it = changeLog.begin();
	  it != changeLog.end();
	  ++it )
    {
	QString changes = htmlEscape( fromUTF8( (*it).text() ) );
	changes.replace( "\n", "<br>" );
	changes.replace( " ", "&nbsp;" );

	html += row(
		    cell( (*it).date()   ) +
		    cell( (*it).author() ) +
		    "<td valign=top>" + changes + "</td>" // cell() calls htmlEscape() !
		    );
    }

    return html.isEmpty() ? "" : table( html );
}


#include "YQPkgChangeLogView.moc"
