
#include <windows.h>
#include <math.h>
#include <wingdi.h>


int running = 1;

struct {
	int width;
	int height;
	int* pixel;
} frame = { 0 };

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap = 0;
static HDC frame_device_context = 0;

int key[128] = { 0 };

struct {
	int x1;
	int y1;
	int x2;
	int y2;
} console = { 10, 10, 1225, 800 };


void Line(int x1, int y1, int x2, int y2, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	int width = frame.width;
	int height = frame.height;

	if ((x1 > width) || (x2 > width) || (x1 < 0) || (x2 < 0)) {
		return;
	};

	if ((y1 > height) || (y2 > height) || (y1 < 0) || (y2 < 0)) {
		return;
	};

	//ray: rx = x1 + deltaX * t
	// ry = y1 + deltaY * t
	// as t ->, goes along the line

	double deltaX = x2 - x1;
	double deltaY = y2 - y1;
	
	// dist = sqrt( (x-h)^2 + (y-k)^2 )
	double hyp = sqrt( deltaX * deltaX + deltaY * deltaY );
	double inc = 1.0 / hyp;

	int pixelNum = frame.width * frame.height;

	for (double t = 0; t < 1; t += inc) {
			
		int pixel = (int)(frame.width * (int)(deltaY * t + y1) + (int)(deltaX * t + x1));

		if (pixel > pixelNum) {
			continue;
		};

		if (pixel < 0) {
			continue;
		};

		frame.pixel[pixel] = color;
	
	};
	
	return;
};


void LineA(int x1, int y1, int angle, int hyp, int color) {

	int x2 = x1 + (int)(hyp * cos(angle * (3.14 / 180)));
	int y2 = y1 + (int)(hyp * sin(angle * (3.14 / 180)));
	
	Line(x1, y1, x2, y2, color);
	
	return;
};


void Rect(int x1, int y1, int x2, int y2, int color) {

	Line(x1, y1, x2, y1, color);
	Line(x2, y1, x2, y2, color);
	Line(x2, y2, x1, y2, color);
	Line(x1, y2, x1, y1, color);

	return;
};


void RectFilled(int x1, int y1, int x2, int y2, int color) {

	if ((y1 > frame.height) || (y2 > frame.height) || (y1 < 0) || (y2 < 0)) {
		y1 = console.y1;
		y2 = console.y2;
	};

	if ((x1 > frame.width) || (x2 > frame.width) || (x1 < 0) || (x2 < 0)) {
		return;
	};

	for (int x = x1; x < x2; x++) {
		for (int y = y1; y < y2; y++) {

			frame.pixel[frame.width * y + x] = color;

		};
	};

};


double CollisionDistance(double x1, double y1, double angle, int vertices[5]) {

	int x2 = vertices[1];
	int y2 = vertices[2];
	int x3 = vertices[3];
	int y3 = vertices[4];

	double deltaX1 = cos(angle * 3.14 / 180);
	double deltaY1 = sin(angle * 3.14 / 180);

	double deltaX2 = vertices[3] - vertices[1];
	double deltaY2 = vertices[4] - vertices[2];

	double v = ( deltaX1 * (y2 - y1) + (deltaY1) * (x1 - x2) ) / ( (deltaY1 * deltaX2) - (deltaX1 * deltaY2) );

	double u = x2 - 1 + (deltaX2 * v);
	
	if (deltaX1 != 0) {
		u = ((x2 - x1) + (deltaX2 * v)) / (deltaX1);
	};
	
	double pointX = (int)(x2 + deltaX2 * v);
	double pointY = (int)(y2 + deltaY2 * v);
	
	if (((pointY < y2) == (pointY < y3)) && ((pointX < x2) == (pointX < x3))) {	// if outside of line
		return -1;
	};

	if ((u <= 0) || (v <= 0)) {
		return -1;
	};

	return (sqrt(((pointX - x1) * (pointX - x1)) + ((pointY - y1) * (pointY - y1))));
};


