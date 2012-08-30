/*
 *  $Id: main.cc,v 1.2 2012-08-30 01:14:13 ueshiba Exp $
 */
#include <unistd.h>
#ifdef WIN32
#  include <io.h>
#  include <fcntl.h>
#endif
#include "TU/Image++.h"

#ifndef EOF
#  define EOF	(-1)
#endif

#define XY_YZ(X, Y, Z)						\
{								\
    rgb->X = *buf;						\
    rgb->Y = (u_int(*(buf+1)) + u_int(*nxt)) >> 1;		\
    rgb->Z = *(nxt+1);						\
    ++rgb;							\
    ++buf;							\
    ++nxt;							\
}

#define YX_ZY(X, Y, Z)						\
{								\
    rgb->X = *(buf+1);						\
    rgb->Y = (u_int(*buf) + u_int(*(nxt+1))) >> 1;		\
    rgb->Z = *nxt;						\
    ++rgb;							\
    ++buf;							\
    ++nxt;							\
}

#define XYX_YZY_XYX(X, Y, Z)					\
{								\
    rgb->X = (u_int(*(prv-1)) + u_int(*(prv+1)) +		\
	       u_int(*(nxt-1)) + u_int(*(nxt+1))) >> 2;		\
    rgb->Y = (u_int(*(buf-1)) +u_int(*(buf+1)) +		\
	       u_int(*prv) + u_int(*nxt)) >> 2;			\
    rgb->Z = *buf;						\
    ++rgb;							\
    ++prv;							\
    ++buf;							\
    ++nxt;							\
}

#define yXy_ZYZ_yXy(X, Y, Z)					\
{								\
    rgb->X = (u_int(*prv) + u_int(*nxt)) >> 1;			\
    rgb->Y = *buf;						\
    rgb->Z = (u_int(*(buf-1)) + u_int(*(buf+1))) >> 1;		\
    ++rgb;							\
    ++prv;							\
    ++buf;							\
    ++nxt;							\
}

