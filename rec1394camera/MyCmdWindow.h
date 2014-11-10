/*
 *  $Id$
 */
#include <iterator>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/FileSelection.h"
#include "TU/v/Timer.h"
#include "TU/Movie.h"
#include "MyCanvasPane.h"
#include "reccamera.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCmdWindow<CAMERAS, PIXEL>					*
************************************************************************/
template <class CAMERAS, class PIXEL>
class MyCmdWindow : public CmdWindow
{
  public:
    typedef typename std::iterator_traits<
		typename CAMERAS::value_type>::value_type	camera_type;
    
  public:
    MyCmdWindow(App& parentApp, CAMERAS& cameras,
		u_int ncol, u_int mul, u_int div)		;
    ~MyCmdWindow()						;
    
    virtual void	callback(CmdId, CmdVal)			;
    virtual void	tick()					;
    
  private:
    void		initializeMovie()			;
    void		setCanvases()				;
    void		setNFrames()				;
    void		repaintCanvases()			;
    void		setFrame()				;
    void		stopContinuousShotIfRunning()		;

    CAMERAS&			_cameras;
    const u_int			_ncol;
    const u_int			_mul;
    const u_int			_div;
    bool			_headIsActive;
    Movie<PIXEL>		_movie;
    Array<MyCanvasPane<PIXEL>*>	_canvases;
    CmdPane			_menuCmd;
    CmdPane			_captureCmd;
    CmdPane			_featureCmd;
    Timer			_timer;
};
 
template <class CAMERAS, class PIXEL>
MyCmdWindow<CAMERAS, PIXEL>::MyCmdWindow(App& parentApp, CAMERAS& cameras,
					 u_int ncol, u_int mul, u_int div)
    :CmdWindow(parentApp,
	       "rec1394: image streams recorder for IEEE1394 cameras",
	       Colormap::RGBColor, 16, 0, 0),
     _cameras(cameras),
     _ncol(ncol),
     _mul(mul),
     _div(div),
     _headIsActive(true),
     _movie(_cameras.dim()),
     _canvases(0),
     _menuCmd(*this, (_cameras.size() > 0 ? createMenuCmds(*_cameras[0])
					  : createMenuCmds())),
     _captureCmd(*this, createCaptureCmds()),
     _featureCmd(*this, (_cameras.size() > 0 ? createFeatureCmds(_cameras)
					     : createFeatureCmds())),
     _timer(*this, 0)
{
    _menuCmd.place(0, 0, _ncol, 1);
    _captureCmd.place(0, 1, _ncol-1, 1);
    _featureCmd.place(_ncol-1, 1, 1, 1);

    show();

    initializeMovie();
    _movie.setCircularMode(_captureCmd.getValue(c_CircularMode));
}

template <class CAMERAS, class PIXEL>
MyCmdWindow<CAMERAS, PIXEL>::~MyCmdWindow()
{
    for (size_t i = 0; i < _canvases.size(); ++i)
	delete _canvases[i];
}
    
