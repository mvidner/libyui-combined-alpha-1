/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						     (c) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:	      YQWizard.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include "YQWizard.h"
#define y2log_component "qt-wizard"
#include <ycp/y2log.h>

// For the command parser

#include <ycp/YCPBoolean.h>
#include <ycp/YCPCode.h>
#include <ycp/YCPInteger.h>
#include "ycp/YCPInteger.h"
#include <ycp/YCPList.h>
#include <ycp/YCPMap.h>
#include <ycp/YCPString.h>
#include <ycp/YCPSymbol.h>
#include <ycp/YCPTerm.h>
#include <ycp/YCPValue.h>
#include <ycp/YCPVoid.h>

#include <string>

#include <qhbox.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qobjectlist.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>
#include <qtoolbutton.h>
#include <qwidgetstack.h>

#include "utf8.h"
#include "YQi18n.h"
#include "YQUI.h"
#include "YQReplacePoint.h"
#include "YQEmpty.h"
#include "YQLabel.h"
#include "YQIconPool.h"
#include "QY2LayoutUtils.h"
#include "YEvent.h"

using std::string;


#define PIXMAP_DIR THEMEDIR "/wizard/"

#define USE_SEPARATOR			1

#define WORK_AREA_BOTTOM_MARGIN		10
#define WORK_AREA_RIGHT_MARGIN		10
#define SEPARATOR_MARGIN		6


YQWizard::YQWizard( QWidget *		parent,
		    YWidgetOpt &	opt,
		    const YCPValue &	backButtonId,	const YCPString & backButtonLabel,
		    const YCPValue &	abortButtonId,	const YCPString & abortButtonLabel,
		    const YCPValue &	nextButtonId,	const YCPString & nextButtonLabel  )
    : QVBox( parent )
    , YWizard( opt,
	       backButtonId,	backButtonLabel,
	       abortButtonId,	abortButtonLabel,
	       nextButtonId,	nextButtonLabel	 )
{
    setWidgetRep( this );
    _stepsEnabled = opt.stepsEnabled.value();

    _sideBar			= 0;
    _stepsPanel			= 0;
    _helpButton			= 0;
    _helpPanel			= 0;
    _helpBrowser		= 0;
    _stepsButton		= 0;
    _clientArea			= 0;
    _dialogIcon			= 0;
    _dialogHeading		= 0;
    _contentsReplacePoint	= 0;
    _buttonBox			= 0;
    _abortButton		= 0;
    _backButton			= 0;
    _nextButton			= 0;
    
    _verboseCommands		= false;


    //
    // Load graphics
    //

    loadGradientPixmaps();
    _gradientCenterColor = pixelColor( _bottomGradientPixmap, 0, 0 );


    //
    // Create widgets
    //

    layoutTitleBar( this );

    QHBox * hBox = new QHBox( this );
    CHECK_PTR( hBox );

    layoutSideBar( hBox );
    layoutWorkArea( hBox );

    y2debug( "Constructor finished." );
}



void YQWizard::layoutTitleBar( QWidget * parent )
{
    QHBox * titleBar = new QHBox( parent );
    CHECK_PTR( titleBar );
    setGradient( titleBar, _titleBarGradientPixmap );
    titleBar->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert

    //
    // Left logo
    //

    QLabel * left = new QLabel( titleBar );
    left->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    QPixmap leftLogo( PIXMAP_DIR "title-bar-left.png" );

    if ( ! leftLogo.isNull() )
    {
	left->setPixmap( leftLogo );
	left->setFixedSize( leftLogo.size() );
	left->setBackgroundOrigin( QWidget::ParentOrigin );
    }


    //
    // Center stretch space
    //

    addHStretch( titleBar );


    //
    // Right logo
    //

    QLabel * right = new QLabel( titleBar );
    CHECK_PTR( right );

    QPixmap rightLogo( PIXMAP_DIR "title-bar-right.png" );

    if ( ! rightLogo.isNull() )
    {
	right->setPixmap( rightLogo );
	right->setFixedSize( rightLogo.size() );
	right->setBackgroundOrigin( QWidget::ParentOrigin );
    }
}



