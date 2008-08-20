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

  File:		YButtonBox.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#include <algorithm>	// max()
#include <vector>
#include <list>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YButtonBox.h"
#include "YPushButton.h"
#include "YUI.h"
#include "YApplication.h"

using std::max;
using std::vector;
using std::list;


YButtonBoxLayoutPolicy	YButtonBox::_layoutPolicy;
YButtonBoxMargins	YButtonBox::_defaultMargins;


struct YButtonBoxPrivate
{
    /**
     * Constructor
     **/
    YButtonBoxPrivate()
	: margins( _defaultMargins )
	{}

    //
    // Data members
    //

    YButtonBoxMargins margins;
};




YButtonBox::YButtonBox()
    : YWidget( parent )
    , priv( new YButtonBoxPrivate() )
{
    YUI_CHECK_NEW( priv );
    setChildrenManager( new YWidgetChildrenManager( this ) );
}


YButtonBox::~YButtonBox()
{
    // NOP
}


void
YButtonBox::setLayoutPolicy( YButtonBoxLayoutPolicy & layoutPolicy )
{
    _layoutPolicy = layoutPolicy;
}


YButtonBoxLayoutPolicy
YButtonBox::layoutPolicy()
{
    return _layoutPolicy;
}


YButtonBoxLayoutPolicy
YButtonBox::kdeLayoutPolicy()
{
    YButtonBoxLayoutPolicy policy;

    policy.buttonOrder           = YKDEButtonOrder;
    policy.equalSizeButtons      = false;
    policy.alignment[ YD_HORIZ ] = YAlignCenter;
    policy.alignment[ YD_VERT  ] = YAlignBegin;	// Align top

    return policy;
}


YButtonBoxLayoutPolicy
YButtonBox::gnomeLayoutPolicy()
{
    YButtonBoxLayoutPolicy policy;

    policy.buttonOrder           = YGnomeButtonOrder;
    policy.equalSizeButtons      = true;
    policy.alignment[ YD_HORIZ ] = YAlignEnd;	// Align right
    policy.alignment[ YD_VERT  ] = YAlignBegin;	// Align top

    return policy;
}


void
YButtonBox::setDefaultMargins( const YButtonBoxMargins & margins )
{
    _defaultMargins = margins;
}


YButtonBoxMargins
YButtonBox::defaultMargins()
{
    return _defaultMargins;
}


void
YButtonBox::setMargins( const YButtonBoxMargins & margins )
{
    priv->margins = margins;
}


YButtonBoxMargins
YButtonBox::margins() const
{
    return priv->margins;
}


void
YButtonBox::setSize( int newWidth, int newHeight )
{
    sanityCheck();
    doLayout( newWidth, newHeight );
}


