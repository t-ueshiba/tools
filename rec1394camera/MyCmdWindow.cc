/*
 *  $Id: MyCmdWindow.cc,v 1.1 2010-12-21 00:13:25 ueshiba Exp $
 */
#include <cstdlib>
#include <cstdio>
#include <sys/time.h>
#include "rec1394camera.h"
#include "MyCmdWindow.h"
#include "MyModalDialog.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
countTime(int& nframes, struct timeval& start)
{
    if (nframes == 10)
    {
	struct timeval	end;
	gettimeofday(&end, NULL);
	double	interval = (end.tv_sec  - start.tv_sec) +
	    (end.tv_usec - start.tv_usec) / 1.0e6;
	std::cerr << nframes / interval << " frames/sec" << std::endl;
	nframes = 0;
    }
    if (nframes++ == 0)
	gettimeofday(&start, NULL);
}

namespace v
{
CmdDef*		createMenuCmds()				;
CmdDef*		createMenuCmds(Ieee1394Camera& camera)		;
CmdDef*		createCaptureCmds()				;
CmdDef*		createFeatureCmds()				;
CmdDef*		createFeatureCmds(const Ieee1394Camera& camera)	;
    
/************************************************************************
*  static data								*
************************************************************************/
static int	movieProp[3];

/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
MyCmdWindow::MyCmdWindow(App& parentApp, const std::string& cameraBase,
			 const Array<Ieee1394Camera*>& cameras,
			 u_int ncol, u_int mul, u_int div)
    :CmdWindow(parentApp,
	       "rec1394: image streams recorder for IEEE1394 cameras",
	       0, Colormap::RGBColor, 16, 0, 0),
     _cameraBase(cameraBase),
     _cameras(cameras),
     _ncol(ncol),
     _mul(mul),
     _div(div),
     _headIsActive(true),
     _movie(_cameras.dim()),
     _canvases(0),
     _menuCmd(*this, (_cameras.dim() > 0 ? createMenuCmds(*_cameras[0])
					 : createMenuCmds())),
     _captureCmd(*this, createCaptureCmds()),
     _featureCmd(*this, (_cameras.dim() > 0 ? createFeatureCmds(*_cameras[0])
					    : createFeatureCmds())),
     _fileSelection(*this),
     _timer(*this, 0)
{
    using namespace	std;
    
    _menuCmd.place(0, 0, _ncol, 1);
    _captureCmd.place(0, 1, _ncol-1, 1);
    _featureCmd.place(_ncol-1, 1, 1, 1);

    if (cameras.dim() > 0)
    {
      // ムービーの各ビューの画像ヘッダにキャリブレーション情報をセットする．
	ifstream	in((_cameraBase + ".calib").c_str());
	if (!in)
	{
	    for (u_int i = 0; i < _movie.nviews(); ++i)
	    {
		Image<PixelType>&	image = _movie.setView(i).image();
	
		in >> image.P >> image.d1 >> image.d2;
	    }
	}
    }
    
    show();

    initializeMovie();
    _movie.setCircularMode(_captureCmd.getValue(c_CircularMode));
}

MyCmdWindow::~MyCmdWindow()
{
    for (u_int i = 0; i < _canvases.dim(); ++i)
	delete _canvases[i];
}
    
void
MyCmdWindow::callback(CmdId id, CmdVal val)
{
    using namespace	std;

    try
    {
	switch (id)
	{
	  case M_Exit:
	    app().exit();
	    break;

	// I/O stuffs.
	  case M_Open:
	  {
	    stopContinuousShotIfRunning();

	    ifstream	in;
	    if (_fileSelection.open(in))
	    {
		_movie.restore(in);
		_headIsActive = true;
		setCanvases();
		repaintCanvases();
	    }
	  }
	    break;

	  case M_Save:
	  {
	    stopContinuousShotIfRunning();

	    ofstream	out;
	    if (_fileSelection.open(out))
		_movie.save(out);
	  }
	    break;

	  case c_SaveCurrentFrame:
	  {
	    stopContinuousShotIfRunning();

	    ofstream	out;
	    if (_fileSelection.open(out))
	    {
		for (u_int i = 0; i < _movie.nviews(); ++i)
		{
		    _movie.setView(i);
		    _movie.image().save(out);
		}
	    }
	  }
	    break;
	  
	  case c_SaveConfig:
	  {
	    stopContinuousShotIfRunning();

	    ofstream	out((_cameraBase + ".conf").c_str());
	    if (!out)
		throw runtime_error("Failed to open camera configuration file!!");
	    out << _cameras[0]->delay() << ' ' << _cameras.dim() << endl;
	    for (u_int i = 0; i < _cameras.dim(); ++i)
		out << "0x" << setw(16) << setfill('0')
		    << hex << _cameras[i]->globalUniqueId() << ' '
		    << dec << *_cameras[i];
	  }
	    break;

	// Camera controlling stuffs.
	  case c_YUV444_160x120:
	  case c_YUV422_320x240:
	  case c_YUV411_640x480:
	  case c_YUV422_640x480:
	  case c_RGB24_640x480:
	  case c_MONO8_640x480:
	  case c_MONO16_640x480:
	  case c_YUV422_800x600:
	  case c_RGB24_800x600:
	  case c_MONO8_800x600:
	  case c_YUV422_1024x768:
	  case c_RGB24_1024x768:
	  case c_MONO8_1024x768:
	  case c_MONO16_800x600:
	  case c_MONO16_1024x768:
	  case c_YUV422_1280x960:
	  case c_RGB24_1280x960:
	  case c_MONO8_1280x960:
	  case c_YUV422_1600x1200:
	  case c_RGB24_1600x1200:
	  case c_MONO8_1600x1200:
	  case c_MONO16_1280x960:
	  case c_MONO16_1600x1200:
	    for (u_int i = 0; i < _cameras.dim(); ++i)
		_cameras[i]->setFormatAndFrameRate(
				    Ieee1394Camera::uintToFormat(id),
				    Ieee1394Camera::uintToFrameRate(val));
	    initializeMovie();
	    break;

	  case c_Format_7_0:
	  case c_Format_7_1:
	  case c_Format_7_2:
	  case c_Format_7_3:
	  case c_Format_7_4:
	  case c_Format_7_5:
	  case c_Format_7_6:
	  case c_Format_7_7:
	  {
	    Ieee1394Camera::Format
		format7  = Ieee1394Camera::uintToFormat(id);
	    Ieee1394Camera::Format_7_Info
		fmt7info = _cameras[0]->getFormat_7_Info(format7);
	    MyModalDialog	modalDialog(*this, fmt7info);
	    u_int		u0, v0, width, height;
	    Ieee1394Camera::PixelFormat
		pixelFormat = modalDialog.getROI(u0, v0, width, height);
	    for (u_int i = 0; i < _cameras.dim(); ++i)
		_cameras[i]->setFormat_7_ROI(format7, u0, v0, width, height)
		    .setFormat_7_PixelFormat(format7, pixelFormat)
		    .setFormatAndFrameRate(
			format7, Ieee1394Camera::uintToFrameRate(val));
	    initializeMovie();
	  }
	    break;
	
	  case c_Brightness:
	  case c_AutoExposure:
	  case c_Sharpness:
	  case c_Hue:
	  case c_Saturation:
	  case c_Gamma:
	  case c_Shutter:
	  case c_Gain:
	  case c_Iris:
	  case c_Focus:
	  case c_Temperature:
	  case c_Zoom:
	    for (u_int i = 0; i < _cameras.dim(); ++i)
		_cameras[i]->setValue(id2feature(id), val);
	    break;

	  case c_WhiteBalance_UB:
	    for (u_int i = 0; i < _cameras.dim(); ++i)
		_cameras[i]
		    ->setWhiteBalance(val,
				      _featureCmd.getValue(c_WhiteBalance_VR));
	    break;
	  case c_WhiteBalance_VR:
	    for (u_int i = 0; i < _cameras.dim(); ++i)
		_cameras[i]
		    ->setWhiteBalance(_featureCmd.getValue(c_WhiteBalance_UB),
				      val);
	    break;
      
	  case c_Brightness	 + OFFSET_ONOFF:
	  case c_AutoExposure    + OFFSET_ONOFF:
	  case c_Sharpness	 + OFFSET_ONOFF:
	  case c_WhiteBalance_UB + OFFSET_ONOFF:
	  case c_WhiteBalance_VR + OFFSET_ONOFF:
	  case c_Hue		 + OFFSET_ONOFF:
	  case c_Saturation	 + OFFSET_ONOFF:
	  case c_Gamma		 + OFFSET_ONOFF:
	  case c_Shutter	 + OFFSET_ONOFF:
	  case c_Gain		 + OFFSET_ONOFF:
	  case c_Iris		 + OFFSET_ONOFF:
	  case c_Focus		 + OFFSET_ONOFF:
	  case c_Temperature     + OFFSET_ONOFF:
	  case c_Zoom		 + OFFSET_ONOFF:
	  {
	    Ieee1394Camera::Feature feature = id2feature(id - OFFSET_ONOFF);
	    if (val)
		for (u_int i = 0; i < _cameras.dim(); ++i)
		    _cameras[i]->turnOn(feature);
	    else
		for (u_int i = 0; i < _cameras.dim(); ++i)
		    _cameras[i]->turnOff(feature);
	  }
	    break;
      
	  case c_Brightness	 + OFFSET_AUTO:
	  case c_AutoExposure    + OFFSET_AUTO:
	  case c_Sharpness	 + OFFSET_AUTO:
	  case c_WhiteBalance_UB + OFFSET_AUTO:
	  case c_WhiteBalance_VR + OFFSET_AUTO:
	  case c_Hue		 + OFFSET_AUTO:
	  case c_Saturation	 + OFFSET_AUTO:
	  case c_Gamma		 + OFFSET_AUTO:
	  case c_Shutter	 + OFFSET_AUTO:
	  case c_Gain		 + OFFSET_AUTO:
	  case c_Iris		 + OFFSET_AUTO:
	  case c_Focus		 + OFFSET_AUTO:
	  case c_Temperature     + OFFSET_AUTO:
	  case c_Zoom		 + OFFSET_AUTO:
	  {
	    Ieee1394Camera::Feature feature = id2feature(id - OFFSET_AUTO);
	    if (val)
		for (u_int i = 0; i < _cameras.dim(); ++i)
		    _cameras[i]->setAutoMode(feature);
	    else
		for (u_int i = 0; i < _cameras.dim(); ++i)
		{
		    _cameras[i]->setManualMode(feature);
		    if (feature == Ieee1394Camera::WHITE_BALANCE)
			_cameras[i]->
			    setWhiteBalance(
				_featureCmd.getValue(c_WhiteBalance_UB),
				_featureCmd.getValue(c_WhiteBalance_VR));
		    else
			_cameras[i]->
			    setValue(feature,
				     _featureCmd.getValue(id - OFFSET_AUTO));
		}
	  }
	    break;
	    
	// Capturing stuffs.
	  case c_ContinuousShot:
	    if (val)
	    {
		_headIsActive = true;
		for (u_int i = 0; i < _cameras.dim(); ++i)
		    _cameras[i]->continuousShot();
		_timer.start(1);
	    }
	    else
	    {
		_timer.stop();
		for (u_int i = 0; i < _cameras.dim(); ++i)
		    _cameras[i]->stopContinuousShot();
	    }
	    break;

	  case c_OneShot:
	    stopContinuousShotIfRunning();
	    _headIsActive = true;
	    for (u_int i = 0; i < _cameras.dim(); ++i)
		_cameras[i]->oneShot();
	    tick();
	    break;

	// Movie stuffs.
	  case c_CircularMode:
	    stopContinuousShotIfRunning();
	    _movie.setCircularMode(val);
	    break;
	    
	  case c_NFrames:
	    if (_cameras.dim() > 0)
	    {
		stopContinuousShotIfRunning();
		initializeMovie();
	    }
	    else
		setNFrames();
	    break;
	    
	  case c_PlayMovie:
	    stopContinuousShotIfRunning();
	    if (val)
	    {
		_headIsActive = true;
		_timer.start(10);
	    }
	    else
		_timer.stop();
	    break;
	
	  case c_ForwardMovie:
	    stopContinuousShotIfRunning();
	    if (!++_movie)
		--_movie;
	    repaintCanvases();
	    break;
	
	  case c_BackwardMovie:
	    stopContinuousShotIfRunning();
	    if (!--_movie)
		_movie.rewind();
	    repaintCanvases();
	    break;
	
	  case c_HeadMovie:
	    stopContinuousShotIfRunning();
	    _movie.setFrame(val);
	    _headIsActive = true;
	    repaintCanvases();
	    break;

	  case c_TailMovie:
	    stopContinuousShotIfRunning();
	    _movie.setFrame(val);
	    _headIsActive = false;
	    repaintCanvases();
	    break;

	  // Editing stuffs.
	  case M_Cut:
	    stopContinuousShotIfRunning();
	    _movie.setFrame(_captureCmd.getValue(c_HeadMovie));
	    _movie.cut(int(_captureCmd.getValue(c_TailMovie)) -
		       int(_captureCmd.getValue(c_HeadMovie)));
	    setNFrames();	// フレーム数が変わっているのでGUIに反映
	    _captureCmd.setValue(c_TailMovie, int(_movie.currentFrame()));
	    _headIsActive = true;
	    repaintCanvases();
	    break;

	  case M_Copy:
	    stopContinuousShotIfRunning();
	    _movie.setFrame(_captureCmd.getValue(c_HeadMovie));
	    _movie.copy(int(_captureCmd.getValue(c_TailMovie)) -
			int(_captureCmd.getValue(c_HeadMovie)));
	    _headIsActive = true;
	    repaintCanvases();
	    break;

	  case M_Paste:
	  {
	      stopContinuousShotIfRunning();
	      _movie.setFrame(_captureCmd.getValue(c_HeadMovie));
	      u_int	n = _movie.paste();
	      setNFrames();	// フレーム数が変わっているのでGUIに反映
	      _captureCmd.setValue(c_TailMovie, int(_movie.currentFrame() + n));
	      _headIsActive = true;
	      repaintCanvases();
	  }
	  break;

	  case c_Rotate:
	    stopContinuousShotIfRunning();
	    _movie.setFrame(_captureCmd.getValue(c_HeadMovie));
	    _movie.rotate();
	    _captureCmd.setValue(c_TailMovie, int(_movie.currentFrame()));
	    _headIsActive = true;
	    repaintCanvases();
	    break;
	}
    }
    catch (exception& err)
    {
	cerr << err.what();
    }
}

