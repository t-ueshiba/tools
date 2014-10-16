/*
 *  $Id: MyCmdWindow.cc,v 1.7 2012-06-30 20:00:51 ueshiba Exp $
 */
#include <fstream>
#include <iomanip>
#include "TU/Ieee1394CameraArray.h"
#include "TU/v/vIeee1394++.h"
#include "MyCmdWindow.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCmdWindow<Ieee1394CameraArray>				*
************************************************************************/
template <> void
MyCmdWindow<Ieee1394CameraArray>::callback(CmdId id, CmdVal val)
{
    using namespace	std;

    try
    {
	if (handleCameraFormats(_cameras, id, val))
	{
	    _captureAndSave.setFormat(_cameras);
	    return;
	}
	else if (handleCameraSpecialFormats(_cameras, id, val, *this))
	{
	    _captureAndSave.setFormat(_cameras);
	    return;
	}
	else if (handleCameraFeatures(_cameras, id, val, _featureCmd))
	    return;
	
	switch (id)
	{
	  case M_Exit:
	    app().exit();
	    break;

	  case M_Save:
	  {
	    stopContinuousShot();

	    ofstream	out(_cameras.configFile().c_str());
	    if (out)
	    {
		out << _cameras[0]->delay() << ' ' << _cameras.size() << endl;
		for (size_t i = 0; i < _cameras.size(); ++i)
		    out << "0x" << setw(16) << setfill('0')
			<< hex << _cameras[i]->globalUniqueId() << ' '
			<< dec << *_cameras[i];
	    }

	    continuousShot();
	  }
	    break;
      
	  case c_ContinuousShot:
	    if (val)
		continuousShot();
	    else
		stopContinuousShot();
	    break;
	}
    }
    catch (exception& err)
    {
	cerr << err.what();
    }
}

}
}
