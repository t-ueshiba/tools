/*
 *  $Id: createFeatureCmds.cc,v 1.1.1.1 2012-09-15 08:03:09 ueshiba Exp $
 */
#include <vector>
#include <boost/foreach.hpp>
#include "flowV4L2camera.h"

namespace TU
{
namespace v
{
/************************************************************************
*  local data								*
************************************************************************/
static const u_int		NFEATURES = 30;
static CmdDef			featureCmds[NFEATURES + 1];
static int			props[NFEATURES][3];
static std::vector<MenuDef>	menus[NFEATURES];
	
/************************************************************************
*  global functions							*
************************************************************************/
CmdDef*
createFeatureCmds(const V4L2Camera& camera)
{
    u_int	n = 0;
    BOOST_FOREACH (V4L2Camera::Feature feature, camera.availableFeatures())
    {
	featureCmds[n].id	  = feature;
	featureCmds[n].val	  = camera.getValue(feature);
	featureCmds[n].title      = camera.getName(feature).c_str();
	featureCmds[n].attrs      = CA_None;
	featureCmds[n].gridx      = 0;
	featureCmds[n].gridy      = n;
	featureCmds[n].gridWidth  = 1;
	featureCmds[n].gridHeight = 1;
	featureCmds[n].size	  = 0;
	
	V4L2Camera::MenuItemRange
	    menuItems = camera.availableMenuItems(feature);

	if (menuItems.first == menuItems.second)
	{
	    int	min, max, step;
	    camera.getMinMaxStep(feature, min, max, step);

	    if (min == 0 && max == 1)
	    {
		featureCmds[n].type = C_ToggleButton;
		featureCmds[n].prop = 0;
	    }
	    else
	    {
		featureCmds[n].type = C_Slider;
		props[n][0] = min;
		props[n][1] = max - min;
		props[n][2] = step;
		featureCmds[n].prop = props[n];
	    }
	}
	else
	{
	    featureCmds[n].type = C_ChoiceMenuButton;

	    menus[n].resize(std::distance(menuItems.first,
					  menuItems.second) + 1);
	    u_int	i = 0;
	    BOOST_FOREACH (const V4L2Camera::MenuItem& menuItem, menuItems)
	    {
		menus[n][i].label   = menuItem.name.c_str();
		menus[n][i].id	    = menuItem.index;
		menus[n][i].checked = (i == featureCmds[n].val);
		menus[n][i].submenu = noSub;

		++i;
	    }
	    menus[n][i].label = 0;

	    featureCmds[n].prop = &menus[n][0];
	}

	++n;
    }
    featureCmds[n].type = C_EndOfList;

    return featureCmds;
}
}
    
}
