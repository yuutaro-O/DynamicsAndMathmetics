#ifndef INCLUDED_RENDER1_4_H_
#include "Render1_4.h"
#endif // !INCLUDED_RENDER1_4_H_
#ifndef INCLUDED_USERDRAWING_H_
#include "UserDrawing.h"
#endif // !INCLUDED_USERDRAWING_H_

int Render1_4::RenderScanLine(void) {
	int x, y;
	int nBright;

	for (y = 0; y < VIEW_HEIGHT; y++) {
		for (x = 0; x < VIEW_WIDTH; x++) {
			nBright = (x + (y / 2)) & 0xff;
			DrawPoints(x, y, nBright, nBright, 255);
		}
		FlushDrawingPictures();
	}
	return 0;
}