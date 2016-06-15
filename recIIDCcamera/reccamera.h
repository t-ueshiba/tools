/*
 *  $Id: rec1394camera.h 1215 2012-11-09 01:36:56Z ueshiba $
 */
#include <sys/time.h>
#include "TU/v/TUv++.h"

/************************************************************************
*  global data								*
************************************************************************/
namespace TU
{
namespace v
{
enum
{
  // File menu
    c_SaveCurrentFrame,
    c_RestoreConfig,
    c_SaveConfig,

  // Camera control.
    c_ContinuousShot,
    c_OneShot,
    c_Trigger,

  // Movie control.
    c_HeadMovie,
    c_TailMovie,
    c_ForwardMovie,
    c_BackwardMovie,
    c_PlayMovie,
    c_CircularMode,
    c_NFramesLabel,
    c_NFrames,
    c_Swap
};

/************************************************************************
*  static data								*
************************************************************************/
static MenuDef nframesMenu[] =
{
    {"300",			300,			false, noSub},
    {"600",			600,			true,  noSub},
    {"900",			900,			false, noSub},
    {"1800",			1800,			false, noSub},
    EndOfMenu
};

static MenuDef fileMenu[] =
{
    {"Open movie...",		M_Open,			false, noSub},
    {"Save movie...",		M_Save,			false, noSub},
    {"Save current frame...",	c_SaveCurrentFrame,	false, noSub},
    {"-",			M_Line,			false, noSub},
    {"Restore camera config.",	c_RestoreConfig,	false, noSub},
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
    {"Swap",			c_Swap,			false, noSub},
    EndOfMenu
};

static CmdDef menuCmds[] =
{
    {C_MenuButton, M_File,   0, "File",   fileMenu, CA_None, 0, 0, 1, 1, 0},
    {C_MenuButton, M_Edit,   0, "Edit",   editMenu, CA_None, 1, 0, 1, 1, 0},
    {C_MenuButton, M_Format, 0, "Format", noProp,   CA_None, 2, 0, 1, 1, 0},
    EndOfCmds
};

/************************************************************************
*  global functions							*
************************************************************************/
template <class CAMERA> inline const CmdDef*
createMenuCmds(const CAMERA& camera)
{
    menuCmds[2].prop = createFormatMenu(camera);
    
    return menuCmds;
}

inline const CmdDef*
createMenuCmds()
{
    menuCmds[2].type = C_EndOfList;

    return menuCmds;
}

inline const CmdDef*
createCaptureCmds()
{
    static const CmdDef captureCmds[] =
    {
	{C_ToggleButton, c_ContinuousShot,	0, "Continuous shot",	noProp,
	 CA_None, 0, 0, 1, 1, 0},
	{C_ToggleButton, c_PlayMovie,		0, "Play",		noProp,
	 CA_None, 1, 0, 1, 1, 0},
	{C_Button,	 c_BackwardMovie,	0, "<",			noProp,
	 CA_None, 2, 0, 1, 1, 0},
	{C_Button,	 c_ForwardMovie,	0, ">",			noProp,
	 CA_None, 3, 0, 1, 1, 0},
	{C_ToggleButton, c_CircularMode,	0, "Circular mode",	noProp,
	 CA_None, 4, 0, 1, 1, 0},
	{C_TextIn,	 c_NFrames,		0, "300",		noProp,
	 CA_None, 5, 0, 1, 1, 30},
	{C_Slider,	 c_HeadMovie,		0, "Head",		noProp,
	 CA_None, 0, 1, 6, 1, 500},
	{C_Slider,	 c_TailMovie,		0, "Tail",		noProp,
	 CA_None, 0, 2, 6, 1, 500},
	EndOfCmds
    };
    
    return captureCmds;
}

inline const CmdDef*
createFeatureCmds()
{
    static const CmdDef	featureCmds[] = { EndOfCmds };

    return featureCmds;
}

}	// namespace v

/************************************************************************
*  global functions							*
************************************************************************/
inline void
countTime(int& nframes, timeval& start)
{
    if (nframes == 10)
    {
	timeval	end;
	gettimeofday(&end, NULL);
	double	interval = (end.tv_sec  - start.tv_sec) +
	    (end.tv_usec - start.tv_usec) / 1.0e6;
	std::cerr << nframes / interval << " frames/sec" << std::endl;
	nframes = 0;
    }
    if (nframes++ == 0)
	gettimeofday(&start, NULL);
}

}	// namespace TU
