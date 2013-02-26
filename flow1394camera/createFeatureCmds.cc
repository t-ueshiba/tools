/*
 *  平成14-19年（独）産業技術総合研究所 著作権所有
 *  
 *  創作者：植芝俊夫
 *
 *  本プログラムは（独）産業技術総合研究所の職員である植芝俊夫が創作し，
 *  （独）産業技術総合研究所が著作権を所有する秘密情報です．創作者によ
 *  る許可なしに本プログラムを使用，複製，改変，第三者へ開示する等の著
 *  作権を侵害する行為を禁止します．
 *  
 *  このプログラムによって生じるいかなる損害に対しても，著作権所有者お
 *  よび創作者は責任を負いません。
 *
 *  Copyright 2002-2007.
 *  National Institute of Advanced Industrial Science and Technology (AIST)
 *
 *  Creator: Toshio UESHIBA
 *
 *  [AIST Confidential and all rights reserved.]
 *  This program is confidential. Any using, copying, changing or
 *  giving any information concerning with this program to others
 *  without permission by the creator are strictly prohibited.
 *
 *  [No Warranty.]
 *  The copyright holders or the creator are not responsible for any
 *  damages in the use of this program.
 *  
 *  $Id: createFeatureCmds.cc 1246 2012-11-30 06:23:09Z ueshiba $
 */
#include "flow1394camera.h"

namespace TU
{
namespace v
{
/************************************************************************
*  local data								*
************************************************************************/
struct Feature
{
    Ieee1394Camera::Feature	feature;
    const char*			name;
    int				prop[3];
};
static Feature		feature[] =
{
    {Ieee1394Camera::BRIGHTNESS,	"Brightness"	 },
    {Ieee1394Camera::AUTO_EXPOSURE,	"Auto exposure"	 },
    {Ieee1394Camera::SHARPNESS,		"Sharpness"	 },
    {Ieee1394Camera::WHITE_BALANCE,	"White bal.(U/B)"},
    {Ieee1394Camera::WHITE_BALANCE,	"White bal.(V/R)"},
    {Ieee1394Camera::HUE,		"Hue"		 },
    {Ieee1394Camera::SATURATION,	"Saturation"	 },
    {Ieee1394Camera::GAMMA,		"Gamma"		 },
    {Ieee1394Camera::SHUTTER,		"Shutter"	 },
    {Ieee1394Camera::GAIN,		"Gain"		 },
    {Ieee1394Camera::IRIS,		"Iris"		 },
    {Ieee1394Camera::FOCUS,		"Focus"		 },
    {Ieee1394Camera::TEMPERATURE,	"Temperature"	 },
    {Ieee1394Camera::ZOOM,		"Zoom"		 }
};
static const u_int	NFEATURES = sizeof(feature)/sizeof(feature[0]);
static CmdDef		featureCmds[3*NFEATURES + 1];

/************************************************************************
*  global functions							*
************************************************************************/
Ieee1394Camera::Feature
id2feature(CmdId id)
{
    return feature[id - c_Brightness].feature;
}

CmdDef*
createFeatureCmds(const Ieee1394Camera& camera)
{
    u_int		ncmds = 0, y = 0;
    for (u_int i = 0; i < NFEATURES; ++i)
    {
	u_int	inq = camera.inquireFeatureFunction(feature[i].feature);
	if (inq & Ieee1394Camera::Presence)
	{
	    u_int	x = 1;
	    
	    if (inq & Ieee1394Camera::OnOff)
	    {
	      // Create toggle button for turning on/off this feature.
		featureCmds[ncmds].type	      = C_ToggleButton;
		featureCmds[ncmds].id	      = c_Brightness+i+OFFSET_ONOFF;
		featureCmds[ncmds].val	      = camera.isTurnedOn(feature[i]
								  .feature);
		featureCmds[ncmds].title      = "On";
		featureCmds[ncmds].prop       = noProp;
		featureCmds[ncmds].attrs      = CA_None;
		featureCmds[ncmds].gridx      = x++;
		featureCmds[ncmds].gridy      = y;
		featureCmds[ncmds].gridWidth  = 1;
		featureCmds[ncmds].gridHeight = 1;
		featureCmds[ncmds].size	      = 0;
		++ncmds;
	    }

	    if (inq & Ieee1394Camera::Manual)
	    {
		if (inq & Ieee1394Camera::Auto)
		{
		  // Create toggle button for setting manual/auto mode.
		    featureCmds[ncmds].type	  = C_ToggleButton;
		    featureCmds[ncmds].id	  = c_Brightness+i+OFFSET_AUTO;
		    featureCmds[ncmds].val	  = camera.isAuto(feature[i]
								  .feature);
		    featureCmds[ncmds].title	  = "Auto";
		    featureCmds[ncmds].prop       = noProp;
		    featureCmds[ncmds].attrs      = CA_None;
		    featureCmds[ncmds].gridx      = x;
		    featureCmds[ncmds].gridy      = y;
		    featureCmds[ncmds].gridWidth  = 1;
		    featureCmds[ncmds].gridHeight = 1;
		    featureCmds[ncmds].size	  = 0;
		    ++ncmds;
		}
		
		featureCmds[ncmds].id	      = c_Brightness+i;
		featureCmds[ncmds].title      = feature[i].name;
		featureCmds[ncmds].gridx      = 0;
		featureCmds[ncmds].gridy      = y;
		featureCmds[ncmds].gridWidth  = 1;
		featureCmds[ncmds].gridHeight = 1;
		featureCmds[ncmds].size	      = 0;

		if (inq & Ieee1394Camera::ReadOut)
		{
		  // Create sliders for setting values.
		    featureCmds[ncmds].type  = C_Slider;
		    featureCmds[ncmds].prop  = feature[i].prop;
		    featureCmds[ncmds].attrs = CA_None;

		    u_int	min, max;
		    camera.getMinMax(feature[i].feature, min, max);
		    feature[i].prop[0] = min;
		    feature[i].prop[1] = max - min;
		    feature[i].prop[2] = 1;

		    if (feature[i].feature == Ieee1394Camera::WHITE_BALANCE)
		    {
			++ncmds;
			++i;
			++y;
			feature[i].prop[0]	      = min;
			feature[i].prop[1]	      = max - min;
			feature[i].prop[2]	      = 1;
			featureCmds[ncmds].type	      = C_Slider;
			featureCmds[ncmds].id	      = c_Brightness+i;
			featureCmds[ncmds].title      = feature[i].name;
			featureCmds[ncmds].prop	      = feature[i].prop;
			featureCmds[ncmds].attrs      = CA_None;
			featureCmds[ncmds].gridx      = 0;
			featureCmds[ncmds].gridy      = y;
			featureCmds[ncmds].gridWidth  = 1;
			featureCmds[ncmds].gridHeight = 1;
			featureCmds[ncmds].size	      = 0;
			u_int	ub, vr;
			camera.getWhiteBalance(ub, vr);
			featureCmds[ncmds-1].val      = ub;
			featureCmds[ncmds  ].val      = vr;
		    }
		    else
			featureCmds[ncmds].val
			    = camera.getValue(feature[i].feature);
		}
		else
		{
		  // Create a label for setting on/off and manual/auto modes.
		    featureCmds[ncmds].type  = C_Label;
		    featureCmds[ncmds].prop  = noProp;
		    featureCmds[ncmds].attrs = CA_NoBorder;
		}
		
		++ncmds;
	    }

	    ++y;
	}
    }
    featureCmds[ncmds].type = C_EndOfList;

    return featureCmds;
}
 
}
}