int GetEnd(int arr[][6], int len) {	//index of last element

	for (int i = 0; i < len; i++) {

		if (arr[i][0] == 0) {
			return (i - 1);
		};

	};

	return -1;

};


//console size: (0, 0) -> (780, 510)
// frame: (10, 10, 750, 500)
static int vertices[200][6] = {

	{ 1, 10, 10, 300, 10, 0x000000E9 },
	{ 1, 10, 500, 300, 500, 0x000000E9 },
	{ 1, 10, 10, 10, 500, 0x0000E9 },

	{ 2, 30, 10, 40, 100, 0x00CF0000 },
	{ 2, 60, 10, 70, 60, 0x00CF0000 },
	{ 2, 60, 70, 70, 100, 0x00CF0000 },
	{ 2, 30, 10, 70, 30, 0x00CF0000 },		//open house

	{ 2, 110, 90, 115, 110, 0x00CF9900 },
	{ 2, 110, 30, 115, 80, 0x00CF9900 },
	{ 2, 110, 30, 280, 35, 0x00CF9900 },	
	{ 2, 175, 35, 180, 40, 0x00CF9900 },
	{ 2, 175, 50, 180, 80, 0x00CF9900 },
	{ 1, 180, 80, 115, 110, 0x00CF9900 },
	{ 1, 175, 80, 110, 110, 0x00CF9900 },	//house
	
	{ 2, 140, 50, 150, 60, 0x00222222 },	//gray cube in house

	{ 2, 220, 80, 225, 85, 0x00505020 },
	{ 2, 215, 60, 220, 65, 0x00505020 },
	{ 2, 240, 50, 245, 55, 0x00505020 },
	{ 2, 250, 70, 255, 75, 0x00505020 },
	{ 2, 240, 90, 245, 95, 0x00505020 },	// small stonehenge

	{ 2, 30, 250, 40, 400, 0x00CF0000 },
	{ 2, 50, 250, 70, 400, 0x00CF0000 },	//tunnel

	{ 2, 40, 220, 50, 210, 0x00CF0000 },
	{ 2, 20, 200, 30, 190, 0x00CF0000 },
	{ 2, 60, 180, 70, 190, 0x00CF0000 },
	{ 2, 30, 160, 40, 170, 0x00CF0000 },	//red stonehenge

	{ 2, 300, 10, 310, 150, 0x0000A0E9 },
	{ 2, 300, 170, 310, 500, 0x0000A0E9 }, // playground exit wall
	
	{ 1, 120, 270, 120, 500, 0x0000A0E9 }, 
	{ 1, 300, 220, 200, 220, 0x0000A0E9 },
	{ 1, 180, 220, 120, 270, 0x0000A0E9 },	// sep room entrance
	
	{ 1, 180, 220, 190, 300, 0x0000A0E9 },	
	{ 1, 200, 220, 200, 300, 0x0000A0E9 },	// sep tapering tunnel
	
	{ 1, 190, 300, 190, 350, 0x0000A0E9 },
	{ 1, 200, 300, 205, 350, 0x0000A0E9 },
	{ 1, 205, 350, 200, 400, 0x00993355 },
	{ 1, 190, 350, 160, 370, 0x00993355 },
	{ 1, 160, 370, 160, 300, 0x00993355 },
	{ 1, 150, 400, 150, 330, 0x00993355 },
	{ 1, 200, 400, 170, 420, 0x00993355 },
	{ 1, 170, 420, 150, 400, 0x00993355 },
	{ 1, 150, 330, 130, 350, 0x00993355 },
	{ 1, 130, 350, 130, 280, 0x00993355 },
	{ 1, 130, 280, 160, 270, 0x00993355 },
	{ 1, 160, 270, 180, 300, 0x00993355 },
	{ 1, 160, 300, 180, 300, 0x00993355 },	// purple rooms

	{ 2, 135, 330, 139, 334, 0x00505020 },
	{ 2, 150, 290, 154, 294, 0x00505020 },
	{ 2, 140, 290, 144, 294, 0x00505020 },	// teleporter in purple room

	{ 1, 144, 292, 150, 292, 0x00000000 },	// teleporter effect

	{ 2, 210, 200, 220, 180, 0x0000FF00 },
	{ 2, 230, 200, 240, 180, 0x0000FF00 },
	{ 2, 250, 200, 260, 180, 0x0000FF00 },
	{ 2, 270, 200, 280, 180, 0x0000FF00 },

	{ 2, 330, 140, 335, 145, 0x00FFFFFF },
	{ 2, 350, 140, 355, 145, 0x00FFFFFF },
	{ 2, 370, 140, 375, 145, 0x00FFFFFF },
	{ 2, 390, 140, 395, 145, 0x00FFFFFF },
	{ 2, 410, 140, 415, 145, 0x00FFFFFF }

};


