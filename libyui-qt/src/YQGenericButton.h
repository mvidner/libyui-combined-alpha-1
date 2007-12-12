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

  File:	      YQGenericButton.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQGenericButton_h
#define YQGenericButton_h

#include <qwidget.h>

#include "YPushButton.h"


class QPushButton;
class QObject;
class YQDialog;

using std::string;

/**
 * Abstract base class for push button and similar widgets -
 * all that can become a YQDialog's "default button".
 **/
class YQGenericButton : public QWidget, public YPushButton
{
    friend class YQDialog;

    Q_OBJECT
    
protected:
    
    /**
     * Constructor.
     **/
    YQGenericButton( YWidget *		parent,
		     const string &	label );

public:
    
    /**
     * Destructor.
     **/
    virtual ~YQGenericButton();

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Returns 'true' if this button is enabled, 'false' otherwise.
     **/
    bool isEnabled() const;

    /**
     * Changes the label (the text) of the button.
     **/
    void setLabel( const QString & label );

    /**
     * Changes the label (the text) of the button.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setLabel( const string & label );

    /**
     * Show this button as the dialog's default button. The button never calls
     * this by itself - the parent dialog is responsible for that.
     **/
    void showAsDefault( bool show = true );

    /**
     * Returns 'true' if this button is shown as a default button - which may
     * mean that this really is the dialogs's default button or it is the
     * dialog's focus button (a button that currently has the keyboard focus).
     *
     * Don't confuse this with YPushButton::isDefaultButton()!
     **/
    bool isShownAsDefault() const;

    /**
     * Set this button's default button state.
     *
     * Reimplemented from YPushButton.
     **/
    virtual void setDefaultButton( bool def = true );

    /**
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();

    /**
     * Set this button's icon.
     *
     * Reimplemented from YPushButton.
     **/
    virtual void setIcon( const string & iconName );

    /**
     * Returns the button's text (label) - useful for log messages etc.
     **/
    QString text() const;
    
    /**
     * Returns the internal Qt PushButton.
     **/
    QPushButton * qPushButton() const { return _qPushButton; }

    /**
     * Returns the internal parent dialog.
     **/
    YQDialog * yQDialog() const { return _dialog; }

    
public slots:

    /**
     * Activate (animated) this button.
     **/
    void activate();

    
protected:

    /**
     * Set the corresponding QPushButton.
     **/
    void setQPushButton( QPushButton * pb );
    
    /**
     * Redirect events from the _qPushButton member to this object.
     *
     * Overwritten from QObject.
     **/
    bool eventFilter( QObject * obj, QEvent * event );

    /**
     * Returns the corresponding YQDialog.
     * Throws an exception if there is none.
     **/
    YQDialog * dialog();

    void forgetDialog();
 
private:
    
    YQDialog *		_dialog;
    QPushButton *	_qPushButton;
    bool		_setDefaultButtonRecursive;
};

#endif // YQGenericButton_h