namespace TU
{
enum Bayer	{RGGB, BGGR, GRBG, GBRG};

/************************************************************************
*  static functions							*
************************************************************************/
template <class S, class T> static const S*
bayerRGGB2x2(const S* buf, T* rgb, int w)
{
    const S*	nxt = buf + w;			// next line
    while ((w -= 2) > 0)
    {
	XY_YZ(r, g, b)
	YX_ZY(r, g, b)
    }
    XY_YZ(r, g, b)
    --buf;
    --nxt;
    XY_YZ(r, g, b)
    
    return buf + 1;
}

template <class S, class T> static const S*
bayerRGGBOdd3x3(const S* buf, T* rgb, int w)
{
    const S	*nxt = buf + w;			// next line
    YX_ZY(b, g, r)				// 左端の画素は2x2で処理
    const S	*prv = buf - w;			// previous line
    while ((w -= 2) > 0)			// 奇数行中間の列を処理
    {
	XYX_YZY_XYX(r, g, b)
	yXy_ZYZ_yXy(r, g, b)
    }
    --buf;
    --nxt;
    YX_ZY(b, g, r)				// 右端の画素は2x2で処理
    
    return buf + 1;
}

template <class S, class T> static const S*
bayerRGGBEven3x3(const S* buf, T* rgb, int w)
{
    const S	*nxt = buf + w;			// next line
    XY_YZ(r, g, b)				// 左端の画素は2x2で処理
    const S	*prv = buf - w;			// previous line
    while ((w -= 2) > 0)			// 偶数行中間の列を処理
    {
	yXy_ZYZ_yXy(b, g, r)
	XYX_YZY_XYX(b, g, r)
    }
    --buf;
    --nxt;
    XY_YZ(r, g, b)				// 右端の画素は2x2で処理

    return buf + 1;
}

template <class S, class T> static const S*
bayerBGGR2x2(const S* buf, T* rgb, int w)
{
    const S*	nxt = buf + w;			// next line
    while ((w -= 2) > 0)
    {
	XY_YZ(b, g, r)
	YX_ZY(b, g, r)
    }
    XY_YZ(b, g, r)
    --buf;
    --nxt;
    XY_YZ(b, g, r)

    return buf + 1;
}

template <class S, class T> static const S*
bayerBGGROdd3x3(const S* buf, T* rgb, int w)
{
    const S	*nxt = buf + w;			// next line
    YX_ZY(r, g, b)				// 左端の画素は2x2で処理
    const S	*prv = buf - w;			// previous line
    while ((w -= 2) > 0)			// 奇数行中間の列を処理
    {
	XYX_YZY_XYX(b, g, r)
	yXy_ZYZ_yXy(b, g, r)
    }
    --buf;
    --nxt;
    YX_ZY(r, g, b)				// 右端の画素は2x2で処理

    return buf + 1;
}

template <class S, class T> static const S*
bayerBGGREven3x3(const S* buf, T* rgb, int w)
{
    const S	*nxt = buf + w;			// next line
    XY_YZ(b, g, r)				// 左端の画素は2x2で処理
    const S	*prv = buf - w;			// previous line
    while ((w -= 2) > 0)			// 偶数行中間の列を処理
    {
	yXy_ZYZ_yXy(r, g, b)
	XYX_YZY_XYX(r, g, b)
    }
    --buf;
    --nxt;
    XY_YZ(b, g, r)				// 右端の画素は2x2で処理

    return buf + 1;
}

template <class S, class T> static const S*
bayerGRBG2x2(const S* buf, T* rgb, int w)
{
    const S*	nxt = buf + w;			// next line
    while ((w -= 2) > 0)
    {
	YX_ZY(r, g, b)
	XY_YZ(r, g, b)
    }
    YX_ZY(r, g, b)
    --buf;
    --nxt;
    YX_ZY(r, g, b)

    return buf + 1;
}

template <class S, class T> static const S*
bayerGRBGOdd3x3(const S* buf, T* rgb, int w)
{
    const S	*nxt = buf + w;			// next line
    XY_YZ(b, g, r)				// 左端の画素は2x2で処理
    const S	*prv = buf - w;			// previous line
    while ((w -= 2) > 0)			// 奇数行中間の列を処理
    {
	yXy_ZYZ_yXy(r, g, b)
	XYX_YZY_XYX(r, g, b)
    }
    --buf;
    --nxt;
    XY_YZ(b, g, r)				// 右端の画素は2x2で処理

    return buf + 1;
}

template <class S, class T> static const S*
bayerGRBGEven3x3(const S* buf, T* rgb, int w)
{
    const S	*nxt = buf + w;			// next line
    YX_ZY(r, g, b)				// 左端の画素は2x2で処理
    const S	*prv = buf - w;			// previous line
    while ((w -= 2) > 0)			// 偶数行中間の列を処理
    {
	XYX_YZY_XYX(b, g, r)
	yXy_ZYZ_yXy(b, g, r)
    }
    --buf;
    --nxt;
    YX_ZY(r, g, b)				// 右端の画素は2x2で処理

    return buf + 1;
}

template <class S, class T> static const S*
bayerGBRG2x2(const S* buf, T* rgb, int w)
{
    const S*	nxt = buf + w;			// next line
    while ((w -= 2) > 0)
    {
	YX_ZY(b, g, r)
	XY_YZ(b, g, r)
    }
    YX_ZY(b, g, r)
    --buf;
    --nxt;
    YX_ZY(b, g, r)

    return buf + 1;
}

template <class S, class T> static const S*
bayerGBRGOdd3x3(const S* buf, T* rgb, int w)
{
    const S	*nxt = buf + w;			// next line
    XY_YZ(r, g, b)				// 左端の画素は2x2で処理
    const S	*prv = buf - w;			// previous line
    while ((w -= 2) > 0)			// 奇数行中間の列を処理
    {
	yXy_ZYZ_yXy(b, g, r)
	XYX_YZY_XYX(b, g, r)
    }
    --buf;
    --nxt;
    XY_YZ(r, g, b)				// 右端の画素は2x2で処理

    return buf + 1;
}

template <class S, class T> static const S*
bayerGBRGEven3x3(const S* buf, T* rgb, int w)
{
    const S	*nxt = buf + w;			// next line
    YX_ZY(b, g, r)				// 左端の画素は2x2で処理
    const S	*prv = buf - w;			// previous line
    while ((w -= 2) > 0)			// 偶数行中間の列を処理
    {
	XYX_YZY_XYX(r, g, b)
	yXy_ZYZ_yXy(r, g, b)
    }
    --buf;
    --nxt;
    YX_ZY(b, g, r)				// 右端の画素は2x2で処理

    return buf + 1;
}

static void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nDecode bayer-encoded pbm images input from stdin and output to stdout.\n"
	 << endl;
    cerr << " Usage: " << s << " [-b bayer]\n"
	 << endl;
    cerr << " options.\n"
	 << "  -b [0-3]:  bayer pattern (0: RGGB, 1: BGGR, 2: GRBG, 3: GBRG)\n"
	 << "  -h:        print this\n"
	 << endl;
}

