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
 *  $Id: flowcamera.h 1246 2012-11-30 06:23:09Z ueshiba $
 */
#include <sys/time.h>
#include <iostream>
#include "TU/v/TUv++.h"

namespace TU
{
/************************************************************************
*  global variables and definitions					*
************************************************************************/
extern bool	active;
    
namespace v
{
enum
{
  // Camera control.
    c_ContinuousShot,
};

/************************************************************************
*  global functions							*
************************************************************************/
template <class CAMERA> inline const CmdDef*
createMenuCmds(const CAMERA& camera)
{
    static MenuDef fileMenu[] =
    {
	{"Restore camera config. from memory",	M_Open,		false, noSub},
	{"Save camera config. to memory",	M_Save,		false, noSub},
	{"Save camera config. to file",		M_SaveAs,	false, noSub},
	{"-",					M_Line,		false, noSub},
	{"Quit",				M_Exit,		false, noSub},
	EndOfMenu
    };

    static CmdDef menuCmds[] =
    {
	{C_MenuButton,   M_File,	   0, "File",		 fileMenu,
	 CA_None, 0, 0, 1, 1, 0},
	{C_MenuButton,   M_Format,	   0, "Format",		 noProp,
	 CA_None, 1, 0, 1, 1, 0},
	{C_ToggleButton, c_ContinuousShot, 0, "Continuous shot", noProp,
	 CA_None, 2, 0, 1, 1, 0},
	EndOfCmds
    };

    menuCmds[1].prop = createFormatMenu(camera);
    
    return menuCmds;
}

}	// namespace v
 
/************************************************************************
*  global functions							*
************************************************************************/
template <class CAMERA> inline void
saveCameraConfig(const CAMERA& camera)
{
}
    
template <class CAMERA> inline void
restoreCameraConfig(CAMERA& camera)
{
}
    
#ifdef __TU_IIDCPP_H
inline void
saveCameraConfig(IIDCCamera& camera)
{
    camera.saveConfig(1);
}
	
inline void
restoreCameraConfig(IIDCCamera& camera)
{
    camera.restoreConfig(1);
}
#endif
    
inline void
countTime()
{
    static int		nframes = 0;
    static timeval	start;
    
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

template <class CAMERA> void
run(Array<CAMERA>& cameras)
{
    CaptureAndSave<CAMERA>	captureAndSave(cameras);
    captureAndSave.saveHeaders(std::cout);		// 画像数とヘッダを出力
    for (auto& camera : cameras)
	camera.continuousShot(true);			// カメラ出力開始

    while (active)
    {
	countTime();

	if (!captureAndSave(std::cout))
	    active = false;
    }

    for (auto& camera : cameras)
	camera.continuousShot(false);			// カメラ出力停止
}

}	// namespace TU