  //! タイマの稼働中に定期的に呼ばれる処理
    void
    MyCmdWindow::tick()
    {
	static int			nframes = 0;
	static struct timeval	start;
	countTime(nframes, start);

	if (!_captureCmd.getValue(c_PlayMovie))
	{
	    syncronizedSnap();				// カメラから画像取り込み．

	    for (u_int i = 0; i < _cameras.dim(); ++i)
	    {
		_movie.setView(i);
		*_cameras[i] >> _movie.image();		// カメラから画像転送．
	    }
	}

	repaintCanvases();	// 画像の表示および現フレームのhead/tail sliderへの反映．

      // 現フレームを1つ進める．head/tail sliderに未反映なので，これらから値を取得
      // する場合は予め repaintCanvases() を呼ぶ必要がある．
	++_movie;
    }

  /*
   *  private member functions
   */
  //! カメラの台数と解像度および指定されたフレーム数に合わせてムービーを初期化する．
    void
    MyCmdWindow::initializeMovie()
    {
	using namespace	std;
    
      // カメラの台数分だけのビューを確保し，そのサイズを設定する．
	Array<Movie<PixelType>::Size >	sizes(_cameras.dim());
	for (u_int i = 0; i < sizes.dim(); ++i)
	    sizes[i] = make_pair(_cameras[i]->width(), _cameras[i]->height());
	_movie.setSizes(sizes);

      // 指定された枚数のフレームを確保する．
	u_int	nframes = atoi(_captureCmd.getString(c_NFrames));
	_movie.insert(nframes);
    
	setCanvases();		// キャンバスとGUI wigetを設定する．
	repaintCanvases();		// head/tail slidersに現フレームを反映する．
    }

