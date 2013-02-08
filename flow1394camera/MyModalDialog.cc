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
 *  $Id: MyModalDialog.cc 1246 2012-11-30 06:23:09Z ueshiba $
 */
#include "MyModalDialog.h"

namespace TU
{
namespace v
{
/************************************************************************
*  static data								*
************************************************************************/
enum	{c_U0, c_V0, c_Width, c_Height, c_PixelFormat, c_Set};

struct PixelFormat
{
    Ieee1394Camera::PixelFormat	pixelFormat;
    const char*			name;
};
    
static CmdDef*
createROICmds(const Ieee1394Camera::Format_7_Info& fmt7info)
{
    static int		prop[4][3];
    static PixelFormat	pixelFormat[] =
    {
	{Ieee1394Camera::MONO_8,	"Y(mono)"},
	{Ieee1394Camera::YUV_411,	"YUV(4:1:1)"},
	{Ieee1394Camera::YUV_422,	"YUV(4:2:2)"},
	{Ieee1394Camera::YUV_444,	"YUV(4:4:4)"},
	{Ieee1394Camera::RGB_24,	"RGB"},
	{Ieee1394Camera::MONO_16,	"Y(mono16)"},
	{Ieee1394Camera::RGB_48,	"RGB(color48)"},
	{Ieee1394Camera::SIGNED_MONO_16,"Y(signed mono16)"},
	{Ieee1394Camera::SIGNED_RGB_48,	"RGB(signed color48)"},
	{Ieee1394Camera::RAW_8,		"RAW(raw8)"},
	{Ieee1394Camera::RAW_16,	"RAW(raw16)"}
    };
    static const int	NPIXELFORMATS = sizeof(pixelFormat)/sizeof(pixelFormat[0]);
    static MenuDef	pixelFormatMenu[NPIXELFORMATS + 1];
    static CmdDef	cmds[] =
    {
	{C_Slider, c_U0,     fmt7info.u0,	"    u0", prop[0], CA_None,
	 0, 0, 1, 1, 0},
	{C_Slider, c_V0,     fmt7info.v0,	"    v0", prop[1], CA_None,
	 0, 1, 1, 1, 0},
	{C_Slider, c_Width,  fmt7info.width,	" width", prop[2], CA_None,
	 0, 2, 1, 1, 0},
	{C_Slider, c_Height, fmt7info.height,	"height", prop[3], CA_None,
	 0, 3, 1, 1, 0},
	{C_ChoiceMenuButton, c_PixelFormat, 0,	"pixel format", pixelFormatMenu, CA_None,
	 0, 4, 1, 1, 0},
	{C_Button, c_Set,    0,			"Set",	  noProp,  CA_None,
	 0, 5, 1, 1, 0},
	EndOfCmds
    };

  // Create commands for setting ROI.
    cmds[0].val = fmt7info.u0;
    prop[0][0]  = 0;
    prop[0][1]  = fmt7info.maxWidth - 1;
    prop[0][2]  = 1;
    cmds[1].val = fmt7info.v0;
    prop[1][0]  = 0;
    prop[1][1]  = fmt7info.maxHeight - 1;
    prop[1][2]  = 1;
    cmds[2].val = fmt7info.width;
    prop[2][0]  = 0;
    prop[2][1]  = fmt7info.maxWidth;
    prop[2][2]  = 1;
    cmds[3].val = fmt7info.height;
    prop[3][0]  = 0;
    prop[3][1]  = fmt7info.maxHeight;
    prop[3][2]  = 1;
    
  // Create a menu button for setting pixel format.
    u_int	npixelformats = 0;
    for (int i = 0; i < NPIXELFORMATS; ++i)
	if (fmt7info.availablePixelFormats & pixelFormat[i].pixelFormat)
	{
	    pixelFormatMenu[npixelformats].label = pixelFormat[i].name;
	    pixelFormatMenu[npixelformats].id	 = pixelFormat[i].pixelFormat;
	    pixelFormatMenu[npixelformats].checked
		= (fmt7info.pixelFormat == pixelFormat[i].pixelFormat);
	    pixelFormatMenu[npixelformats].submenu = noSub;
	    ++npixelformats;
	}
    pixelFormatMenu[npixelformats].label = 0;
    
    return cmds;
}

/************************************************************************
*  class MyModalDialog							*
************************************************************************/
MyModalDialog::MyModalDialog(Window& parentWindow,
			     const Ieee1394Camera::Format_7_Info& fmt7info)
    :ModalDialog(parentWindow, "ROI for Format_7_x", createROICmds(fmt7info)),
     _fmt7info(fmt7info)
{
}
    
Ieee1394Camera::PixelFormat
MyModalDialog::getROI(u_int& u0, u_int& v0, u_int& width, u_int& height)
{
    show();
    u0		= pane().getValue(c_U0);
    v0		= pane().getValue(c_V0);
    width	= pane().getValue(c_Width);
    height	= pane().getValue(c_Height);

    return Ieee1394Camera::uintToPixelFormat(pane().getValue(c_PixelFormat));
}

void
MyModalDialog::callback(CmdId id, CmdVal val)
{
    switch (id)
    {
      case c_U0:
      {
	int u0 = _fmt7info.unitU0
	       * ((int(val) + _fmt7info.unitU0/2) / _fmt7info.unitU0);
	pane().setValue(c_U0, u0);
      }
	break;
      case c_V0:
      {
        int v0 = _fmt7info.unitV0
	       * ((int(val) + _fmt7info.unitV0/2) / _fmt7info.unitV0);
	pane().setValue(c_V0, v0);
      }
	break;
      case c_Width:
      {
        int w = _fmt7info.unitWidth
	      * ((int(val) + _fmt7info.unitWidth/2) / _fmt7info.unitWidth);
	pane().setValue(c_Width, w);
      }
	break;
      case c_Height:
      {
        int h = _fmt7info.unitHeight
	      * ((int(val) + _fmt7info.unitHeight/2) / _fmt7info.unitHeight);
	pane().setValue(c_Height, h);
      }
	break;
	
      case c_Set:
	hide();
	break;
    }
}

}
}
