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


/*-/

   File:       NCProgressBar.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCProgressBar_h
#define NCProgressBar_h

#include <iosfwd>

#include <yui/YProgressBar.h>
#include "NCWidget.h"

class NCProgressBar;


class NCProgressBar : public YProgressBar, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCProgressBar & OBJ );

    NCProgressBar & operator=( const NCProgressBar & );
    NCProgressBar( const NCProgressBar & );


    typedef long long Value_t;

    NClabel  label;
    Value_t  maxval;
    Value_t  cval;
    NCursesWindow * lwin;
    NCursesWindow * twin;

    void setDefsze();
    void tUpdate();

protected:

    virtual const char * location() const { return "NCProgressBar"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();

public:

    NCProgressBar( YWidget * parent,
		   const string & label,
		   int maxValue = 100 );
    virtual ~NCProgressBar();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setValue( int newValue );

    virtual void setEnabled( bool do_bv );
};


#endif // NCProgressBar_h
