#ifndef INCLUDED_USERDRAWING_H_
#include "UserDrawing.h"
#endif // !INCLUDED_USERDRAWING_H_
//#ifndef INCLUDED_RENDER1_1_H_
//#include "Render1_1.h"
//#endif // !INCLUDED_RENDER1_1_H_
#ifndef INCLUDED_RENDER1_2_H_
#include "Render_1_2.h"
#endif // !INCLUDED_RENDER1_2_H_

int UserDrawing(void){
	Render1_2::RenderScanLine();
	return 0;
}