void SetArray(int arr[][6], int i, int arr2[6]) {

	arr[i][0] = arr2[0];
	arr[i][1] = arr2[1];
	arr[i][2] = arr2[2];
	arr[i][3] = arr2[3];
	arr[i][4] = arr2[4];
	arr[i][5] = arr2[5];

	return;
};


static void CalculateVertices() {

	int iLast = GetEnd(vertices, (sizeof(vertices) / sizeof(vertices[0])));

	for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {

		if (vertices[i][0] == 2) {

			int newPoints1[6] = { 1, vertices[i][1], vertices[i][4], vertices[i][3], vertices[i][4], vertices[i][5] };
			SetArray(&vertices, (iLast + 1), newPoints1);

			int newPoints2[6] = { 1, vertices[i][3], vertices[i][4], vertices[i][3], vertices[i][2], vertices[i][5] };
			SetArray(&vertices, (iLast + 2), newPoints2);

			int newPoints3[6] = { 1, vertices[i][3], vertices[i][2], vertices[i][1], vertices[i][2], vertices[i][5] };
			SetArray(&vertices, (iLast + 3), newPoints3);

			int newPoints[6] = { 1, vertices[i][1], vertices[i][2], vertices[i][1], vertices[i][4], vertices[i][5] };
			SetArray(&vertices, i, newPoints);	//replace first command w/ data

			iLast = (iLast + 3);

		};

	};

	return;
};


int DarkenColor(int color, double percent) {

	if (percent <= 0) {
		return color;
	};
	
	percent = 1 - percent / 100.0;

	color = RGB(GetRValue(color) * percent, GetGValue(color) * percent, GetBValue(color) * percent);

	return color;
};


static int fov = 60;
static const int numRays = 150;

struct {
	double x;
	double y;
	double dir;
	double speed;
} plr = { 20, 70, 90, .4 };