void YQWizard::layoutSideBar( QWidget * parent )
{
    _sideBar = new QWidgetStack( parent );
    CHECK_PTR( _sideBar );
    _sideBar->setMinimumWidth( YQUI::ui()->defaultSize( YD_HORIZ ) / 5 );
    _sideBar->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) ); // hor/vert
    _sideBar->setMargin( 0 );


    layoutHelpPanel();
    
    if ( _stepsEnabled )
    {
	layoutStepsPanel();
	showSteps();
    }
}



void YQWizard::layoutStepsPanel()
{
    _stepsPanel = new QVBox( _sideBar );
    CHECK_PTR( _stepsPanel );


    // Top gradient

    QLabel * topGradient = new QLabel( _stepsPanel );
    CHECK_PTR( topGradient );
    setGradient( topGradient, _topGradientPixmap );


    // Steps

    _sideBar->addWidget( _stepsPanel );

    QLabel * steps = new QLabel(
				"<font size=3 color=#669900>"
				"<b>Base Installation</b>"
				"<ul>"
				"<li>Language<br>"
				"<li>Installation Settings<br>"
				"<li>Perform Installation<br>"
				"</ul>"
				"<b>Configuration</b>"
				"<ul>"
				"<li>Root Password"
				"<li>Network"
				"<li>Online Update"
				"<li>Users"
				"<li>Clean Up"
				"<li>Release Notes"
				"<li>Device Configuration"
				"</ul>"
				"</font>",
				_stepsPanel );
    CHECK_PTR( steps );

    steps->setPaletteBackgroundColor( _gradientCenterColor );
    steps->setFont( YQUI::ui()->currentFont() );

    steps->setMargin( 10 );

    steps->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    steps->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) ); // hor/vert

    QWidget * stretch = addVStretch( _stepsPanel );
    CHECK_PTR( stretch );
    stretch->setPaletteBackgroundColor( _gradientCenterColor );


    // Bottom gradient

    QLabel * bottomGradient= new QLabel( _stepsPanel );
    CHECK_PTR( bottomGradient );
    setGradient( bottomGradient, _bottomGradientPixmap );


    // Help button - intentionally without keyboard shortcut
    // (the text is only a fallback anyway if no icon can be found)
    _helpButton = new QPushButton( _( "Help" ), bottomGradient );
    CHECK_PTR( _helpButton );
    centerAtBottom( bottomGradient, _helpButton, WORK_AREA_BOTTOM_MARGIN );
    QPixmap pixmap = QPixmap( PIXMAP_DIR "help-button.png" );

    if ( ! pixmap.isNull() )
	_helpButton->setPixmap( pixmap );

    connect( _helpButton, SIGNAL( clicked()  ),
	     this,	  SLOT	( showHelp() ) );
}



void YQWizard::layoutHelpPanel()
{
    _helpPanel = new QHBox( _sideBar );
    CHECK_PTR( _helpPanel );
    _sideBar->addWidget( _helpPanel );

    addGradientColumn( _helpPanel );

    QVBox * vbox = new QVBox( _helpPanel );
    CHECK_PTR( vbox );


    // Help browser

    _helpBrowser = new QTextBrowser( vbox );
    CHECK_PTR( _helpBrowser );

    _helpBrowser->setMimeSourceFactory( 0 );
    _helpBrowser->setFont( YQUI::ui()->currentFont() );
    _helpBrowser->installEventFilter( this );
    _helpBrowser->setTextFormat( Qt::RichText );
    _helpBrowser->setMargin( 4 );

    QPixmap bgPixmap( PIXMAP_DIR "help-background.png" );

    if ( ! bgPixmap.isNull() )
	_helpBrowser->setPaletteBackgroundPixmap( bgPixmap );



    if ( _stepsEnabled )
    {
	// Bottom gradient

	QLabel * buttonParent = new QLabel( vbox );
	CHECK_PTR( buttonParent );
	// buttonParent->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum ) ); // hor/vert


	// "Steps" button - intentionally without keyboard shortcut
	// (the text is only a fallback anyway if no icon can be found)
	_stepsButton = new QPushButton( _( "Steps" ), buttonParent );
	CHECK_PTR( _stepsButton );

	QPixmap pixmap( PIXMAP_DIR "steps-button.png" );

	if ( ! pixmap.isNull() )
	    _stepsButton->setPixmap( pixmap );

	QGridLayout * grid = centerAtBottom( buttonParent, _stepsButton, WORK_AREA_BOTTOM_MARGIN );
	setBottomCroppedGradient( buttonParent, _bottomGradientPixmap, grid->sizeHint().height() );


	connect( _stepsButton, SIGNAL( clicked()   ),
		 this,	   SLOT	 ( showSteps() ) );
    }
    else
    {
	QWidget * bottomSpacer = addVSpacing( vbox, WORK_AREA_BOTTOM_MARGIN );
	CHECK_PTR( bottomSpacer );
	setBottomCroppedGradient( bottomSpacer, _bottomGradientPixmap, WORK_AREA_BOTTOM_MARGIN );
    }

    addGradientColumn( _helpPanel );
}



