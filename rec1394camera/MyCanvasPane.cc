/*
 *  $Id$
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
