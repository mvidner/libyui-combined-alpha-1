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

  File:	      YQPatternSelector.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQPatternSelector_h
#define YQPatternSelector_h

#include <qvbox.h>
#include <qcolor.h>
#include <ycp/YCPString.h>

#include "YQPackageSelectorBase.h"


class QPushButton;

class YQPkgDescriptionView;
class YQPkgSelList;
class YQPkgSelectionsFilterView;


class YQPatternSelector : public YQPackageSelectorBase
{
    Q_OBJECT

public:

    YQPatternSelector( QWidget * parent, const YWidgetOpt & opt );


protected slots:

    /**
     * User clicked on "Details..." - start the detailed package selection.
     * This will return from UI::RunPkgSelection() with `details .
     **/
    void detailedPackageSelection();

    
protected:

    // Layout methods - create and layout widgets

    void	basicLayout();
    QWidget *	layoutLeftPane	( QWidget * parent );
    QWidget *	layoutRightPane	( QWidget * parent );
    void	layoutButtons	( QWidget * parent );

    /**
     * Establish Qt signal / slot connections.
     **/
    void makeConnections();


    // Data members


    QPushButton *			_detailsButton;

    YQPkgSelList *			_selList;
    YQPkgSelectionsFilterView *		_selectionsFilterView;
    YQPkgDescriptionView *		_descriptionView;
};



#endif // YQPatternSelector_h