void YQWizard::layoutWorkArea( QHBox * parentHBox )
{
    QVBox * workAreaVBox = new QVBox( parentHBox );
    CHECK_PTR( workAreaVBox );

    QVBox * workArea = new QVBox( workAreaVBox );
    CHECK_PTR( workArea );

    // workArea->setFrameStyle( QFrame::TabWidgetPanel | QFrame::Sunken );
    // workArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
    workArea->setFrameStyle( QFrame::Box | QFrame::Plain );
    workArea->setMargin( 4 );

    layoutClientArea( workArea );

    //
    // Button box
    //

    _buttonBox = new QHBox( workArea );
    CHECK_PTR( _buttonBox );
    _buttonBox->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); // hor/vert
    _buttonBox->setMargin( 5 );
    layoutButtonBox();


    //
    // Spacers (purely decorative) at the bottom and right of the client area
    //

    QWidget * bottomSpacer = addVSpacing( workAreaVBox, WORK_AREA_BOTTOM_MARGIN );
    CHECK_PTR( bottomSpacer );
    setBottomCroppedGradient( bottomSpacer, _bottomGradientPixmap, WORK_AREA_BOTTOM_MARGIN );

    addGradientColumn( parentHBox, WORK_AREA_RIGHT_MARGIN );
}



void YQWizard::layoutClientArea( QWidget * parent )
{
    _clientArea = new QVBox( parent );
    CHECK_PTR( _clientArea );
    _clientArea->setMargin( 4 );

    //
    // Dialog icon and heading
    //

    QHBox * headingHBox = new QHBox( _clientArea );
    CHECK_PTR( headingHBox );
    headingHBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert

    _dialogIcon = new QLabel( headingHBox );
    CHECK_PTR( _dialogIcon );

    addHSpacing( headingHBox );

    _dialogHeading = new QLabel( headingHBox );
    CHECK_PTR( _dialogHeading );
    _dialogHeading->setFont( YQUI::ui()->headingFont() );

    addHStretch( headingHBox );
    addVSpacing( _clientArea );

#if USE_SEPARATOR

    QHBox * hbox = new QHBox( _clientArea );

    addHSpacing( hbox, SEPARATOR_MARGIN );

    QFrame * separator = new QFrame( hbox );
    CHECK_PTR( separator );
    separator->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    addHSpacing( hbox, SEPARATOR_MARGIN );
    addVSpacing( _clientArea );
#endif


    //
    // Replace point for wizard contents
    //

    YWidgetOpt widgetOpt;
    _contentsReplacePoint = new YQReplacePoint( _clientArea, widgetOpt );
    CHECK_PTR( _contentsReplacePoint );

    _contentsReplacePoint->setId( YCPSymbol( YWizardContentsReplacePointID ) ); // `id(`contents)
    addChild( _contentsReplacePoint );
    _contentsReplacePoint->installEventFilter( this );
    _contentsReplacePoint->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert


    //
    // Initial YEmpty widget contents of replace point
    //

    YQEmpty * empty = new YQEmpty( _contentsReplacePoint, widgetOpt );
    empty->setParent( _contentsReplacePoint );
    _contentsReplacePoint->addChild( empty );
}



void YQWizard::layoutButtonBox()
{
    destroyButtons();


    addHSpacing( _buttonBox, 4 );

    //
    // "Abort" button
    //

    _abortButton = new QPushButton( fromUTF8( _abortButtonLabel->value() ), _buttonBox );
    CHECK_PTR( _abortButton );
    connect( _abortButton,	SIGNAL( clicked()	),
	     this,		SLOT  ( abortClicked() ) );

    addHStretch( _buttonBox );


    //
    // "Back" button
    //

    _backButton	 = new QPushButton( fromUTF8( _backButtonLabel->value()	 ), _buttonBox );
    CHECK_PTR( _backButton );
    connect( _backButton,	SIGNAL( clicked()	),
	     this,		SLOT  ( backClicked()	) );

    addHSpacing( _buttonBox );


    //
    // "Next" button
    //

    _nextButton	 = new QPushButton( fromUTF8( _nextButtonLabel->value()	 ), _buttonBox );
    CHECK_PTR( _nextButton );
    connect( _nextButton,	SIGNAL( clicked()	),
	     this,		SLOT  ( nextClicked()	) );


    addHSpacing( _buttonBox, 4 );
}



