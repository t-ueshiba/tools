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
 *  $Id: createMenuCmds.cc 1246 2012-11-30 06:23:09Z ueshiba $
 */
#include "flow1394camera.h"

namespace TU
{
namespace v
{
/************************************************************************
*  local data								*
************************************************************************/
struct Format
{
    Ieee1394Camera::Format	format;
    const char*			name;
    
};
static Format format[] =
{
    {Ieee1394Camera::YUV444_160x120,	"160x120-YUV(4:4:4)"},
    {Ieee1394Camera::YUV422_320x240,	"320x240-YUV(4:2:2)"},
    {Ieee1394Camera::YUV411_640x480,	"640x480-YUV(4:1:1)"},
    {Ieee1394Camera::YUV422_640x480,	"640x480-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_640x480,	"640x480-RGB"},
    {Ieee1394Camera::MONO8_640x480,	"640x480-Y(mono)"},
    {Ieee1394Camera::MONO16_640x480,	"640x480-Y(mono16)"},
    {Ieee1394Camera::YUV422_800x600,	"800x600-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_800x600,	"800x600-RGB"},
    {Ieee1394Camera::MONO8_800x600,	"800x600-Y(mono)"},
    {Ieee1394Camera::YUV422_1024x768,	"1024x768-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_1024x768,	"1024x768-RGB"},
    {Ieee1394Camera::MONO8_1024x768,	"1024x768-Y(mono)"},
    {Ieee1394Camera::MONO16_800x600,	"800x600-Y(mono16)"},
    {Ieee1394Camera::MONO16_1024x768,	"1024x768-Y(mono16)"},
    {Ieee1394Camera::YUV422_1280x960,	"1280x960-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_1280x960,	"1280x960-RGB"},
    {Ieee1394Camera::MONO8_1280x960,	"1280x960-Y(mono)"},
    {Ieee1394Camera::YUV422_1600x1200,	"1600x1200-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_1600x1200,	"1600x1200-RGB"},
    {Ieee1394Camera::MONO8_1600x1200,	"1600x1200-Y(mono)"},
    {Ieee1394Camera::MONO16_1280x960,	"1280x960-Y(mono16)"},
    {Ieee1394Camera::MONO16_1600x1200,	"1600x1200-Y(mono16)"},
    {Ieee1394Camera::Format_7_0,	"Format_7_0"},
    {Ieee1394Camera::Format_7_1,	"Format_7_1"},
    {Ieee1394Camera::Format_7_2,	"Format_7_2"},
    {Ieee1394Camera::Format_7_3,	"Format_7_3"},
    {Ieee1394Camera::Format_7_4,	"Format_7_4"},
    {Ieee1394Camera::Format_7_5,	"Format_7_5"},
    {Ieee1394Camera::Format_7_6,	"Format_7_6"},
    {Ieee1394Camera::Format_7_6,	"Format_7_7"}
};
static const int	NFORMATS = sizeof(format)/sizeof(format[0]);

struct FrameRate
{
    Ieee1394Camera::FrameRate	frameRate;
    const char*			name;
};
static FrameRate frameRate[] =
{
    {Ieee1394Camera::FrameRate_1_875,	"1.875fps"},
    {Ieee1394Camera::FrameRate_3_75,	"3.75fps"},
    {Ieee1394Camera::FrameRate_7_5,	"7.5fps"},
    {Ieee1394Camera::FrameRate_15,	"15fps"},
    {Ieee1394Camera::FrameRate_30,	"30fps"},
    {Ieee1394Camera::FrameRate_60,	"60fps"},
    {Ieee1394Camera::FrameRate_x,	"custom frame rate"}
};
static const int	NRATES=sizeof(frameRate)/sizeof(frameRate[0]);

static MenuDef		formatMenu[NFORMATS + 1];
static MenuDef		rateMenu[NFORMATS][NRATES + 1];

static MenuDef fileMenu[] =
{
    {"Save camera config.",	c_SaveConfig,		false, noSub},
    {"-",			M_Line,			false, noSub},
    {"Quit",			M_Exit,			false, noSub},
    EndOfMenu
};

static CmdDef MenuCmds[] =
{
    {C_MenuButton,   M_File,		0, "File",	    fileMenu, CA_None,
     0, 0, 1, 1, 0},
    {C_MenuButton,   c_Format,		0, "Format",	  formatMenu, CA_None,
     1, 0, 1, 1, 0},
    {C_ToggleButton, c_ContinuousShot,	1, "Continuous shot", noProp, CA_None,
     2, 0, 1, 1, 0},
    {C_Button,	     c_OneShot,		0, "One shot",	      noProp, CA_None,
     3, 0, 1, 1, 0},
    EndOfCmds
};

/************************************************************************
*  global functions							*
************************************************************************/
CmdDef*
createMenuCmds(Ieee1394Camera& camera)
{
    Ieee1394Camera::Format	current_format = camera.getFormat();
    Ieee1394Camera::FrameRate	current_rate   = camera.getFrameRate();
    u_int			nitems = 0;
    for (int i = 0; i < NFORMATS; ++i)
    {
	u_int	inq = camera.inquireFrameRate(format[i].format);
	u_int	nrates = 0;
	for (int j = 0; j < NRATES; ++j)
	{
	    if (inq & frameRate[j].frameRate)
	    {
	      // Create submenu items for setting frame rate.
		rateMenu[nitems][nrates].label	   = frameRate[j].name;
		rateMenu[nitems][nrates].id	   = frameRate[j].frameRate;
		rateMenu[nitems][nrates].checked
		    = ((current_format == format[i].format) &&
		       (current_rate == frameRate[j].frameRate));
		rateMenu[nitems][nrates].submenu   = noSub;
		++nrates;
	    }
	}
	rateMenu[nitems][nrates].label = 0;
	
	if (nrates != 0)
	{
	  // Create menu items for setting format.
	    formatMenu[nitems].label	 = format[i].name;
	    formatMenu[nitems].id	 = format[i].format;
	    formatMenu[nitems].checked	 = true;
	    formatMenu[nitems].submenu	 = rateMenu[nitems];
	    ++nitems;
	}
    }
    formatMenu[nitems].label = 0;

    return MenuCmds;
}
 
}
}
