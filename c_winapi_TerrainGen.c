
#include <Windows.h>
#include "Drawing.h"

int running = 1;
int keys[128] = { 0 };

struct frameStruct frame = { 0 };


BITMAPINFO frame_bitmapInfo;
HBITMAP frame_hBitmap;
HDC frame_hdc;

HWND hWnd;

int showMap = 1;
int showMiniMap = 0;

#define map_size 100
#define miniMap_scale 5

float map[map_size][map_size] = { { 0 } };
#define map_realY 10
int map_realX = 10;
int tile_scale = 1;


#define num_genParams 3
int genParams[num_genParams] = { 12, 100, 100 };
enum {
	PARAM_PEAKHEIGHT,
	PARAM_TERRAINHEIGHT,
	PARAM_PEAKROUNDNESS
};

void map_Generate() {

	for (int i = 0; i < num_genParams; i++) {
		if (genParams[i] == 0) {
			genParams[i] = 1;
		};
	};

	for (int x = 0; x < map_size; x++) {

		for (int y = 0; y < map_size; y++) {

			map[x][y] = (rand() % 100) / 100.0;
		};

	};

	for (int i = 0; i < genParams[PARAM_PEAKROUNDNESS]; i++) {

		for (int x = 1; x < map_size - 1; x++) {

			for (int y = 1; y < map_size - 1; y++) {
				
				if (map[x][y] == 0) {
					continue;
				};

				if (rand() % genParams[PARAM_PEAKHEIGHT]) {
					continue;
				};

				map[x + 1][y] = (map[x][y] + map[x + 1][y]) / 2;
				map[x][y + 1] = (map[x][y] + map[x][y + 1]) / 2;

			};


		};

		for (int x = map_size - 1; x >= 1; x--) {

			for (int y = 1; y < map_size - 1; y++) {
				
				if (map[x][y] == 0) {
					continue;
				};

				if (rand() % genParams[PARAM_PEAKHEIGHT]) {
					continue;
				};

				map[x - 1][y] = (map[x][y] + map[x - 1][y]) / 2;
				map[x][y + 1] = (map[x][y] + map[x][y + 1]) / 2;

			};


		};

		for (int x = 1; x < map_size - 1; x++) {

			for (int y = map_size - 1; y >= 1; y--) {
				
				if (map[x][y] == 0) {
					continue;
				};

				if (rand() % genParams[PARAM_PEAKHEIGHT]) {
					continue;
				};

				map[x + 1][y] = (map[x][y] + map[x + 1][y]) / 2;
				map[x][y - 1] = (map[x][y] + map[x][y - 1]) / 2;

			};


		};

		for (int x = map_size - 1; x >= 1; x--) {

			for (int y = map_size - 1; y >= 1; y--) {
				
				if (map[x][y] == 0) {
					continue;
				};

				if (rand() % genParams[PARAM_PEAKHEIGHT]) {
					continue;
				};

				map[x - 1][y] = (map[x][y] + map[x - 1][y]) / 2;
				map[x][y - 1] = (map[x][y] + map[x][y - 1]) / 2;

			};


		};

		frame_DrawRectFilled(&frame, 10, 10, 110, 30, 0x002200, 0.5);
		frame_DrawRectFilled(&frame, 10, 10, 10 + 100 * (1.0 * i / genParams[PARAM_PEAKROUNDNESS]), 30, 0x009900, 0);
		InvalidateRect(hWnd, NULL, 0);
		UpdateWindow(hWnd);


	};

	return;
};


void map_Rotate(int dir) {

	if (dir == 0) {
		return;
	};

	float** tMap = malloc(map_size * sizeof(*tMap));	
	for (int i = 0; i < map_size; i++) {
		tMap[i] = malloc(map_size * sizeof(**tMap));
	};

	for (int x = 0; x < map_size; x++) {	// counterclockwise ('A')

		if (dir > 0) {
			break;
		};

		for (int y = 0; y < map_size; y++) {
			tMap[x][y] = map[y][map_size - x - 1];
		};

	};

	for (int y = 0; y < map_size; y++) {	// clockwise ('D')

		if (dir < 0) {
			break;
		};

		for (int x = 0; x < map_size; x++) {
			tMap[x][y] = map[map_size - y - 1][x];
		};


	};
	
	for (int x = 0; x < map_size; x++) {
		for (int y = 0; y < map_size; y++) {
			map[x][y] = tMap[x][y];
		};
	};

	free(tMap);

	return;
};