void
YButtonBox::doLayout( int width, int height )
{
    vector<YPushButton *> buttons = buttonsByButtonOrder();

    if ( buttons.empty() )
	return;
    
    YPushButton * helpButton = findButton( YHelpButton );
    
    int prefWidth  = preferredWidth();
    int prefHeight = preferredHeight();
    YButtonBoxMargins margins = priv->margins;
    int widthLoss = 0;

    if ( width < prefWidth ) // Not enough horizontal space
    {
	// Try reducing margins

	int missing = prefWidth - width;

	if ( missing <= margins.left + margins.right )
	{
	    margins.left  -= missing / 2;
	    margins.right -= missing / 2;
	    missing = 0;
	}
	else
	{
	    missing -= margins.left;
	    missing -= margins.right;
	    margins.left  = 0;
	    margins.right = 0;
	}

	if ( missing > 0 && buttons.count() > 1 )
	{
	    // Try reducing spacing
	    
	    int totalSpacing = ( buttons.count() - 1 ) * margins.spacing;

	    if ( missing <= totalSpacing )
	    {
		totalSpacing -= missing;
		margins.spacing = totalSpacing / ( buttons.count() - 1 );
		missing = 0;
	    }
	    else
	    {
		missing -= totalSpacing;
		margins.spacing = 0;
	    }
	}

	if ( missing > 0 && helpButton )
	{
	    // Try reducing help button extra spacing
	    
	    if ( missing <= margins.helpButtonExtraSpacing )
	    {
		margins.helpButtonExtraSpacing -= missing;
		missing = 0;
	    }
	    else
	    {
		missing -= margins.helpButtonExtraSpacing;
		margins.helpButtonExtraSpacing = 0;
	    }
	}

	
	// Distribute missing width among all buttons
	
	if ( missing > 0 )
	    widthLoss = missing / buttons.count();
    }

    if ( width > prefWidth ) // Excess horizontal space
    {
	int excessWidth = width - prefWidth;
	
	switch ( _layoutPolicy.alignment[ YD_HORIZ ] )
	{
	    case YAlignCenter:
		margins.left  += excessWidth / 2;
		margins.right += excessWidth / 2;
		break;
		
	    case YAlignBegin:
		margins.right += excessWidth;
		break;
		
	    case YAlignEnd:
		margins.left  += excessWidth;
		break;
	}
    }


    if ( height < prefHeight ) // Not enough vertical space
    {
	// Reduce margins
	
	int missing = prefHeight - height;

	if ( missing < margins.top + margins.bottom )
	{
	    margins.top    -= missing / 2;
	    margins.bottom -= missing / 2;
	}
	else
	{
	    margins.top    = 0;
	    margins.bottom = 0;
	}
    }

    
    int x_pos = margins.left;
    int buttonWidth = 0;
    
    if ( _layoutPolicy.equalSizeButtons )
    {
	buttonWidth  = maxChildSize( YD_HORIZ );
	buttonWidth -= widthLoss;
    }

    for ( vector<YPushButton *>::iterator it = buttons.begin();
	  it != buttons.end();
	  ++it )
    {
	YPushButton * button = *it;

	if ( button == helpButton && button != buttons.front() )
	    x_pos += helpButtonExtraSpacing;
	
	if ( ! _layoutPolicy.equalSizeButtons )
	{
	    buttonWidth  = button->preferredWidth();
	    buttonWidth -= widthLoss;
	}

	int y_pos = margins.top;
	int buttonHeight = button->preferredHeight();

	if ( buttonHeight + margins.top + margins.bottom > height )
	{
	    // Not enough vertical space - cut off button

	    buttonHeight = height; // Top and bottom margins are already gone
	    y_pos = 0;
	}
	else // Excess vertical space
	{
	    // Distribute excess vertical space
	    
	    int excessHeight = height - buttonHeight;
	    excessHeight -= margins.top;
	    excessHeight -= margins.bottom;

	    switch ( _layoutPolicy.alignment[ YD_VERT ] )
	    {
		case YAlignBegin:	// Align top
		    break;
		    
		case YAlignCenter:
		    y_pos += excessHeight / 2;
		    break;

		case YAlignEnd:		// Align bottom
		    y_pos += excessHeight;
		    break;
	    }
	}

	button->setSize( buttonWidth, buttonHeight );
	moveChild( button, x_pos, y_pos );

	x_pos += buttonWidth;
	x_pos += margins.spacing;

	if ( button == helpButton )
	    x_pos += margins.helpButtonExtraSpacing;
    }
}