void YQWizard::loadGradientPixmaps()
{
    _topGradientPixmap		= QPixmap( PIXMAP_DIR "top-gradient.png"	);
    _bottomGradientPixmap	= QPixmap( PIXMAP_DIR "bottom-gradient.png"	);
    _titleBarGradientPixmap	= QPixmap( PIXMAP_DIR "title-bar-gradient.png"	);
}



void YQWizard::setGradient( QWidget * widget, const QPixmap & pixmap )
{
    if ( widget && ! pixmap.isNull() )
    {
	widget->setFixedHeight( pixmap.height() );
	widget->setPaletteBackgroundPixmap( pixmap );
    }
}



void YQWizard::setBottomCroppedGradient( QWidget * widget, const QPixmap & pixmap, int croppedHeight )
{
    setGradient( widget, bottomCropPixmap( pixmap, croppedHeight ) );
}



QPixmap YQWizard::bottomCropPixmap( const QPixmap & full, int croppedHeight )
{
    QPixmap pixmap;

    if ( full.height() > croppedHeight )
    {
	pixmap = QPixmap( full.width(), croppedHeight );

	bitBlt( &pixmap, 0, 0,					// dest, dest_x, dest_y
		&full,	 0, full.height() - croppedHeight - 1,	// src, src_x, src_y
		full.width(), croppedHeight );			// src_width, src_height
    }
    else
    {
	pixmap = full;
    }

    return pixmap;
}



QColor YQWizard::pixelColor( const QPixmap & pixmap, int x, int y )
{
    // QPixmap doesn't allow direct access to pixel values (which makes some
    // sense since this requires a round-trip to the X server - pixmaps are X
    // server resources), so we need to convert the QPixmap to a QImage to get
    // that information. But since this conversion is expensive, we might save
    // some performance if we only convert the part we really need - so let's
    // cut out a tiny portion of the original pixmap and convert only that tiny
    // portion.

    QPixmap tiny( 1, 1 );

    bitBlt( &tiny, 0, 0,	// dest, dest_x, dest_y
	    &pixmap, x, y,	// src, src_x, src_y
	    1, 1 );		// src_width, src_height

    QImage image = tiny.convertToImage();


    return QColor( image.pixel( 0, 0 ) );
}



QWidget * YQWizard::addGradientColumn( QWidget * parent, int width )
{
    if ( ! parent )
	return 0;

    QVBox * vbox = new QVBox( parent );
    CHECK_PTR( vbox );

    QWidget * topGradient = addHSpacing( vbox, width );
    CHECK_PTR( topGradient );
    setGradient( topGradient, _topGradientPixmap );

    QWidget * centerStretch = new QWidget( vbox );
    CHECK_PTR( centerStretch );
    centerStretch->setPaletteBackgroundColor( _gradientCenterColor );


    QWidget * bottomGradient = new QWidget( vbox );
    CHECK_PTR( bottomGradient );
    setGradient( bottomGradient, _bottomGradientPixmap );

    return bottomGradient;
}



QGridLayout * YQWizard::centerAtBottom( QWidget * parent, QWidget * child, int margin )
{
    QGridLayout * grid = new QGridLayout( parent,
					  2,		// rows
					  3,		// columns
					  margin );
    CHECK_PTR( grid );

    // Give as much space as possible to the (empty) top row
    // -> Bottom-align the child widget
    grid->setRowStretch( 0, 99 );
    grid->setRowStretch( 1, 0  );

    // Evenly distribute excess space between left and right column,
    // don't resize the center column
    // -> center the widget in the center column
    grid->setColStretch( 0, 99 );
    grid->setColStretch( 1, 0  );
    grid->setColStretch( 2, 99 );

    grid->addWidget( child, 1, 1 );	// Bottom row, center column

    return grid;
}



