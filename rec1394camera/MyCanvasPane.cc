/*
 *  $Id: MyCanvasPane.cc,v 1.1 2010-12-21 00:13:25 ueshiba Exp $
 */
#include "MyCanvasPane.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCanvasPane							*
************************************************************************/
void
MyCanvasPane::repaintUnderlay()
{
    _dc << _image;
}

}
}
