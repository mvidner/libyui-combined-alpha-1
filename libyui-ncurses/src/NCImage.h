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

   File:       NCImage.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCImage_h
#define NCImage_h

#include <iosfwd>

#include <yui/YImage.h>
#include "NCWidget.h"

class NCImage;


class NCImage : public YImage, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCImage & OBJ );

    NCImage & operator=( const NCImage & );
    NCImage( const NCImage & );


    NClabel label;

protected:

    virtual const char * location() const { return "NCImage"; }

public:

    NCImage( YWidget * parent, string defaulttext, bool animated = false );
    virtual ~NCImage();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setEnabled( bool do_bv );
};


#endif // NCImage_h