void YQWizard::destroyButtons()
{
    if ( _backButton  )
    {
	delete _backButton;
	_backButton = 0;
    }

    if ( _abortButton )
    {
	delete _abortButton;
	_abortButton = 0;
    }

    if ( _nextButton  )
    {
	delete _nextButton;
	_nextButton = 0;
    }
}



void YQWizard::setDialogIcon( const char * iconName )
{
    if ( _dialogIcon )
    {
	if ( iconName )
	{
	    QPixmap icon( iconName );

	    if ( icon.isNull() )
		y2warning( "Couldn't load dialog icon \"%s\"", iconName );
	    else
		_dialogIcon->setPixmap( icon );
	}
	else
	{
	    _dialogIcon->clear();
	}
    }
}


void YQWizard::setDialogHeading( const QString & headingText )
{
    if ( _dialogHeading )
    {
	if ( headingText )
	    _dialogHeading->setText( headingText );
	else
	    _dialogHeading->clear();
    }
}


void YQWizard::setHelpText( const QString & helpText )
{
    if ( _helpBrowser )
    {
	if ( helpText )
	    _helpBrowser->setText( helpText );
	else
	    _helpBrowser->clear();
    }
}


void YQWizard::addChild( YWidget * ychild )
{
    if ( ! ychild )
    {
	y2error( "Cannot add NULL child" );
	return;
    }

    // FIXME delete any old child first

    QWidget * child = (QWidget *) ychild->widgetRep();
    CHECK_PTR( child );
    child->reparent( _clientArea, 0, QPoint( 0, 0 ) ); // parent, wflags, pos

    YContainerWidget::addChild( ychild );
}



void YQWizard::backClicked()
{
    sendEvent( _backButtonId );
}



void YQWizard::abortClicked()
{
    sendEvent( _abortButtonId );
}



void YQWizard::nextClicked()
{
    sendEvent( _nextButtonId );
}



void YQWizard::showHelp()
{
    if ( _sideBar && _helpPanel )
    {
	_sideBar->raiseWidget( _helpPanel );
    }
}



void YQWizard::showSteps()
{
    if ( _sideBar && _stepsPanel )
    {
	_sideBar->raiseWidget( _stepsPanel );
    }
}



void YQWizard::sendEvent( YCPValue id )
{
    // Wizard events are sent as menu events - the semantics are similar.
    //
    // Widget events wouldn't do since they use their widget's ID as the ID to
    // return (which would be inappropriate since that would be the ID of the
    // wizard widget). Another type of event (WizardEvent) could be introduced,
    // but it would add little more information (if any) than MenuEvent.
    //
    // YQPackageSelector uses the same approach. After all, one widget that can
    // return multiple IDs is roughly the semantics of MenuEvents.

    YQUI::ui()->sendEvent( new YMenuEvent( id ) );
}



long YQWizard::nicesize( YUIDimension dim )
{
    return dim == YD_HORIZ ? sizeHint().width() : sizeHint().height();
}



void YQWizard::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
    resizeClientArea();
}



void YQWizard::resizeClientArea()
{
    // y2debug( "resizing client area" );
    QRect contentsRect = _clientArea->contentsRect();
    _contentsReplacePoint->setSize( contentsRect.width(), contentsRect.height() );
}



bool YQWizard::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Resize && obj == _contentsReplacePoint )
    {
	resizeClientArea();
	return true;		// Event handled
    }

    return QWidget::eventFilter( obj, ev );
}


