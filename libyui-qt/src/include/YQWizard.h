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

  File:	      YQWizard.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQWizard_h
#define YQWizard_h

#include "YWizard.h"
#include <qvbox.h>
#include <qpixmap.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qdict.h>

#include <string>

class QGridLayout;
class QHBox;
class QLabel;
class QPushButton;
class QTextBrowser;
class QToolButton;
class QWidgetStack;

class YQReplacePoint;


class YQWizard : public QVBox, public YWizard
{
    Q_OBJECT

    class Step;
    
public:
    /**
     * Constructor
     */
    YQWizard( QWidget * 	parent,
	      YWidgetOpt & 	opt,
	      const YCPValue & 	backButtonId,	const YCPString & backButtonLabel,
	      const YCPValue & 	abortButtonId,	const YCPString & abortButtonLabel,
	      const YCPValue & 	nextButtonId,	const YCPString & nextButtonLabel  );

    /**
     * Destructor
     **/
    virtual ~YQWizard();

    /**
     * Generic direct access to implementation-specific functions.
     * See YQWizard.cc for details.
     *
     * Returns 'true' on success, 'false' on failure.
     * Reimplemented from YWizard.
     **/
    virtual YCPValue command( const YCPTerm & command );


    /**
     * Notification that a child widget has been added.
     * Reimplemented from YContainerWidget.
     **/
    void addChild( YWidget * child );

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize( YUIDimension dim );

    /**
     * Sets the new size of the widget.
     **/
    void setSize( long newWidth, long newHeight );

    /**
     * Event filter - inherited from QWidget
     **/
    bool eventFilter( QObject * obj, QEvent * ev );

    /**
     * Set a dialog icon. 0 is a valid value - it clears the current icon.
     **/
    void setDialogIcon( const char * iconName );

    /**
     * Set a dialog heading. 0 is a valid value - it clears the old text.
     **/
    void setDialogHeading( const QString & headingText );

    /**
     * Returns 'true' if this wizard was created with steps enabled, i.e. the
     * side bar has a "steps" view.
     **/
    bool stepsEnabled() const { return _stepsEnabled; }

    /**
     * Add a step for the steps panel on the side bar.
     * This only adds the step to the internal list of steps.
     * The display is only updated upon calling updateSteps().
     **/
    void addStep( const QString & text, const QString & id );

    /**
     * Add a step heading for the steps panel on the side bar.
     * This only adds the heading to the internal list of steps.
     * The display is only updated upon calling updateSteps().
     **/
    void addStepHeading( const QString & text );

    /**
     * Delete all steps and step headings from the internal lists.
     * The display is only updated upon calling updateSteps().
     **/
    void deleteSteps();

    /**
     * Set the current step. This also triggers updateSteps() if necessary.
     **/
    void setCurrentStep( const QString & id );
    
    /**
     * Update the steps display: Reflect the internal steps and heading lists
     * in the layout.
     **/
    void updateSteps();


public slots:


    /**
     * Set the help text. 0 is a valid value - it clears the old text.
     **/
    void setHelpText( const QString & helpText );

    /**
     * Adapt the size of the client area (the ReplacePoint(`id(`contents)) to
     * fit in its current space.
     **/
    void resizeClientArea();

    /**
     * Show the current help text.
     *
     * This is useful only if it is obscured by any wizard steps, but it can
     * safely be called at any time.
     **/
    void showHelp();

    /**
     * Show the current wizard steps, if there are any. If there are none,
     * nothing happens.
     **/
    void showSteps();

    /**
     * Set a widget's background pixmap to a gradient pixmap and set the
     * widget's height (fixed) to that pixmap's height.
     **/
    static void setGradient( QWidget * widget, const QPixmap & pixmap );

    /**
     * Set a widget's background to the lower portion (the bottom
     * 'croppedHeight' pixels) of a pixmap and set the widget's height (fixed)
     * to that 'croppedHeight'.
     **/
    static void setBottomCroppedGradient( QWidget * widget,
					  const QPixmap & pixmap,
					  int croppedHeight );

    /**
     * Bottom-crop a pixmap: Return a pixmap with the bottom 'croppedHeight'
     * pixels.
     **/
    static QPixmap bottomCropPixmap( const QPixmap & pixmap, int croppedHeight );

    /**
     * Return the color of pixel( x, y ) of a pixmap.
     * This is a _very_ expensive operation!
     **/
    static QColor pixelColor( const QPixmap & pixmap, int x, int y );


protected slots:

    /**
     * Internal notification that the "Back" button has been clicked.
     **/
    void backClicked();

    /**
     * Internal notification that the "Abort" button has been clicked.
     **/
    void abortClicked();

    /**
     * Internal notification that the "Next" button has been clicked.
     **/
    void nextClicked();


protected:

    // Layout functions

    void layoutTitleBar	( QWidget * parent );
    void layoutSideBar	( QWidget * parent );
    void layoutStepsPanel();
    void layoutHelpPanel();
    void layoutWorkArea	( QHBox * parentHBox );
    void layoutClientArea( QWidget * parent );
    void layoutButtonBox();


    /**
     * Load gradient pixmaps
     **/
    void loadGradientPixmaps();

    /**
     * Load step status icons
     **/
    void loadStepsIcons();

    /**
     * Destroy the button box's buttons
     **/
    void destroyButtons();

    /**
     * Update all step - use appropriate icons and colors
     **/
    void updateStepStates();
    
