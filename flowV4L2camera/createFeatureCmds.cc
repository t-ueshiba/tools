/*
 *  $Id: createFeatureCmds.cc,v 1.1.1.1 2012-09-15 08:03:09 ueshiba Exp $
 */
#include <vector>
#include <boost/foreach.hpp>
#include "TU/v/CmdPane.h"
#include "flowV4L2camera.h"

namespace TU
{
namespace v
{
/************************************************************************
*  local data								*
************************************************************************/
static const size_t		NFEATURES = 30;
static CmdDef			featureCmds[1 + NFEATURES + 1];
static int			props[NFEATURES][3];
static std::vector<MenuDef>	menus[NFEATURES];
static Array<CmdDef>		cameraChoiceCmds;
static Array<std::string>	cameraChoiceTitles;
    
/************************************************************************
*  global functions							*
************************************************************************/
CmdDef*
createFeatureCmds(const V4L2CameraArray& cameras)
{
    cameraChoiceCmds  .resize(cameras.size() + 2);
    cameraChoiceTitles.resize(cameras.size() + 1);
	
    size_t	n = 0, i = 0;
    for (; i < cameras.size(); ++i)
    {
	(cameraChoiceTitles[i] += "cam-") += ('0' + i);
	    
	cameraChoiceCmds[i].type       = C_RadioButton;
	cameraChoiceCmds[i].id	       = i;
	cameraChoiceCmds[i].val	       = 0;
	cameraChoiceCmds[i].title      = cameraChoiceTitles[i].c_str();
	cameraChoiceCmds[i].prop       = noProp;
	cameraChoiceCmds[i].attrs      = CA_None;
	cameraChoiceCmds[i].gridx      = i;
	cameraChoiceCmds[i].gridy      = 0;
	cameraChoiceCmds[i].gridWidth  = 1;
	cameraChoiceCmds[i].gridHeight = 1;
	cameraChoiceCmds[i].size       = 0;
    }
    cameraChoiceTitles[i] = "All";
    cameraChoiceCmds[i].type       = C_RadioButton;
    cameraChoiceCmds[i].id	   = i;
    cameraChoiceCmds[i].val	   = 0;
    cameraChoiceCmds[i].title      = cameraChoiceTitles[i].c_str();
    cameraChoiceCmds[i].prop       = noProp;
    cameraChoiceCmds[i].attrs      = CA_None;
    cameraChoiceCmds[i].gridx      = i;
    cameraChoiceCmds[i].gridy      = 0;
    cameraChoiceCmds[i].gridWidth  = 1;
    cameraChoiceCmds[i].gridHeight = 1;
    cameraChoiceCmds[i].size       = 0;

    cameraChoiceCmds[++i].type = C_EndOfList;
	
    featureCmds[n].type	      = C_ChoiceFrame;
    featureCmds[n].id	      = c_CameraChoice;
    featureCmds[n].val	      = cameras.size();
    featureCmds[n].title      = 0;
    featureCmds[n].prop       = cameraChoiceCmds.data();
    featureCmds[n].attrs      = CA_None;
    featureCmds[n].gridx      = 0;
    featureCmds[n].gridy      = n;
    featureCmds[n].gridWidth  = 1;
    featureCmds[n].gridHeight = 1;
    featureCmds[n].size	      = 0;

    ++n;
    
    BOOST_FOREACH (V4L2Camera::Feature feature, cameras[0]->availableFeatures())
    {
	featureCmds[n].id	  = feature;
	featureCmds[n].val	  = cameras[0]->getValue(feature);
	featureCmds[n].title      = cameras[0]->getName(feature).c_str();
	featureCmds[n].attrs      = CA_None;
	featureCmds[n].gridx      = 0;
	featureCmds[n].gridy      = n;
	featureCmds[n].gridWidth  = 1;
	featureCmds[n].gridHeight = 1;
	featureCmds[n].size	  = 0;
	
	V4L2Camera::MenuItemRange
	    menuItems = cameras[0]->availableMenuItems(feature);

	if (menuItems.first == menuItems.second)
	{
	    int	min, max, step;
	    cameras[0]->getMinMaxStep(feature, min, max, step);

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
	    size_t	i = 0;
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

void
refreshFeatureCmds(const V4L2Camera& camera, CmdPane& cmdPane)
{
    for (CmdDef* featureCmd = featureCmds + 1;
	 featureCmd->type != C_EndOfList; ++featureCmd)
	cmdPane.setValue(featureCmd->id,
			 camera.getValue(
			     V4L2Camera::uintToFeature(featureCmd->id)));
}
    
}
}
