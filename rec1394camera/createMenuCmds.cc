/*
 *  $Id: createMenuCmds.cc,v 1.1 2010-12-21 00:13:26 ueshiba Exp $
 */
#include "rec1394camera.h"

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
    {Ieee1394Camera::YUV444_160x120,   "160x120-YUV(4:4:4)"},
    {Ieee1394Camera::YUV422_320x240,   "320x240-YUV(4:2:2)"},
    {Ieee1394Camera::YUV411_640x480,   "640x480-YUV(4:1:1)"},
    {Ieee1394Camera::YUV422_640x480,   "640x480-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_640x480,    "640x480-RGB"},
    {Ieee1394Camera::MONO8_640x480,    "640x480-Y(mono)"},
    {Ieee1394Camera::MONO16_640x480,   "640x480-Y(mono16)"},
    {Ieee1394Camera::YUV422_800x600,   "800x600-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_800x600,    "800x600-RGB"},
    {Ieee1394Camera::MONO8_800x600,    "800x600-Y(mono)"},
    {Ieee1394Camera::YUV422_1024x768,  "1024x768-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_1024x768,   "1024x768-RGB"},
    {Ieee1394Camera::MONO8_1024x768,   "1024x768-Y(mono)"},
    {Ieee1394Camera::MONO16_800x600,   "800x600-Y(mono16)"},
    {Ieee1394Camera::MONO16_1024x768,  "1024x768-Y(mono16)"},
    {Ieee1394Camera::YUV422_1280x960,  "1280x960-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_1280x960,   "1280x960-RGB"},
    {Ieee1394Camera::MONO8_1280x960,   "1280x960-Y(mono)"},
    {Ieee1394Camera::YUV422_1600x1200, "1600x1200-YUV(4:2:2)"},
    {Ieee1394Camera::RGB24_1600x1200,  "1600x1200-RGB"},
    {Ieee1394Camera::MONO8_1600x1200,  "1600x1200-Y(mono)"},
    {Ieee1394Camera::MONO16_1280x960,  "1280x960-Y(mono16)"},
    {Ieee1394Camera::MONO16_1600x1200, "1600x1200-Y(mono16)"},
    {Ieee1394Camera::Format_5_0,       "Format_5_0"},
    {Ieee1394Camera::MONO8_640x480x2,  "640x480x2-Y(mono)"},
    {Ieee1394Camera::Format_5_2,       "Format_5_2"},
    {Ieee1394Camera::Format_5_3,       "Format_5_3"},
    {Ieee1394Camera::Format_5_4,       "Format_5_4"},
    {Ieee1394Camera::Format_5_5,       "Format_5_5"},
    {Ieee1394Camera::Format_5_6,       "Format_5_6"},
    {Ieee1394Camera::Format_5_7,       "Format_5_7"},
    {Ieee1394Camera::Format_7_0,       "Format_7_0"},
    {Ieee1394Camera::Format_7_1,       "Format_7_1"},
    {Ieee1394Camera::Format_7_2,       "Format_7_2"},
    {Ieee1394Camera::Format_7_3,       "Format_7_3"},
    {Ieee1394Camera::Format_7_4,       "Format_7_4"},
    {Ieee1394Camera::Format_7_5,       "Format_7_5"},
    {Ieee1394Camera::Format_7_6,       "Format_7_6"},
    {Ieee1394Camera::Format_7_7,       "Format_7_7"},
    {Ieee1394Camera::Format_7_7,       "No cameras"}
};
static const int	NFORMATS = sizeof(format)/sizeof(format[0]) - 1;

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
    {Ieee1394Camera::FrameRate_120,	"120fps"},
    {Ieee1394Camera::FrameRate_240,	"240fps"},
    {Ieee1394Camera::FrameRate_x,	"custom frame rate"}
};
static const int	NRATES=sizeof(frameRate)/sizeof(frameRate[0]);

static MenuDef		formatMenu[NFORMATS + 1];
static MenuDef		rateMenu[NFORMATS][NRATES + 1];

static MenuDef nframesMenu[] =
{
    {"300",			 300,			false, noSub},
    {"600",			 600,			true,  noSub},
    {"900",			 900,			false, noSub},
    {"1800",			1800,			false, noSub},
    EndOfMenu
};

static MenuDef fileMenu[] =
{
    {"Open movie...",		M_Open,			false, noSub},
    {"Save movie...",		M_Save,			false, noSub},
    {"Save current frame...",	c_SaveCurrentFrame,	false, noSub},
    {"-",			M_Line,			false, noSub},
    {"Save camera config.",	c_SaveConfig,		false, noSub},
    {"-",			M_Line,			false, noSub},
    {"Quit",			M_Exit,			false, noSub},
    EndOfMenu
};

static MenuDef editMenu[] =
{
    {"Cut",			M_Cut,			false, noSub},
    {"Copy",			M_Copy,			false, noSub},
    {"Paste",			M_Paste,		false, noSub},
    {"Rotate",			c_Rotate,		false, noSub},
    EndOfMenu
};

static CmdDef MenuCmds[] =
{
    {C_MenuButton, M_File,   0, "File",   fileMenu,   CA_None, 0, 0, 1, 1, 0},
    {C_MenuButton, M_Edit,   0, "Edit",   editMenu,   CA_None, 1, 0, 1, 1, 0},
    {C_MenuButton, c_Format, 0, "Format", formatMenu, CA_None, 2, 0, 1, 1, 0},
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
 
CmdDef*
createMenuCmds()
{
    formatMenu[0].label	  = format[NFORMATS].name;
    formatMenu[0].id	  = format[NFORMATS].format;
    formatMenu[0].checked = false;
    formatMenu[0].submenu = 0;
    formatMenu[1].label	  = 0;

    return MenuCmds;
}

}
}