vector<YPushButton *>
YButtonBox::buttonsByButtonOrder() const
{
    vector<YPushButton *> specialButtons( YMaxButtonRole, (YPushButton *) 0 );
    vector<YPushButton *> customButtons;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	YPushButton * button = dynamic_cast<YPushButton *> (*it);

	if ( ! button )
	    YUI_THROW( YUIInvalidChildException( this, *it ) );
	    
	switch ( button->role() )
	{
	    case YOKButton:
	    case YCancelButton:
	    case YApplyButton:
	    case YHelpButton:

		if ( specialButtons[ button->role() ] ) // Only one of each of those is allowed
		{
		    string msg = "Multiple buttons with that role [";
		    msg += button->debugLabel();
		    msg += "]";
		    YUI_THROW( YUIButtonRoleMismatchException( msg ) );
		}
		else
		{
		    specialButtons[ button->role() ] = button;
		}
		break;

	    case YCustomButton:
		customButtons.push_back( button );
		break;

	    case YMaxButtonRole:
		YUI_THROW( YUIButtonRoleMismatchException( "Invalid button role" ) );
		break;
	}
    }

    vector<YPushButton *> buttons;

    if ( _layoutPolicy.buttonOrder == YKDEButtonOrder )
    {
	if ( specialButtons[ YOKButton     ] )	buttons.push_back( specialButtons[ YOKButton     ] );
	if ( specialButtons[ YApplyButton  ] )	buttons.push_back( specialButtons[ YApplyButton  ] );
	if ( specialButtons[ YCancelButton ] )	buttons.push_back( specialButtons[ YCancelButton ] );

	buttons.insert( buttons.end(), customButtons.begin(), customButtons.end() );

	if ( specialButtons[ YHelpButton   ] )	buttons.push_back( specialButtons[ YHelpButton   ] );
    }
    else // YGnomeButtonOrder
    {
	if ( specialButtons[ YHelpButton   ] )	buttons.push_back( specialButtons[ YHelpButton   ] );

	buttons.insert( buttons.end(), customButtons.begin(), customButtons.end() );
	
	if ( specialButtons[ YApplyButton  ] )	buttons.push_back( specialButtons[ YApplyButton  ] );
	if ( specialButtons[ YCancelButton ] )	buttons.push_back( specialButtons[ YCancelButton ] );
	if ( specialButtons[ YOKButton     ] )	buttons.push_back( specialButtons[ YOKButton     ] );
    }

    return buttons;
}


int
YButtonBox::preferredWidth()
{
    if ( childrenCount() < 1 )
	return 0;

    int width = ( childrenCount() - 1 ) * priv->spacing;

    if ( _layoutPolicy.equalSizeButtons )
	width += maxChildSize( YD_HORIZ ) * childrenCount();
    else
	width += totalChildrenWidth();

    width += priv->margins.left;
    width += priv->margins.right;

    if ( priv->margins.helpButtonExtraSpacing )
    {
	if ( findButton( YHelpButton ) )
	    width += priv->margins.helpButtonExtraSpacing;
    }

    return width;
}


int
YButtonBox::preferredHeight()
{
    int height = maxChildSize( YD_VERT );
    height += priv->margins.top;
    height += priv->margins.bottom;

    return height;
}


int
YButtonBox::maxChildSize( YUIDimension dim ) const
{
    int maxSize = 0;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	maxChildSize = max( maxChildSize, (*it)->preferredSize( dim ) );
    }

    return maxSize;
}


int
YButtonBox::totalChildrenWidth() const
{
    int totalWidth = 0;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	totalWidth += (*it)->preferredWidth();
    }

    return totalWidth;
}


bool
YButtonBox::stretchable( YUIDimension dimension ) const
{
    switch ( dim )
    {
	case YD_HORIZ:	return true;
	case YD_VERT :	return false;

	default:
	    YUI_THROW( YUIInvalidDimensionException() );
	    return 0;
    }
}


YPushButton *
YButtonBox::findButton( YButtonRole * role )
{
    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	YPushButton * button = dynamic_cast<YPushButton *> (*it);

	if ( button && button->role == role )
	    return button;
    }

    return 0;
}


void
YButtonBox::sanityCheck()
{
    YPushButton okButton     = 0;
    YPushButton cancelButton = 0;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	YPushButton * button = dynamic_cast<YPushButton *> (*it);

	if ( ! button )
	    YUI_THROW( YUIInvalidChildException( this, *it ) );

	switch ( button->role() )
	{
	    case YOKButton:

		if ( okButton )
		    YUI_THROW( YUIButtonRoleMismatchException( "Multiple buttons with role [OK]" ) );
		else
		    okButton = button;
		break;


	    case YCancelButton:

		if ( cancelButton )
		    YUI_THROW( YUIButtonRoleMismatchException( "Multiple buttons with role [Cancel]" ) );
		else
		    cancelButton = button;
		break;


	    default:
		break;
	}
    }

    if ( childrenCount() > 1 )
    {
	if ( ! okButton || ! cancelButton )
	    YUI_THROW( YUIButtonRoleMismatchException( "Button role mismatch: Must have both [OK] and [Cancel] roles" ) );
    }

    return 0;
}
