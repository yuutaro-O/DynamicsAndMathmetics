#ifndef INCLUDED_USERRENDERING_H_
#include "UserDrawing.h"
#endif // !INCLUDED_USERRENDERING_H_
#ifndef INCLUDED_RENDER_1_2_H_
#include "Render_1_2.h"
#endif // !INCLUDED_RENDER_1_2_H_

int Render1_2::RenderScanLine(void) {
	int x, y;
	int nBright;

	for (y = 0; y < VIEW_HEIGHT; y++) {
		for (x = 0; x < VIEW_WIDTH; x++) {
			nBright = x & 0xff;
			DrawPoints(x, y, nBright, nBright, 255);
		}
		FlushDrawingPictures();
	}
	return 0;
}