LRESULT CALLBACK WindProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	WNDCLASS wc = { };
	wc.lpszClassName = L"Class Name";
	wc.lpfnWndProc = WindProc;
	wc.hInstance = hInstance;

	RegisterClass(&wc);

	RECT rect;
	rect.left = 400;
	rect.top = 250;
	rect.right = 950;
	rect.bottom = 950;

	hWnd = CreateWindowEx(
		0,
		L"Class Name",
		L"Terrain Gen - 'N': gen, 'A/D': rotate, '1-2': selectParam, 'up/down': changeVal, 'Q/E': toggleMaps, 'R': randMap",
		WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0,
		0,
		hInstance,
		0
	);

	frame_bitmapInfo.bmiHeader.biSize = sizeof(frame_bitmapInfo.bmiHeader);
	frame_bitmapInfo.bmiHeader.biPlanes = 1;
	frame_bitmapInfo.bmiHeader.biBitCount = 32;
	frame_bitmapInfo.bmiHeader.biCompression = BI_RGB;

	frame_hdc = CreateCompatibleDC(frame_hdc);

	ShowWindow(hWnd, 1);


	int cooldown = 0;
	map_Generate();

	int paramSelected = 0;

	MSG msg;
	while (running) {

		PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE);

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		for (int i = 0; i < 128; i++) {
			keys[i] = HIWORD(GetKeyState(i));
		};

		if (keys[27]) {	// esc
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		};

		if (keys['G'] && (cooldown <= 0)) {	// generate
			map_Generate();
			cooldown = 5;
		};

		if (keys['A'] && (cooldown <= 0)) {	// rotation
			map_Rotate(-1);
			cooldown = 5;
		};
		if (keys['D'] && (cooldown <= 0)) {
			map_Rotate(1);
			cooldown = 5;
		};

		if (keys['Q'] && (cooldown <= 0)) {	// show/hide map
			showMiniMap = !showMiniMap;
			cooldown = 5;
		};
		if (keys['E'] && (cooldown <= 0)) {
			showMap = !showMap;
			cooldown = 5;
		};

		if (keys['R'] && (cooldown <= 0)) {	// random map
			for (int i = 0; i < num_genParams; i++) {
				genParams[i] = rand() % 200;
			};
			map_Generate();
			cooldown = 5;
		};

		cooldown--;


		for (int i = 0; i < num_genParams; i++) {

			if (!keys['1' + i]) {
				continue;
			};

			paramSelected = i;

		};


		if (GetAsyncKeyState(VK_UP)) {
			genParams[paramSelected] += 2;
		};

		if (GetAsyncKeyState(VK_DOWN)) {
			genParams[paramSelected] -= 2;
		};

		for (int i = 0; i < frame.width * frame.height; i++) {
			frame.pixels[i] = 0;
		};

		
		for (int x = map_size - 1; x >= 0; x--) {	// draw map

			if (!showMap) {
				break;
			};

			for (int y = map_size - 1; y >= 0; y--) {

				if (map[x][y] <= 0) {
					continue;
				};

				int height = (int)(1.0 * map[x][y] * map_size + .5);
				int bottomHeight = 0;

				int h1 = 0;
				int h2 = 0;
				if (x > 0 && y > 0) {
					h1 = (int)(1.0 * map[x - 1][y] * map_size + .5);
					h2 = (int)(1.0 * map[x][y - 1] * map_size + .5);
				};

				bottomHeight = h1;
				if (h2 < bottomHeight) {
					bottomHeight = h2;
				};

				bottomHeight--;

				if (bottomHeight > height) {
					continue;
				};

				float colorMult = 1.4 * height / map_size;

				int color = 0xffffff;

				if (map[x][y] < .55 * (genParams[PARAM_TERRAINHEIGHT] / 100.0)) {	// grass
					color = 0x00ff00;
				};

				if (map[x][y] < .49 * (genParams[PARAM_TERRAINHEIGHT] / 100.0)) {	// sand
					color = 0xeecc99;
				};

				if (map[x][y] < .48 * (genParams[PARAM_TERRAINHEIGHT] / 100.0)) {	// water
					color = 0x0000ff;
				};

				if (map[x][y] < .44 * (genParams[PARAM_TERRAINHEIGHT] / 100.0)) {	// deepWater
					color = 0x000088;
				};

				short r = GetRValue(color) * colorMult;
				short g = GetGValue(color) * colorMult;
				short b = GetBValue(color) * colorMult;

				int x1 = tile_scale * (x - y);
				int y1 = tile_scale * (x + y + bottomHeight);
				int x2 = tile_scale * (x - y + 1);
				int y2 = tile_scale * (x + y + bottomHeight + 1);
				int x3 = x2;
				int y3 = tile_scale * (x + y + height + 1);
				int x4 = x1;
				int y4 = tile_scale * (x + y + height);

				frame_DrawTriangleFilled(&frame,
					x1 + map_realX,
					y1 + map_realY,
					x2 + map_realX,
					y2 + map_realY,
					x3 + map_realX,
					y3 + map_realY,
					RGB((int)(r * .3), (int)(g * .3), (int)(b * .3))
				);
				frame_DrawTriangleFilled(&frame,
					x1 + map_realX,
					y1 + map_realY,
					x3 + map_realX,
					y3 + map_realY,
					x4 + map_realX,
					y4 + map_realY,
					RGB((int)(r * .3), (int)(g * .3), (int)(b * .3))
				);				

				x2 = tile_scale * (x - y - 1);
				y2 = tile_scale * (x + y + 1 + bottomHeight);
				x3 = x2;
				y3 = tile_scale * (x + y + 1 + height);

				frame_DrawTriangleFilled(&frame,
					x1 + map_realX,
					y1 + map_realY,
					x2 + map_realX,
					y2 + map_realY,
					x3 + map_realX,
					y3 + map_realY,
					RGB((int)(r * .4), (int)(g * .4), (int)(b * .4))
				);
				frame_DrawTriangleFilled(&frame,
					x1 + map_realX,
					y1 + map_realY,
					x3 + map_realX,
					y3 + map_realY,
					x4 + map_realX,
					y4 + map_realY,
					RGB((int)(r * .4), (int)(g * .4), (int)(b * .4))
				);
					
				x1 = tile_scale * (x - y + 1);
				y1 = tile_scale * (x + y + height + 1);
				x2 = tile_scale * (x - y);
				y2 = tile_scale * (x + y + height + 2);

				frame_DrawTriangleFilled(&frame,
					x1 + map_realX,
					y1 + map_realY,
					x2 + map_realX,
					y2 + map_realY,
					x3 + map_realX,
					y3 + map_realY,
					RGB((int)(r * .6), (int)(g * .6), (int)(b * .6))
				);
				frame_DrawTriangleFilled(&frame,
					x1 + map_realX,
					y1 + map_realY,
					x3 + map_realX,
					y3 + map_realY,
					x4 + map_realX,
					y4 + map_realY,
					RGB((int)(r * .6), (int)(g * .6), (int)(b * .6))
				);


			};


		};
		

		for (int x = 0; x < map_size; x++) {	// draw miniMap
			
			if (!showMiniMap) {
				break;
			};

			for (int y = 0; y < map_size; y++) {

				int color = RGB_BLEND(0xffffff, 0, (int)(map[x][y] * 100.0));

				for (int iX = 0; iX < tile_scale; iX++) {


					for (int iY = 0; iY < tile_scale; iY++) {

						int rX = (x * tile_scale + iX + 10);
						int rY = (y * tile_scale + iY + 10);

						if (rX < 0 || rX >= frame.height) {
							continue;
						};

						if (rY < 0 || rY >= frame.height) {
							continue;
						};
						
						frame.pixels[rY * frame.width + rX] = color;

					};

				};

			};


		};



		char text[string_maxLength] = { 0 };	// display values

		for (int i = 0; i < num_genParams; i++) {

			text_ClearString(text);

			if (i == paramSelected) {
				text_AppendString(text, ">");
			};

			switch (i) {

			case (PARAM_PEAKHEIGHT): {
				text_AppendString(text, "PeakHeight: ");
				break;
			}

			case (PARAM_TERRAINHEIGHT): {
				text_AppendString(text, "TerrainHeight: ");
				break;
			}

			case (PARAM_PEAKROUNDNESS): {
				text_AppendString(text, "PeakRoundness: ");
				break;
			}
			};

			text_AppendInt(text, genParams[i]);
			frame_DrawText(&frame, 10, frame.height - 20 * (i + 1), text, 2, 2, -1, 0);

		};

		frame_DrawText(&frame, 20, frame.height / 2, "<A", 2, 2, -1, 0);
		frame_DrawText(&frame, frame.width - 45, frame.height / 2, "D>", 2, 2, -1, 0);

		frame_DrawText(&frame, 10, 20, "Q", 2, 2, -1, 0);

		InvalidateRect(hWnd, NULL, 0);
		UpdateWindow(hWnd);
		
		Sleep(20);

	};


	return 0;

};


