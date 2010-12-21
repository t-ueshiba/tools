/*
 *  $Id: createCaptureCmds.cc,v 1.1 2010-12-21 00:13:26 ueshiba Exp $
 */
#include "rec1394camera.h"

namespace TU
{
namespace v
{
/************************************************************************
*  local data								*
************************************************************************/
static CmdDef CaptureCmds[] =
{
    {C_Button,	     c_OneShot,		0, "One shot",		noProp, CA_None,
     0, 0, 1, 1, 0},
    {C_ToggleButton, c_ContinuousShot,	0, "Continuous shot",	noProp, CA_None,
     1, 0, 1, 1, 0},
    {C_ToggleButton, c_PlayMovie,	0, "Play",		noProp, CA_None,
     2, 0, 1, 1, 0},
    {C_Button,	     c_BackwardMovie,	0, "<",			noProp, CA_None,
     3, 0, 1, 1, 0},
    {C_Button,	     c_ForwardMovie,	0, ">",			noProp, CA_None,
     4, 0, 1, 1, 0},
    {C_ToggleButton, c_CircularMode,	0, "Circular mode",	noProp, CA_None,
     5, 0, 1, 1, 0},
    {C_TextIn,	     c_NFrames,		0, "300",		noProp, CA_None,
     6, 0, 1, 1, 30},
    {C_Slider,	     c_HeadMovie,	0, "Head",		noProp, CA_None,
     0, 1, 7, 1, 500},
    {C_Slider,	     c_TailMovie,	0, "Tail",		noProp, CA_None,
     0, 2, 7, 1, 500},
    EndOfCmds
};

/************************************************************************
*  global functions							*
************************************************************************/
CmdDef*
createCaptureCmds()
{
    return CaptureCmds;
}
 
}
}
