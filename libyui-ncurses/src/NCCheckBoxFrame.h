/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCCheckBoxFrame.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCCheckBoxFrame_h
#define NCCheckBoxFrame_h

#include <iosfwd>

#include "YCheckBoxFrame.h"
#include "NCWidget.h"
#include "NCCheckBox.h"

class NCCheckBoxFrame;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCCheckBoxFrame
//
//	DESCRIPTION :
//
class NCCheckBoxFrame : public YCheckBoxFrame, public NCWidget {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCCheckBoxFrame & OBJ );

  NCCheckBoxFrame & operator=( const NCCheckBoxFrame & );
  NCCheckBoxFrame            ( const NCCheckBoxFrame & );

  private:

    NClabel label;
    bool isEnabled;
    
  protected:

    bool gotBuddy();

    virtual const char * location() const { return "NCCheckBoxFrame"; }

    virtual void wRedraw();

  public:

    NCCheckBoxFrame( NCWidget * parent, const YWidgetOpt & opt,
		     const YCPString & label, bool checked );
    virtual ~NCCheckBoxFrame();

    virtual long nicesize( YUIDimension dim );
    virtual void setSize( long newwidth, long newheight );

    virtual void setLabel( const YCPString & nlabel );

    virtual void setEnabling( bool do_bv );

    virtual bool getValue() { return isEnabled; }
    
    virtual void setValue( bool enable ) { isEnabled = enable; }

    virtual bool setKeyboardFocus();


    virtual NCursesEvent wHandleInput( wint_t key );
};

///////////////////////////////////////////////////////////////////

#endif // NCCheckBoxFrame_h