template <class CAMERAS, class PIXEL> void
MyCmdWindow<CAMERAS, PIXEL>::callback(CmdId id, CmdVal val)
{
    using namespace	std;

    try
    {
	if (setCameraFormat(_cameras, id, val) ||
	    setCameraSpecialFormat(_cameras, id, val, *this))
	{
	    initializeMovie();
	    return;
	}
	else if (setCameraFeatureValue(_cameras, id, val, _featureCmd))
	    return;
	
	switch (id)
	{
	  case M_Exit:
	    app().exit();
	    break;

	// I/O stuffs.
	  case M_Open:
	  {
	    stopContinuousShotIfRunning();

	    FileSelection	fileSelection(*this);
	    ifstream		in;
	    if (fileSelection.open(in))
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

	    FileSelection	fileSelection(*this);
	    ofstream		out;
	    if (fileSelection.open(out))
		_movie.save(out);
	  }
	    break;

	  case c_SaveCurrentFrame:
	  {
	    stopContinuousShotIfRunning();
	    setFrame();		// 現フレームを現在GUIに表示されているものに設定
	    
	    FileSelection	fileSelection(*this);
	    ofstream		out;
	    if (fileSelection.open(out))
	    {
		for (size_t i = 0; i < _movie.nviews(); ++i)
		    _movie.image(i).save(out);
	    }
	  }
	    break;
	  
	  case c_RestoreConfig:
	  {
	    stopContinuousShotIfRunning();

	    ifstream	in(_cameras.configFile().c_str());
	    if (in)
		in >> _cameras;

	    refreshFeatureCmds(_cameras, _featureCmd);
	  }
	    break;
      
	  case c_SaveConfig:
	  {
	    ofstream	out(_cameras.configFile().c_str());
	    if (!out)
		throw runtime_error("Failed to open camera configuration file!!");
	    out << _cameras;
	  }
	    break;

	// Capturing stuffs.
	  case c_ContinuousShot:
	    if (val)
	    {
		_headIsActive = true;
		exec(_cameras, &camera_type::continuousShot);
		_timer.start(1);
	    }
	    else
	    {
		_timer.stop();
		exec(_cameras, &camera_type::stopContinuousShot);
	    }
	    break;

	// Movie stuffs.
	  case c_CircularMode:
	    _movie.setCircularMode(val);
	    break;
	    
	  case c_NFrames:
	    if (_cameras.size() > 0)
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
	    _headIsActive = true;
	    setFrame();
	    break;

	  case c_TailMovie:
	    stopContinuousShotIfRunning();
	    _headIsActive = false;
	    setFrame();
	    break;

	  // Editing stuffs.
	  case M_Cut:
	    stopContinuousShotIfRunning();
	    repaintCanvases();	// head/tail sliderの表示を現フレームに一致させる
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
	    repaintCanvases();	// head/tail sliderの表示を現フレームに一致させる
	    _movie.setFrame(_captureCmd.getValue(c_HeadMovie));
	    _movie.copy(int(_captureCmd.getValue(c_TailMovie)) -
			int(_captureCmd.getValue(c_HeadMovie)));
	    _headIsActive = true;
	    repaintCanvases();
	    break;

	  case M_Paste:
	  {
	    stopContinuousShotIfRunning();
	    repaintCanvases();	// head/tail sliderの表示を現フレームに一致させる
	    _movie.setFrame(_captureCmd.getValue(c_HeadMovie));
	    u_int	n = _movie.paste();
	    setNFrames();	// フレーム数が変わっているのでGUIに反映
	    _captureCmd.setValue(c_TailMovie, int(_movie.currentFrame() + n));
	    _headIsActive = true;
	    repaintCanvases();
	  }
	  break;

	  case c_Swap:
	    stopContinuousShotIfRunning();
	    repaintCanvases();	// head/tail sliderの表示を現フレームに一致させる
	    _movie.setFrame(_captureCmd.getValue(c_HeadMovie));
	    _movie.swap();
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
template <class CAMERAS, class PIXEL> void
MyCmdWindow<CAMERAS, PIXEL>::tick()
{
    static int			nframes = 0;
    static struct timeval	start;
    countTime(nframes, start);

    if (!_captureCmd.getValue(c_PlayMovie))
    {
	exec(_cameras, &camera_type::snap);		// カメラから画像取り込み．
	for (size_t i = 0; i < _cameras.size(); ++i)
	    *_cameras[i] >> _movie.image(i);		// カメラから画像転送．
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
template <class CAMERAS, class PIXEL> void
MyCmdWindow<CAMERAS, PIXEL>::initializeMovie()
{
    using namespace	std;

  // カメラの台数分だけのビューを確保し，そのサイズを設定する．
    Array<typename Movie<PIXEL>::Size>	sizes(_cameras.size());
    for (size_t i = 0; i < sizes.size(); ++i)
	sizes[i] = make_pair(_cameras[i]->width(), _cameras[i]->height());
    _movie.setSizes(sizes);

    if (_movie.nviews() > 0)
    {
      // ムービーの各ビューの画像ヘッダにキャリブレーション情報をセットする．
	ifstream	in(_cameras.calibFile().c_str());
	if (in)
	{
	    for (size_t i = 0; i < _movie.nviews(); ++i)
	    {
		Image<PIXEL>&	image = _movie.image(i);
	
		in >> image.P >> image.d1 >> image.d2;
	    }
	}
    }
    
  // 指定された枚数のフレームを確保する．
    size_t	nframes = atoi(_captureCmd.getString(c_NFrames));
    _movie.insert(nframes);
    
    setCanvases();		// キャンバスとGUI wigetを設定する．
    repaintCanvases();		// head/tail slidersに現フレームを反映する．
}

//! ムービーが生成されたとき，それに合わせてキャンバスとGUI widgetを設定する．
template <class CAMERAS, class PIXEL> void
MyCmdWindow<CAMERAS, PIXEL>::setCanvases()
{
    if (_canvases.size() != _movie.nviews())
    {
	for (size_t i = 0; i < _canvases.size(); ++i)
	    delete _canvases[i];		// 既存キャンバスを廃棄する．

	_canvases.resize(_movie.nviews());	// 新たにビュー数だけ確保する．
	for (size_t i = 0; i < _canvases.size(); ++i)
	{
	    Image<PIXEL>&	image = _movie.image(i);
	    _canvases[i] = new MyCanvasPane<PIXEL>(*this, image, image.width(),
						   image.height(), _mul, _div);
	    _canvases[i]->place(i % _ncol, 2 + i / _ncol, 1, 1);
	}
    }
    else
    {
	for (size_t i = 0; i < _canvases.size(); ++i)
	    _canvases[i]->resize();
    }

    setNFrames();			// フレーム数をGUIに反映させる．
}

//! ムービーのフレーム数が変わったとき，それをGUI widgetに反映させる．
template <class CAMERAS, class PIXEL> void
MyCmdWindow<CAMERAS, PIXEL>::setNFrames()
{
    static int	headProp[3], tailProp[3];
    
    headProp[0] = tailProp[0] = 0;		// 最初のフレーム番号
    headProp[1] = _movie.nframes() - 1;		// 最後のフレーム番号
    tailProp[1] = _movie.nframes();		// 最後の次のフレーム番号
    headProp[2] = tailProp[2] = 1;		// 刻み
    _captureCmd.setProp(c_HeadMovie, headProp); // head sliderに設定
    _captureCmd.setProp(c_TailMovie, tailProp);	// tail sliderに設定

    char	s[256];
    sprintf(s, "%d", _movie.nframes());
    _captureCmd.setString(c_NFrames, s);	// widgetにフレーム数を設定
}

//! 現フレームの内容をキャンバスに表示し，それをhead/tail sliderに反映させる．
template <class CAMERAS, class PIXEL> void
MyCmdWindow<CAMERAS, PIXEL>::repaintCanvases()
{
    for (size_t i = 0; i < _canvases.size(); ++i)
	_canvases[i]->repaintUnderlay();

    const int	current = _movie.currentFrame();
    if (_headIsActive)				// head sliderを操作中なら...
    {
	_captureCmd.setValue(c_HeadMovie, current);	// それに現フレームを設定
	if (_captureCmd.getValue(c_TailMovie) < current)// tailがheadより後ろなら
	    _captureCmd.setValue(c_TailMovie, current);	// tailを追従させる
    }
    else					// tail sliderを操作中なら...
    {
	_captureCmd.setValue(c_TailMovie, current);	// それに現フレームを設定
	if (_captureCmd.getValue(c_HeadMovie) > current)// headがtailより後ろなら
	    _captureCmd.setValue(c_HeadMovie, current);	// headを追従させる
    }
}

//! ムービーの現フレームを現在head/tail sliderに表示されているフレーム番号に設定する．
template <class CAMERAS, class PIXEL> void
MyCmdWindow<CAMERAS, PIXEL>::setFrame()
{
    const int	current = _captureCmd.getValue((_headIsActive ? c_HeadMovie
							      : c_TailMovie));
    _movie.setFrame(current);

    for (size_t i = 0; i < _canvases.size(); ++i)
	_canvases[i]->repaintUnderlay();
}
    
//! カメラから画像を出力中ならそれを停止する．また，現フレームをhead/tail sliderへ反映させる．
template <class CAMERAS, class PIXEL> void
MyCmdWindow<CAMERAS, PIXEL>::stopContinuousShotIfRunning()
{
    if (_captureCmd.getValue(c_ContinuousShot))
    {
	_timer.stop();
	exec(_cameras, &camera_type::stopContinuousShot);
	_captureCmd.setValue(c_ContinuousShot, 0);
    }
}

}
}