LRESULT CALLBACK WindProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


	switch (uMsg) {

	case (WM_SIZE): {

		frame_bitmapInfo.bmiHeader.biWidth = LOWORD(lParam);
		frame_bitmapInfo.bmiHeader.biHeight = HIWORD(lParam);

		frame_hBitmap = CreateDIBSection(frame_hdc, &frame_bitmapInfo, BI_RGB, &frame.pixels, NULL, NULL);

		SelectObject(frame_hdc, frame_hBitmap);
		                                                                                           
		frame.width = LOWORD(lParam);
		frame.height = HIWORD(lParam);

		tile_scale = (int)(1.0 * ((frame.height - map_realY) / map_size) / 2.6);
		if (tile_scale == 0) {
			tile_scale++;
		};
		map_realX = frame.width / 2;

		break;
	};

	case (WM_PAINT): {

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BitBlt(
			hdc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			ps.rcPaint.right - ps.rcPaint.left,
			ps.rcPaint.bottom - ps.rcPaint.top,
			frame_hdc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			SRCCOPY
		);

		EndPaint(hWnd, &ps);
		break;

	};

	case (WM_QUIT): {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		PostQuitMessage(0);
		break;
	};

	case (WM_CLOSE): {
		DestroyWindow(hWnd);
		running = 0;
		break;
	};

	default: {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	};

	return 0;

};
