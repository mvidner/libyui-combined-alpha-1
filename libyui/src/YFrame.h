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

  File:		YFrame.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YFrame_h
#define YFrame_h

#include <string>
#include "YSingleChildContainerWidget.h"
#include "ImplPtr.h"

class YFramePrivate;
using std::string;


class YFrame : public YSingleChildContainerWidget
{
protected:
    /**
     * Constructor.
     **/
    YFrame( YWidget * parent, const string & label );

public:
    /**
     * Destructor.
     **/
    virtual ~YFrame();
    
    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YFrame"; }

    /**
     * Change the frame label.
     *
     * Derived classes should overwrite this, but call this base class function
     * in the overwritten function. 
     **/
    virtual void setLabel( const string & newLabel );

    /**
     * Get the current frame label.
     **/
    string label() const;

    /**
     * Set a property.
     * Reimplemented from YWidget.
     *
     * This method may throw exceptions, for example
     *   - if there is no property with that name
     *   - if the expected type and the type mismatch
     *   - if the value is out of range
     *
     * This function returns 'true' if the value was successfully set and
     * 'false' if that value requires special handling (not in error cases:
     * those are covered by exceptions).
     **/
    virtual bool setProperty( const string & propertyName,
			      const YPropertyValue & val );

    /**
     * Get a property.
     * Reimplemented from YWidget.
     *
     * This method may throw exceptions, for example
     *   - if there is no property with that name
     **/
    virtual YPropertyValue getProperty( const string & propertyName );

    /**
     * Return this class's property set.
     * This also initializes the property set upon the first call.
     *
     * Reimplemented from YWidget.
     **/
    virtual const YPropertySet & propertySet();

    
private:

    ImplPtr<YFramePrivate> priv;
};


#endif // YFrame_h