bool YQWizard::isCommand( QString declaration, const YCPTerm & term )
{
    declaration = declaration.simplifyWhiteSpace();

    // Check command name

    QString command = declaration;
    command.remove( QRegExp( "\\s*\\(.*$" ) );	// remove arguments

    if ( term->name().c_str() != command )
	return false;

    //
    // Check arguments
    //

    QString arg_decl = declaration;
    arg_decl.remove( QRegExp( "^.*\\(" ) );	// remove "command ("
    arg_decl.remove( QRegExp( "\\).*$" ) );	// remove ")"

    QStringList argDeclList = QStringList::split( ",", arg_decl );

    //
    // Check number of arguments
    //

    if ( argDeclList.size() != (unsigned) term->size() )
    {
	y2error( "Bad arguments for wizard command %s : %s",
		 (const char *) declaration, term->toString().c_str() );
	return false;
    }


    //
    // Check each individual argument
    //

    bool ok = true;

    for ( unsigned i=0; i < argDeclList.size() && ok; i++ )
    {
	QString wanted = argDeclList[ i ].stripWhiteSpace();
	YCPValue seen  = term->value( i );

	if 	( wanted == "string"	)	ok = seen->isString();
	else if ( wanted == "boolean" 	)	ok = seen->isBoolean();
	else if ( wanted == "bool" 	)	ok = seen->isBoolean();
	else if ( wanted == "list" 	)	ok = seen->isList();
	else if ( wanted == "map" 	)	ok = seen->isMap();
	else if ( wanted == "integer" 	)	ok = seen->isInteger();
	else if ( wanted == "int" 	)	ok = seen->isInteger();
	else
	{
	    y2error( "Bad declaration for wizard command %s : Unknown type \"%s\"",
		     (const char *) declaration, (const char *) wanted );
	}
    }

    if ( ! ok )
    {
	y2error( "Bad arguments for wizard command %s : %s",
		 (const char *) declaration, term->toString().c_str() );
    }

    if ( ok && _verboseCommands )
    {
	// Intentionally logging as milestone because a YCP app just explicitly
	// requested this log level 
	y2milestone( "Recognized wizard command %s : %s",
		     (const char *) declaration, term->toString().c_str() );
    }

    return ok;
}


QString YQWizard::qStringArg( const YCPTerm & term, int argNo )
{
    return fromUTF8( stringArg( term, argNo ).c_str() );
}


string YQWizard::stringArg( const YCPTerm & term, int argNo )
{
    if ( term->size() > argNo )
    {
	YCPValue arg( term->value( argNo ) );

	if ( arg->isString() )
	    return arg->asString()->value();
    }

    y2error( "Couldn't convert arg #%d of '%s' to string", argNo, term->toString().c_str() );
    return "";
}


bool YQWizard::boolArg( const YCPTerm & term, int argNo )
{
    if ( term->size() > argNo )
    {
	YCPValue arg( term->value( argNo ) );

	if ( arg->isBoolean() )
	    return arg->asBoolean()->value();
    }

    y2error( "Couldn't convert arg #%d of '%s' to bool", argNo, term->toString().c_str() );
    return false;
}


void YQWizard::setButtonLabel( QPushButton * button, const QString & newLabel )
{
    if ( button )
    {
	button->setText( newLabel );

	if ( newLabel.isEmpty() )
	    button->hide();
	else
	    button->show();
    }
}



YCPValue YQWizard::command( const YCPTerm & cmd )
{
#define OK YCPBoolean( true );


    if ( isCommand( "SetHelpText   	  ( string )", cmd ) )	{ setHelpText	( qStringArg( cmd, 0 ) );		return OK; }
    if ( isCommand( "SetDialogIcon 	  ( string )", cmd ) )	{ setDialogIcon	( qStringArg( cmd, 0 ) );		return OK; }
    if ( isCommand( "SetDialogHeading	  ( string )", cmd ) )	{ setDialogHeading( qStringArg( cmd, 0 ) );		return OK; }

    if ( isCommand( "SetAbortButtonLabel  ( string )", cmd ) )	{ setButtonLabel( _abortButton, qStringArg( cmd, 0 ) );	return OK; }
    if ( isCommand( "SetBackButtonLabel   ( string )", cmd ) )	{ setButtonLabel( _backButton,  qStringArg( cmd, 0 ) );	return OK; }
    if ( isCommand( "SetNextButtonLabel	  ( string )", cmd ) )	{ setButtonLabel( _nextButton,  qStringArg( cmd, 0 ) );	return OK; }
    if ( isCommand( "SetCancelButtonLabel ( string )", cmd ) )	{ setButtonLabel( _abortButton, qStringArg( cmd, 0 ) );	return OK; }
    if ( isCommand( "SetAcceptButtonLabel ( string )", cmd ) )	{ setButtonLabel( _nextButton,  qStringArg( cmd, 0 ) );	return OK; }
    if ( isCommand( "SetVerboseCommands	  ( bool   )", cmd ) )	{ setVerboseCommands( boolArg( cmd, 0 ) );		return OK; }

    y2error( "Undefined wizard command: %s", cmd->toString().c_str() );
    return YCPBoolean( false );

#undef OK
}



#include "YQWizard.moc"
