#ifndef INCLUDED_RENDER_1_2_H_
#define INCLUDED_RENDER_1_2_H_
#ifndef  INCLUDED_DXLENDERLING_H_
#include "DxLenderling.h"
#endif // ! INCLUDED_DXLENDERLING_H_
class Render1_2{
private:
	Render1_2();
	~Render1_2();
public:
	static int RenderScanLine(void);
};
#endif // !INCLUDED_RENDER_1_2_H_