static int overlayOn = 0;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	const wchar_t CLASS_NAME[] = L"Class Name";

	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;

	RegisterClass(&wc);

	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	RECT rect;
	rect.left = 250;
	rect.top = 100;
	rect.right = rect.left + 1000;
	rect.bottom = rect.top + 750;

	// b/f create window, fill out bitmap info
	frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
	frame_bitmap_info.bmiHeader.biPlanes = 1;
	frame_bitmap_info.bmiHeader.biBitCount = 32;
	frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
	frame_device_context = CreateCompatibleDC(0);


	HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"Title", style, rect.left, rect.top, rect.right, rect.bottom, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	BringWindowToTop(hWnd);

	CalculateVertices();	// Create seperate vertices for each rectangle
	
	MSG msg = { };
	int i = 0;
	while (running == 1) {

		PeekMessage(&msg, hWnd, 0u, 0u, PM_REMOVE);

		TranslateMessage(&msg);
		DispatchMessage(&msg);


		for (int i = 0; i < 127; i++) {
			// 65 - 90
			// returns (up/down)(toggle), 1 or 0 for each
			key[i] = 0;
			if (GetKeyState(i) / 10) {
				key[i] = 1;
			};
		};

		if (key[65]) {	//A
			plr.dir -= 1;
		};
		if (key[68]) {	//D
			plr.dir += 1;
		};
		if (key[87]) {	//W

			plr.x += plr.speed * cos(plr.dir * 3.14 / 180);
			plr.y += plr.speed * sin(plr.dir * 3.14 / 180);

		};
		if (key[83]) {	//S

			plr.x -= plr.speed * cos(plr.dir * 3.14 / 180);
			plr.y -= plr.speed * sin(plr.dir * 3.14 / 180);
		};

		if (key[27]) {	//esc
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		};



		RectFilled(0, 0, frame.width, frame.height, 0x00000000);	//black bg

		int consoleCenter = (console.y2 - console.y1) / 2;

		RectFilled(console.x1, console.y1, console.x2, consoleCenter, 0x00002000);	//grass
		RectFilled(console.x1, consoleCenter, console.x2, console.y2, RGB(255, 150, 100));	//sky
		
		
		double raySpacing = (console.x2 - console.x1 * 1.0) / numRays;

		int fovLeft = plr.dir - (fov / 2);
		int fovRight = plr.dir + (fov / 2);

		int ray = 0;

		for (double a = fovLeft; a <= fovRight; a += (1.0 * fov / numRays)) {

			double dist = 100000;
			int line = 0;
			
			for (int l = 0; l < sizeof(vertices) / sizeof(vertices[0]); l++) {

				double tempDist = CollisionDistance(plr.x, plr.y, a, vertices[l]);

				if (tempDist < 0) {
					continue;
				};
				
				if (tempDist < dist) {
					dist = tempDist;
					line = l;
				};

			};
			
			int lineAngle = 50;

			if (vertices[line][3] - vertices[line][1] != 0) {
				lineAngle = (vertices[line][4] - vertices[line][2]) / (vertices[line][3] - vertices[line][1]);
			};

			lineAngle = abs(lineAngle);

			
			int color = vertices[line][5];
			
			if (color > 0) {
				color = DarkenColor(vertices[line][5], (lineAngle / 3));
				color = DarkenColor(color, (ray % 2) * 5);
			};
			
			if (color == 0) {	// teleporter effect
				color = RGB(200, 0, 200);
				color = DarkenColor(color, rand() % 20);
			};

			dist = 5000 / dist;
			RectFilled(console.x1 + (ray * raySpacing), consoleCenter - (dist / 2), console.x1 + ((ray + 1) * (raySpacing)), consoleCenter + (dist / 2), color);	//draw rects

			ray++;	
			
		};		
		

		if (overlayOn == 0) {
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
			Sleep(1);
			continue;
		};
		
		//2d overlay

		for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
			for (int v = 1; v < 5; v++) {
				Line(vertices[i][1], vertices[i][2], vertices[i][3], vertices[i][4], vertices[i][5]);
			};
		};
		
		LineA(plr.x, plr.y, fovLeft, 20, -1);
		LineA(plr.x, plr.y, fovRight, 20, -1);

		
		
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
		Sleep(1);
	};


};


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {

	case WM_DESTROY:
		running = 0;
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		//if (MessageBox(hWnd, L"Sure?", L"Confirm", MB_YESNO) == 6) {
			DestroyWindow(hWnd);
		//};
		return 0;

	case WM_PAINT: {
		
		static PAINTSTRUCT paint;
		static HDC device_context;
		device_context = BeginPaint(hWnd, &paint);

		BitBlt (device_context,
				paint.rcPaint.left, paint.rcPaint.top,
				paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
				frame_device_context,
				paint.rcPaint.left, paint.rcPaint.top,
				SRCCOPY);


		EndPaint(hWnd, &paint);

		return 0;
		};


	case WM_SIZE: {
		frame_bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
		frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

		if (frame_bitmap) {
			DeleteObject(frame_bitmap);
		};
		frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, &frame.pixel, 0, 0);
		SelectObject(frame_device_context, frame_bitmap);

		frame.width = LOWORD(lParam);
		frame.height = HIWORD(lParam);

		return 0;
		};
};

return DefWindowProc(hWnd, uMsg, wParam, lParam);
};