    /**
     * Add a (left or right) margin of the specified width to a widget,
     * consisting of a fixed height top gradient , a flexible center part (in
     * the gradient center color) and a fixed height bottom gradient.
     *
     * The bottom gradient widget is returned as a reference for other
     * background pixmaps.
     **/
    QWidget * addGradientColumn( QWidget * parent, int width = 8 );

    /**
     * Add a grid layout to a parent widget that centers its (only) child at
     * the parent's bottom, maintaining the specified margin from all edges
     * (including the bottom).
     *
     * Returns the newly created grid layout.
     **/
    QGridLayout * centerAtBottom( QWidget * parent, QWidget * child, int margin );

    
    /**
     * Send a wizard event with the specified ID.
     **/
    void sendEvent( YCPValue id );


    //
    // Wizard command mini-parser
    //

    /**
     * Check if 'term' matches wizard command 'declaration'.
     * 'declaration' is a function prototype like this:
     *
     *		myFunction ( string, boolean, string )
     *
     * Void functions are declared without any parameters:
     *
     *		myFunction ()
     *
     * Function names must be unique. They cannot be overloaded.
     **/
    bool isCommand( QString declaration, const YCPTerm & term );

    /**
     * Return argument number 'argNo' from 'term' as QString.
     **/
    QString YQWizard::qStringArg( const YCPTerm & term, int argNo );

    /**
     * Return argument number 'argNo' from 'term' as std::string.
     **/
    std::string stringArg( const YCPTerm & term, int argNo );

    /**
     * Return argument number 'argNo' from 'term' as bool.
     **/
    bool boolArg( const YCPTerm & term, int argNo );

    /**
     * Return argument as type 'any' (plain YCPValue)
     **/
    YCPValue YQWizard::anyArg( const YCPTerm & term, int argNo );

    /**
     * Set a button's label.
     **/
    void setButtonLabel( QPushButton * button, const QString & newLabel );

    /**
     * Enable or disable a widget.
     **/
    void enableWidget( QWidget * w, bool enabled );

    /**
     * Set the keyboard focus to a widget.
     **/
    void setFocus( QWidget * w );
    
    /**
     * Set wizard command verbosity
     **/
    void setVerboseCommands( bool verbose ) { _verboseCommands = verbose; }


    /**
     * Set text color and status icon for one wizard step
     **/
    void setStepStatus( YQWizard::Step * step, const QPixmap & icon, const QColor & color );

    /**
     * Find a step with the specified ID. Returns 0 if there is no such step.
     **/
    YQWizard::Step * findStep( const QString & id );

    
    //
    // Data members
    //

    bool	_stepsEnabled;
    bool	_verboseCommands;
    bool	_protectNextButton;
    bool	_stepsDirty;

    QPixmap	_titleBarGradientPixmap;
    QPixmap	_topGradientPixmap;
    QColor	_gradientCenterColor;
    QPixmap	_bottomGradientPixmap;

    QPixmap	_stepCurrentIcon;
    QPixmap	_stepToDoIcon;
    QPixmap	_stepDoneIcon;

    QColor	_stepCurrentColor;
    QColor	_stepToDoColor;
    QColor	_stepDoneColor;

    QString	_currentStepID;


    QWidgetStack *	_sideBar;
    QVBox *		    _stepsPanel;
    QVBox *			_stepsBox;
    QGridLayout *		    _stepsGrid;
    QPushButton	*		_helpButton;
    QHBox *		    _helpPanel;
    QTextBrowser *		_helpBrowser;
    QPushButton *		_stepsButton;

    QVBox *		_clientArea;
    QLabel *		    _dialogIcon;
    QLabel *		    _dialogHeading;
    YQReplacePoint *	    _contentsReplacePoint;
    QHBox *		_buttonBox;
    QPushButton *	    _abortButton;
    QPushButton *	    _backButton;
    QPushButton *	    _nextButton;

    QPtrList<YQWizard::Step> 	_stepsList;
    QDict<YQWizard::Step>	_stepsIDs;


    
protected:

    /**
     * Helper class to represent a wizard step internally
     **/
    class Step
    {
    public:

	Step( const QString & name = "", const QString & id = "" )
	    : _name( name )
	    , _statusLabel( 0 )
	    , _nameLabel(0)
	    , _enabled( true )
	    , _idList( id )
	{}

	/**
	 * Destructor. Intentionally not deleting the widgets.
	 **/
	virtual ~Step() {}

	virtual bool isHeading() const { return false; }

	QString  name()		const { return _name;		}
	QLabel * statusLabel()	const { return _statusLabel;	}
	QLabel * nameLabel()	const { return _nameLabel;	}
	bool	 isEnabled() 	const { return _enabled;	}
	const QStringList & id() const { return _idList;	}
	void addID( const QString & id ) { _idList.append( id ); }
	virtual bool hasID( const QString & id ) { return _idList.find( id ) != _idList.end(); }

	void setStatusLabel( QLabel * label )	{ _statusLabel = label; }
	void setNameLabel  ( QLabel * label )	{ _nameLabel   = label; }
	void setEnabled( bool enabled )		{ _enabled = enabled; }

    protected:

	QString		_name;
	QLabel * 	_statusLabel;
	QLabel *	_nameLabel;
	bool		_enabled;
	QStringList	_idList;
    };


    /**
     * Helper class to represent a wizard step heading internally
     **/
    class StepHeading: public Step
    {
    public:

	StepHeading( const QString & name = "" )
	    : Step( name, "" )
	    {}

	virtual ~StepHeading() {}
	virtual bool isHeading() const { return true; }
	virtual bool hasID( const QString & id ) { return false; }
    };

}; // class YQWizard



#endif // YQWizard_h