static Image<RGB>
decodeBayer(const Image<u_char>& in, Bayer bayer)
{
    using namespace	std;

    Image<RGB>	out(in.width(), in.height());
    out.P  = in.P;
    out.d1 = in.d1;
    out.d2 = in.d2;
    
    switch (bayer)
    {
      case RGGB:
      {
	const u_char*	p = bayerRGGB2x2(&in[0][0], &out[0][0], in.width());
	int		v = 1;
	while (v < out.height() - 1)	// 中間の行を処理
	{
	    p = bayerRGGBOdd3x3 (p, &out[v++][0], in.width());
	    p = bayerRGGBEven3x3(p, &out[v++][0], in.width());
	}
	bayerRGGB2x2(p - in.width(), &out[v][0], in.width());
      }
        break;

      case BGGR:
      {
	const u_char*	p = bayerBGGR2x2(&in[0][0], &out[0][0], in.width());
	int		v = 1;
	while (v < out.height() - 1)	// 中間の行を処理
	{
	    p = bayerBGGROdd3x3 (p, &out[v++][0], in.width());
	    p = bayerBGGREven3x3(p, &out[v++][0], in.width());
	}
	bayerBGGR2x2(p - in.width(), &out[v][0], in.width());
      }
        break;

      case GRBG:
      {
	const u_char*	p = bayerGRBG2x2(&in[0][0], &out[0][0], in.width());
	int		v = 1;
	while (v < out.height() - 1)	// 中間の行を処理
	{
	    p = bayerGRBGOdd3x3 (p, &out[v++][0], in.width());
	    p = bayerGRBGEven3x3(p, &out[v++][0], in.width());
	}
	bayerGRBG2x2(p - in.width(), &out[v][0], in.width());
      }
	break;

      default:
      {
	const u_char*	p = bayerGBRG2x2(&in[0][0], &out[0][0], in.width());
	int		v = 1;
	while (v < out.height() - 1)	// 中間の行を処理
	{
	    p = bayerGBRGOdd3x3 (p, &out[v++][0], in.width());
	    p = bayerGBRGEven3x3(p, &out[v++][0], in.width());
	}
	bayerGBRG2x2(p - in.width(), &out[v][0], in.width());
      }
        break;
    }

    return out;
}
    
}

/************************************************************************
*  global functions							*
************************************************************************/
int
main(int argc, char* argv[])
{
    using namespace	std;
    using namespace	TU;

    Bayer	bayer = RGGB;
    extern char	*optarg;
    extern int	optind;
    for (int c; (c =getopt(argc, argv, "b:h")) != EOF; )
	switch (c)
	{
	  case 'b':
	    switch (atoi(optarg))
	    {
	      default:
		bayer = RGGB;
		break;
	      case 1:
		bayer = BGGR;
		break;
	      case 2:
		bayer = GRBG;
		break;
	      case 3:
		bayer = GBRG;
		break;
	    }
	    break;
	    
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}

    try
    {
#ifdef WIN32
	if (_setmode(_fileno(stdin), _O_BINARY) == -1)
	    throw runtime_error("Cannot set stdin to binary mode!!"); 
	if (_setmode(_fileno(stdout), _O_BINARY) == -1)
	    throw runtime_error("Cannot set stdout to binary mode!!"); 
#endif
	for (Image<u_char> in; in.restore(cin); )
	{
	    Image<RGB>	out = decodeBayer(in, bayer);
	    out.save(cout);
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}

