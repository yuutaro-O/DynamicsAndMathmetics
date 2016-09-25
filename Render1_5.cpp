#ifndef INCLUDED_RENDER_1_5_H_
#include "Render1_5.h"
#endif // !INCLUDED_RENDER_1_5_H_
#ifndef INCLUDED_DXLENDERLING_H_
#include "DxLenderling.h"
#endif // !INCLUDED_DXLENDERLING_H_
#ifndef INCLUDED_USERDRAWING_H_
#include "UserDrawing.h"
#endif

int Render1_5::RenderScanLine(void) {
	int x, y;
	int nBright;

	for (y = 0; y < VIEW_HEIGHT; y++) {
		for (x = 0; x < VIEW_WIDTH; x++) {
			nBright = abs((((x + y) * 2) & 0x1ff) - 255);
			DrawPoints(x, y, nBright, nBright, 255);
		}
		FlushDrawingPictures();
	}
	return 0;
}
