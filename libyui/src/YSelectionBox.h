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
 ////                                       ***  core library  ***                                        ////
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

  File:		YSelectionBox.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YSelectionBox_h
#define YSelectionBox_h

#include "YSelectionWidget.h"

class YSelectionBoxPrivate;


/**
 * Selection box: List box that displays a (scrollable) list of items from
 * which the user can select exactly one. Each item has a label text and an
 * optional icon (*).
 *
 * This widget displays a number of items at once (as screen space permits). If
 * there is little screen space, you might consider using a ComboBox instead
 * which (in non-editable mode which is the default) displays just one item
 * (the selected item) right away and the others in a pop-up dialog upon mouse
 * click or keypress.
 *
 * The selection box also has a caption label that is
 * displayed above the list. The hotkey displayed in that caption label will
 * move the keyboard focus into the list.
 *
 * If multiple columns are needed, use the table widget instead.
 * For tree-like structures, use the tree widget.
 *
 *
 * (*) Not all UIs (in particular not text-based UIs) support displaying icons,
 * so an icon should never be an exclusive means to display any kind of
 * information.
 **/
class YSelectionBox : public YSelectionWidget
{
protected:

    /**
     * Constructor.
     **/
    YSelectionBox( YWidget * parent, const std::string & label );

public:
    /**
     * Destructor.
     **/
    virtual ~YSelectionBox();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YSelectionBox"; }

    /**
     * Return 'true' if this SelectionBox should be very small.
     **/
    bool shrinkable() const;

    /**
     * Make this SelectionBox very small. This will take effect only upon the
     * next geometry management run.
     *
     * Derived classes can overwrite this, but should call this base class
     * function in the new function.
     **/
    virtual void setShrinkable( bool shrinkable = true );

    /**
     * Deliver even more events than with notify() set.
     *
     * For YSelectionBox, this is relevant mostly for the NCurses UI:
     *
     * In graphical UIs like the Qt UI, the user can use the mouse to select an
     * item in a selection box. With notify() set, this will send an event
     * right away (i.e., it will make UserInput and related return, while
     * normally it would only return when the user clicks a PushButton).
     *
     * In the NCurses UI, there is no mouse, so the user has to use the cursor
     * keys to move to the item he wants to select. In immediateMode(), every
     * cursor key press will make the selection box send an event. Without
     * immediateMode(), the NCSelectionBox will wait until the user hits the
     * [Return] key until an event is sent. Depending on what the application
     * does upon each selection box event, immediateMode() might make the
     * application less responsive.
     **/
    bool immediateMode() const;

    /**
     * Set immediateMode() on or off.
     **/
    void setImmediateMode( bool on = true );

    /**
     * Set a property.
     * Reimplemented from YWidget.
     *
     * This function may throw YUIPropertyExceptions.
     *
     * This function returns 'true' if the value was successfully set and
     * 'false' if that value requires special handling (not in error cases:
     * those are covered by exceptions).
     **/
    virtual bool setProperty( const std::string & propertyName,
			      const YPropertyValue & val );

    /**
     * Get a property.
     * Reimplemented from YWidget.
     *
     * This method may throw YUIPropertyExceptions.
     **/
    virtual YPropertyValue getProperty( const std::string & propertyName );

    /**
     * Return this class's property set.
     * This also initializes the property upon the first call.
     *
     * Reimplemented from YWidget.
     **/
    virtual const YPropertySet & propertySet();

    /**
     * The name of the widget property that will return user input.
     * Inherited from YWidget.
     **/
    const char * userInputProperty() { return YUIProperty_CurrentItem; }


private:

    ImplPtr<YSelectionBoxPrivate> priv;
};


#endif // YSelectionBox_h
