/*
 *  $Id: MyCmdWindow.cc,v 1.7 2012-06-30 20:00:51 ueshiba Exp $
 */
#include <fstream>
#include "TU/V4L2CameraArray.h"
#include "TU/v/vV4L2++.h"
#include "MyCmdWindow.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCmdWindow<V4L2CameraArray>					*
************************************************************************/
template <> void
MyCmdWindow<V4L2CameraArray>::callback(CmdId id, CmdVal val)
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
	    if (_menuCmd.getValue(c_ContinuousShot))
		stopContinuousShot();

	    ofstream	out(_cameras.configFile().c_str());
	    if (out)
	    {
		out << _cameras.size() << endl;
		for (size_t i = 0; i < _cameras.size(); ++i)
		    out << _cameras[i]->dev() << ' ' << *_cameras[i];
	    }

	    if (_menuCmd.getValue(c_ContinuousShot))
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
