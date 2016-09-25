#ifndef INCLUDED_RENDER1_4_H_
#define INCLUDED_RENDER1_4_H_
#ifndef INCLUDED_DXLENDERLING_H_
#include "DxLenderling.h"
#endif // !INCLUDED_DXLENDERLING_H_

class Render1_4 {
private:
	Render1_4();
	~Render1_4();
public:
	static int RenderScanLine(void);
};

#endif // !INCLUDED_RENDER1_4_H_