  //! ムービーが生成されたとき，それに合わせてキャンバスとGUI widgetを設定する．
    void
    MyCmdWindow::setCanvases()
    {
	if (_canvases.dim() != _movie.nviews())
	{
	    for (u_int i = 0; i < _canvases.dim(); ++i)
		delete _canvases[i];		// 既存キャンバスを廃棄する．

	    _canvases.resize(_movie.nviews());	// 新たにビュー数だけ確保する．
	    for (u_int i = 0; i < _canvases.dim(); ++i)
	    {
		Image<PixelType>&	image = _movie.setView(i).image();
		_canvases[i] = new MyCanvasPane(*this, image,
						image.width(), image.height(),
						_mul, _div);
		_canvases[i]->place(i % _ncol, 2 + i / _ncol, 1, 1);
	    }
	}
	else
	{
	    for (u_int i = 0; i < _canvases.dim(); ++i)
		_canvases[i]->resize();
	}

	setNFrames();			// フレーム数をGUIに反映させる．
    }

  //! ムービーのフレーム数が変わったとき，それをGUI widgetに反映させる．
    void
    MyCmdWindow::setNFrames()
    {
	movieProp[0] = 0;					// 最初のフレーム番号
	movieProp[1] = _movie.nframes() - 1;		// 最後のフレーム番号
	movieProp[2] = 1;					// 刻み
	_captureCmd.setProp(c_HeadMovie, movieProp);	// head sliderに設定
	_captureCmd.setProp(c_TailMovie, movieProp);	// tail sliderに設定

    char	s[256];
    sprintf(s, "%6d", _movie.nframes());
    _captureCmd.setString(c_NFrames, s);		// widgetにフレーム数を設定
}

//! 現フレームの内容をキャンバスに表示し，それをhead/tail sliderに反映させる．
void
MyCmdWindow::repaintCanvases()
{
    for (u_int i = 0; i < _canvases.dim(); ++i)
	_canvases[i]->repaintUnderlay();

    const int	current = _movie.currentFrame();
    if (_headIsActive)				// head sliderを操作中なら...
    {
	_captureCmd.setValue(c_HeadMovie, current);	// それに現フレームを設定
	if (_captureCmd.getValue(c_TailMovie) < current)// tailがheadより後ろなら
	    _captureCmd.setValue(c_TailMovie, current);	// tail sliderを追従させる
    }
    else					// tail sliderを操作中なら...
    {
	_captureCmd.setValue(c_TailMovie, current);	// それに現フレームを設定
	if (_captureCmd.getValue(c_HeadMovie) > current)// headがtailより後ろなら
	    _captureCmd.setValue(c_HeadMovie, current);	// headを追従させる
    }
}
    
//! カメラから画像を出力中ならそれを停止する．また，現フレームをhead/tail sliderへ反映させる．
void
MyCmdWindow::stopContinuousShotIfRunning()
{
    if (_captureCmd.getValue(c_ContinuousShot))
    {
	_timer.stop();
	for (u_int i = 0; i < _cameras.dim(); ++i)
	    _cameras[i]->stopContinuousShot();
	_captureCmd.setValue(c_ContinuousShot, 0);
    }

    repaintCanvases();	// GUIに表示されるフレームと実際の現フレームを一致させる
}

//! 複数のカメラによって同期した画像を撮影する．
void
MyCmdWindow::syncronizedSnap()
{
#if defined(USE_VIDEO1394)
    const u_int64_t	margin = 2000;
    u_int64_t		last = 0;
    for (u_int i = 0; i < _cameras.dim(); ++i)
    {
	u_int64_t	filltime
			    = timeval2u_int64(_cameras[i]->snap().filltime());
	if (last + margin < filltime)
	{
	    last = filltime;
	    for (u_int j = 0; j < i; ++j)
		do
		{
		    filltime = timeval2u_int64(_cameras[j]->snap().filltime());
		} while (filltime + margin < last);
	}
	else if (filltime + margin < last)
	    do
	    {
		filltime = timeval2u_int64(_cameras[i]->snap().filltime());
	    } while (filltime + margin < last);
    }
#else
    for (int i = 0; i < _cameras.dim(); ++i)
	_cameras[i]->snap();
#endif
}
    
}
}
