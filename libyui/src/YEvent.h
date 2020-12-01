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

  File:		YEvent.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YEvent_h
#define YEvent_h


#include <string>
#include "YDialog.h"
#include "YSimpleEventHandler.h"

using std::string;
class YWidget;
class YItem;
class YDialog;


/**
 * Abstract base class for events to be returned upon UI::UserInput()
 * and related functions.
 **/
class YEvent
{
public:

    enum EventType
    {
	NoEvent = 0,
	UnknownEvent,
	WidgetEvent,
	MenuEvent,
	KeyEvent,
	CancelEvent,
	TimeoutEvent,
	DebugEvent
    };


    enum EventReason
    {
	UnknownReason = 0,
	Activated,
	SelectionChanged,
	ValueChanged
    };


    /**
     * Constructor.
     **/
    YEvent( EventType eventType = UnknownEvent );

    /**
     * Returns the event type.
     **/
    EventType eventType() const { return _eventType; }

    /**
     * Returns the unique serial no. of this event.
     * This is mainly useful for debugging.
     **/
    unsigned long serial() const { return _serial; }

    /**
     * Returns the widget that caused this event or 0 if there is none.
     *
     * This default implementation always returns 0.
     * Subclasses that actually return widgets should overwrite this method.
     **/
    virtual YWidget * widget() const { return 0; }

    /**
     * Return the YItem that corresponds to this event or 0 if there is none.
     *
     * This default implementation always returns 0.
     * Subclasses that actually return items should overwrite this method.
     **/
    virtual YItem * item() const { return 0; }

    /**
     * Return the dialog this event belongs to or 0 if no dialog was set yet.
     **/
    YDialog * dialog() const { return _dialog; }

    /**
     * Returns the character representation of an event type.
     **/
    static const char * toString( EventType eventType );

    /**
     * Returns the character representation of an event reason.
     **/
    static const char * toString( EventReason reason );


protected:
    
    /**
     * Set the dialog this event belongs to.
     **/
    void setDialog( YDialog * dia ) { _dialog = dia; }

    /**
     * Protected destructor - events can only be deleted via
     * YDialog::deleteEvent(). The associated dialog will take care of this
     * event and delete it when appropriate.
     *
     * This desctructor is virtual to force a polymorph object 
     * so dynamic_cast<> can be used.
     **/
    virtual ~YEvent();
    
private:
    
    friend void YDialog::deleteEvent( YEvent * event );
    friend void YSimpleEventHandler::deleteEvent( YEvent * event );


    //
    // Data members
    //

    EventType 			_eventType;
    unsigned long		_serial;
    YDialog *			_dialog;

    static unsigned long	_nextSerial;
    static int			_activeEvents;
};



class YWidgetEvent: public YEvent
{
public:

    /**
     * Constructor.
     **/
    YWidgetEvent( YWidget *	widget		= 0,
		  EventReason	reason		= Activated,
		  EventType 	eventType	= WidgetEvent );

    /**
     * Returns the widget that caused this event.
     * Reimplemented from YEvent.
     **/
    virtual YWidget * widget() const { return _widget; }

    /**
     * Returns the reason for this event. This very much like an event sub-type.
     **/
    EventReason reason() const { return _reason; }

protected:

    /**
     * Protected destructor - events can only be deleted via
     * YDialog::deleteEvent(). The associated dialog will take care of this
     * event and delete it when appropriate.
     **/
    virtual ~YWidgetEvent() {}

    
    //
    // Data members
    //
    
    YWidget * 	_widget;
    EventReason	_reason;
};


class YKeyEvent: public YEvent
{
public:

    /**
     * Constructor.
     *
     * Create a key event with a specified key symbol (a text describing the
     * key, such as "CursorLeft", "F1", etc.) and optionally the widget that
     * currently has the keyboard focus.
     **/
    YKeyEvent( const string &	keySymbol,
	       YWidget *	focusWidget = 0 );

    /**
     * Returns the key symbol - a text describing the
     * key, such as "CursorLeft", "F1", "a", "A", etc.
     **/
    string keySymbol() const { return _keySymbol; }

    /**
     * Returns the widget that currently has the keyboard focus.
     *
     * This might be 0 if no widget has the focus or if the creator of
     * this event could not obtain that information.
     **/
    YWidget * focusWidget() const { return _focusWidget; }

protected:
    
    /**
     * Protected destructor - events can only be deleted via
     * YDialog::deleteEvent(). The associated dialog will take care of this
     * event and delete it when appropriate.
     **/
    virtual ~YKeyEvent() {}

    
    //
    // Data members
    //

    string	_keySymbol;
    YWidget * 	_focusWidget;
};


/**
 * Event to be returned upon menu selection.
 **/
class YMenuEvent: public YEvent
{
public:

    YMenuEvent( YItem * item )
	: YEvent( MenuEvent )
	, _item( item )
	{}

    YMenuEvent( const char *     id )	: YEvent( MenuEvent ), _item(0), _id( id ) {}
    YMenuEvent( const string & 	 id )	: YEvent( MenuEvent ), _item(0), _id( id ) {}

    /**
     * Return the YItem that corresponds to this event or 0 if the event was
     * constructed with a string ID.
     *
     * Reimplemented from YEvent.
     **/
    virtual YItem * item() const { return _item; }

    /**
     * Return the string ID of this event. This will be an empty string if the
     * event was constructed with a YItem.
     **/
    string id() const { return _id; }

protected:
    
    /**
     * Protected destructor - events can only be deleted via
     * YDialog::deleteEvent(). The associated dialog will take care of this
     * event and delete it when appropriate.
     **/
    virtual ~YMenuEvent() {}

    
    //
    // Data members
    //

    YItem * _item;
    string  _id;
};


/**
 * Event to be returned upon closing a dialog with the window manager close
 * button (or Alt-F4)
 **/
class YCancelEvent: public YEvent
{
public:

    YCancelEvent() : YEvent( CancelEvent ) {}

    
protected:
    /**
     * Protected destructor - events can only be deleted via
     * YDialog::deleteEvent(). The associated dialog will take care of this
     * event and delete it when appropriate.
     **/
    virtual ~YCancelEvent() {}
};


/**
 * Event to be returned upon closing a dialog with the window manager close
 * button (or Alt-F4)
 **/
class YDebugEvent: public YEvent
{
public:

    YDebugEvent() : YEvent( DebugEvent ) {}
    
protected:
    /**
     * Protected destructor - events can only be deleted via
     * YDialog::deleteEvent(). The associated dialog will take care of this
     * event and delete it when appropriate.
     **/
    virtual ~YDebugEvent() {}
};


/**
 * Event to be returned upon timeout
 * (i.e. no event available in the specified timeout)
 **/
class YTimeoutEvent: public YEvent
{
public:

    YTimeoutEvent() : YEvent( TimeoutEvent ) {}
    
protected:
    /**
     * Protected destructor - events can only be deleted via
     * YDialog::deleteEvent(). The associated dialog will take care of this
     * event and delete it when appropriate.
     **/
    virtual ~YTimeoutEvent() {}
};


#endif // YEvent_h
