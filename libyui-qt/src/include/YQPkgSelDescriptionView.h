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

  File:	      YQPkgSelDescriptionView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgSelDescriptionView_h
#define YQPkgSelDescriptionView_h

#include "YQPkgDescriptionView.h"


/**
 * @short Display the description of a zypp::ResObject derived object along with its
 * name and summary.
 **/
class YQPkgSelDescriptionView : public YQPkgDescriptionView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgSelDescriptionView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgSelDescriptionView();

    /**
     * Show details for the specified package:
     * In this case the package description.
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( zypp::ResObject::constPtr zyppObj );

protected:

    /**
     * Format the heading in HTML
     **/
    QString htmlHeading( zypp::ResObject::constPtr zyppObj );
};


#endif // ifndef YQPkgSelDescriptionView_h
