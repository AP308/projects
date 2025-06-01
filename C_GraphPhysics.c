#include <stdio.h>
#include <Windows.h>
#include <Math.h>

#include "Graphics.h"
#include "Util.h"


struct frameStruct frame = { 0 };

static BITMAPINFO frame_bitmapInfo;	// bitmap info
static HBITMAP frame_hBitmap = 0;	// bitmap handle
static HDC frame_dc = 0;	// frame device context

int keys[128] = { 0 };
int running = 1;

int sim_running = 0;

struct {
	int offsetX;
	int offsetY;
	double scaleX;
	double scaleY;
} function = { -250, -200, 1, 1 };

#define function_quality 2	// determines how long line approximations are (higher = more blocky, lower = more detail)

struct {
	double x;
	double v;
} obj1 = { 3, .5 };


double f(double x) {

	//double y = x * x;
	//double y = 10.0 - sqrt((x * x) + 5.0 * cos(x + .2)) + 1 * sin(x);
	double y = (0.25 * x * x) - (10 * x) + (24 * sqrt(x));
	return (y / function.scaleY) - function.offsetY;

};

double f_prime(double x) {

	//double y = 2*x;
	//double y = (-1 / (2 * sqrt(x * x + 5 * cos(x + 0.2)))) * (2 * x - 5 * sin(x + 0.2)) + cos(x);
	double y = (0.5 * x) - 10 + (12 / sqrt(x));
	return y;

};



LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {


	wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASS wc = {  };
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	frame_bitmapInfo.bmiHeader.biSize = sizeof(frame_bitmapInfo.bmiHeader);
	frame_bitmapInfo.bmiHeader.biPlanes = 1;
	frame_bitmapInfo.bmiHeader.biBitCount = 32;
	frame_bitmapInfo.bmiHeader.biCompression = BI_RGB;
	frame_dc = CreateCompatibleDC(frame_dc);

	DWORD style = WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU;

	RECT rect;
	rect.left = 250;
	rect.top = 150;
	rect.right = rect.left + 600;
	rect.bottom = rect.top + 500;

	HWND hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Title",
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWnd, nCmdShow);



	int sim_time = 0;
	int hotkey_run = 20;

	MSG msg = { };

	while (running == 1) {

		PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE);

		TranslateMessage(&msg);
		DispatchMessage(&msg);


		for (int i = 0; i < 128; i++) {
			keys[i] = GetKeyState(i) / 10;
		};

		if (keys[27]) {	// esc
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		};


		if (keys[65]) {// A
			function.offsetX -= 10;
		};

		if (keys[68]) {// D
			function.offsetX += 10;
		};

		if (keys[87]) {// W
			function.offsetY += 10;
		};

		if (keys[83]) {// S
			function.offsetY -= 10;
		};

		if (keys[80] && hotkey_run >= 20) {
			sim_running = !sim_running;
			hotkey_run = 0;
		};

		if (hotkey_run < 20) {
			hotkey_run++;
		};

		// 73 , 79
		if (keys[73]) {	// I
			function.scaleX -= function.scaleX / 25.0;
			function.scaleY -= function.scaleY / 25.0;
		};

		if (keys[79]) {
			function.scaleX += function.scaleX / 25.0;
			function.scaleY += function.scaleY / 25.0;
		};

		if (function.scaleX < .001) {
			function.scaleX = .001;
		};

		if (function.scaleY < .001) {
			function.scaleY = .001;
		};

		if (GetAsyncKeyState(VK_LEFT)) {
			obj1.v -= 0.1;
		};

		if (GetAsyncKeyState(VK_RIGHT)) {
			obj1.v += 0.1;
		};


		// calc obj1 physics
		if (sim_running) {
			obj1.x += obj1.v;
			obj1.v -= 0.005 * f_prime(obj1.x);
			obj1.v *= 0.99;
		};

		for (int i = 0; i < frame.width * frame.height; i++) {
			frame.pixels[i] = 0;
		};

		if (sim_running) {
			frame_DrawCircleFilled(&frame, 50, 50, 10, -1);
		};

		// draw f(x)
		int i = -function_quality;

		for (int x = function.offsetX; x < function.offsetX + frame.width; x += function_quality) {

			i += function_quality;

			double y = f(x * function.scaleX);
			double y_next = f((1.0 * x + function_quality) * function.scaleX);

			if (y < 0 || y > frame.height) {
				continue;
			};

			frame_DrawLine(&frame, i, y, i + function_quality, y_next, -1);
						
		};
		

		
		
		
		frame_DrawCircleFilled(&frame, (obj1.x / function.scaleX - function.offsetX) , f(obj1.x), 5, 0x00FF0000);

		
		frame_DrawLineA(&frame, (obj1.x / function.scaleX - function.offsetX), f(obj1.x), 10 * obj1.v, DeltasToDegrees(1, f_prime(obj1.x)), 0x00000FF00);


		char text[string_maxLength] = { 0 };

		text_AppendString(&text, "offsetX: ");
		text_AppendInt(&text, function.offsetX);
		frame_DrawText(&frame, 20, frame.height - 20, &text, 2, -1, -1);

		text_ClearString(&text);
		text_AppendString(&text, "offsetY: ");
		text_AppendInt(&text, function.offsetY);
		frame_DrawText(&frame, 20, frame.height - 35, &text, 2, -1, -1);

		text_ClearString(&text);
		text_AppendString(&text, "scaleX: ");
		text_AppendDouble(&text, function.scaleX, 3);
		frame_DrawText(&frame, 20, frame.height - 50, &text, 2, -1, -1);

		text_ClearString(&text);
		text_AppendString(&text, "scaleY: ");
		text_AppendDouble(&text, function.scaleY, 3);
		frame_DrawText(&frame, 20, frame.height - 65, &text, 2, -1, -1);

		text_ClearString(&text);
		text_AppendString(&text, "Currently drawing x = [");
		text_AppendInt(&text, function.offsetX * function.scaleX);
		text_AppendString(&text, ",");
		text_AppendInt(&text, (function.offsetX + frame.width) * function.scaleX);
		text_AppendString(&text, "]");
		frame_DrawText(&frame, 20, frame.height - 80, &text, 2, -1, -1);

		text_ClearString(&text);
		text_AppendString(&text, "obj pos: ");
		text_AppendDouble(&text, obj1.x, 3);
		text_AppendString(&text, ",");
		text_AppendDouble(&text, f(obj1.x), 3);
		frame_DrawText(&frame, 20, frame.height - 95, &text, 2, -1, -1);

		text_ClearString(&text);
		text_AppendString(&text, "ang: ");
		text_AppendDouble(&text, f_prime(obj1.x), 3);
		frame_DrawText(&frame, 20, frame.height - 110, &text, 2, -1, -1);

		text_ClearString(&text);
		text_AppendString(&text, "vel:");
		text_AppendDouble(&text, obj1.v, 3);
		frame_DrawText(&frame, 20, frame.height - 125, &text, 2, -1, -1);


		InvalidateRect(hWnd, NULL, 0);
		UpdateWindow(hWnd);

		Sleep(20);


	};


	return 0;

}




LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


	switch (uMsg) {

	case (WM_CLOSE): {
		DestroyWindow(hWnd);
		break;
	};

	case (WM_DESTROY): {
		PostQuitMessage(0);
		running = 0;
		break;
	};

	case (WM_SIZE): {

		frame_bitmapInfo.bmiHeader.biWidth = LOWORD(lParam);
		frame_bitmapInfo.bmiHeader.biHeight = HIWORD(lParam);

		if (frame_hBitmap) {
			DeleteObject(frame_hBitmap);
		};

		frame_hBitmap = CreateDIBSection(NULL, &frame_bitmapInfo, DIB_RGB_COLORS, &frame.pixels, 0, 0);

		SelectObject(frame_dc, frame_hBitmap);


		frame.width = LOWORD(lParam);
		frame.height = HIWORD(lParam);


	};

	case (WM_PAINT): {

		static PAINTSTRUCT ps;
		static HDC hdc;
		hdc = BeginPaint(hWnd, &ps);

		BitBlt(
			hdc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			ps.rcPaint.right - ps.rcPaint.left,
			ps.rcPaint.bottom - ps.rcPaint.top,
			frame_dc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			SRCCOPY
		);


		EndPaint(hWnd, &ps);


	};


	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	};


};