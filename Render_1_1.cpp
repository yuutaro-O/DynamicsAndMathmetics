#ifndef INCLUDED_RENDER_1_1_H_
#include "Render1_1.h"
#endif // !INCLUDED_RENDER_1_1_H_
#ifndef INCLUDED_USERDRAWING_H_
#include "UserDrawing.h"
#endif // !INCLUDED_USERDRAWING_H_

//スキャンラインアルゴリズムのレンダリング
int Render1_1::RenderScanLine(void) {
	int x, y;
	int nBright;

	for (y = 0; y < VIEW_HEIGHT; y++) {
		for (x = 0; x < VIEW_WIDTH; x++) {
			nBright = y & 0xff;
			DrawPoints(x, y, nBright, nBright, 255);
		}
		FlushDrawingPictures();
	}
	return 0;
}