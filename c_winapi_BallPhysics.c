
#include <Windows.h>
#include "Util.h"
#include "Drawing.h"

int running = 1;
int keys[128] = { 0 };

HBITMAP frame_hBitmap;
BITMAPINFO frame_bitmapInfo;
HDC frame_hdc;

struct frameStruct frame = { 0 };

struct {
	int x;
	int y;
	int velX;
	int velY;
} ball = { 0 };


#define gravity_strength 1

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASS wc = { };
	wc.lpszClassName = CLASS_NAME;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;

	RegisterClass(&wc);
	
	//DWORD style = WS_POPUPWINDOW;

	RECT rect;
	rect.left = 300;
	rect.top = 200;
	rect.right = 700;
	rect.bottom = 600;

	HWND hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Ball Physics",
		WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_BORDER,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	frame_bitmapInfo.bmiHeader.biSize = sizeof(frame_bitmapInfo.bmiHeader);
	frame_bitmapInfo.bmiHeader.biPlanes = 1;
	frame_bitmapInfo.bmiHeader.biBitCount = 32;
	frame_bitmapInfo.bmiHeader.biCompression = BI_RGB;
	frame_hdc = CreateCompatibleDC(frame_hdc);

	ShowWindow(hWnd, SW_SHOW);

	ball.velX = 0;
	ball.velY = 0;

	ball.x = 600;
	ball.y = 500;

	int mouse_cX = 0;
	int mouse_cY = 0;

	int ballRadius = 15;
	int s_debugMode = 0;
	int s_gravFlip = 0;

	int settingsCooldown = 0;


	RECT windowPos;
	GetWindowRect(hWnd, &windowPos); // pos of window
	
	RECT canvasPos;	// drawing area (ball stays in this area)
	GetWindowRect(hWnd, &canvasPos);

	POINT mousePos;
	GetCursorPos(&mousePos);
	POINT mousePos2;
	GetCursorPos(&mousePos2);

	MSG msg;
	while (running) {

		PeekMessage(&msg, hWnd, 0u, 0u, PM_REMOVE);

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		for (int i = 0; i < 128; i++) {
			keys[i] = HIWORD(GetKeyState(i));
		};

		if (keys[27]) {
			SendMessage(hWnd, WM_QUIT, NULL, NULL);
		};

		settingsCooldown++;

		if (keys[68] && settingsCooldown >= 10) {	// D
			s_debugMode = !s_debugMode;
			settingsCooldown = 0;
		};

		if (keys[70] && settingsCooldown >= 10) {	// F
			s_gravFlip = !s_gravFlip;
			settingsCooldown = 0;
		};
		
		if (keys[73] && settingsCooldown >= 10) {	// I
			ballRadius -= 2;
			settingsCooldown = 0;
		};

		if (keys[79] && settingsCooldown >= 10) {	// O
			ballRadius += 2;
			settingsCooldown = 0;
		};

		GetCursorPos(&mousePos);
		mouse_cX = -1 * (mousePos2.x - mousePos.x);
		mouse_cY = mousePos2.y - mousePos.y;
		mousePos2 = mousePos;

		GetWindowRect(hWnd, &windowPos);
		canvasPos = windowPos;
		
		canvasPos.left += 7;
		canvasPos.top += 30;
		canvasPos.right -= 10;
		canvasPos.bottom -= 10;

		int movingWindow = 0;

		if (GetAsyncKeyState(VK_LBUTTON) && (mousePos.y > windowPos.top + 30)) {
			movingWindow = 1;
		};

		if (movingWindow) {
			MoveWindow(hWnd, (windowPos.left + mouse_cX), (windowPos.top - mouse_cY), (windowPos.right - windowPos.left), (windowPos.bottom - windowPos.top), TRUE);
		};
		

		int collided = 0;

		if (ball.x <= canvasPos.left + ballRadius) {
			ball.velX *= -1;
			ball.x = canvasPos.left + ballRadius;
			collided++;
		};

		if (ball.x >= canvasPos.right - ballRadius) {
			ball.velX *= -1;
			ball.x = canvasPos.right - ballRadius;
			collided++;
		};

		if (ball.y <= canvasPos.top + ballRadius) {
			ball.velY *= -1;
			ball.y = canvasPos.top + ballRadius;
			collided++;
		};

		if (ball.y >= canvasPos.bottom - ballRadius) {
			ball.velY *= -1;
			ball.y = canvasPos.bottom - ballRadius;
			collided++;
		};


		if (collided && movingWindow) {	// boost speed when hits moving window
			ball.velX += mouse_cX;
			ball.velY += mouse_cY;
		};

		if (collided) {	// slowdown when hitting wall
			ball.velX *= .9;
			ball.velY *= .9;
		};
		
		if (!s_gravFlip) {
			ball.velY -= gravity_strength;	// gravity
		};

		if (s_gravFlip) {
			ball.velX -= gravity_strength;	// gravity
		};

		ball.x += ball.velX;
		ball.y -= ball.velY;


		for (int i = 0; i < frame.width * frame.height; i++) {
			frame.pixels[i] = 0;
		};


		frame_DrawCircleFilled(&frame, ball.x - windowPos.left - 7, - ball.y + windowPos.bottom - 10, ballRadius, 0x00FF0000);


		frame_DrawText(&frame, 10, frame.height - 10, "Debug Mode: D", 1, -1, -1);

		if (!s_debugMode) {
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			Sleep(20);
			continue;
		};
		
		char text[string_maxLength] = { 0 };
		
		if (s_gravFlip) {
			frame_DrawText(&frame, 10, frame.height - 20, "GravityFlip: F", 1, -1, 0x0000FF00);
		};
		if (!s_gravFlip) {
			frame_DrawText(&frame, 10, frame.height - 20, "GravityFlip: F", 1, -1, 0x00FF0000);
		};
			
		// window data
		text_ClearString(&text);
		text_AppendString(&text, "winPos: ");
		text_AppendInt(&text, windowPos.left);
		text_AppendString(&text, ", ");
		text_AppendInt(&text, windowPos.top);
		text_AppendString(&text, " - ");
		text_AppendInt(&text, windowPos.right);
		text_AppendString(&text, ", ");
		text_AppendInt(&text, windowPos.bottom);
		frame_DrawText(&frame, 10, frame.height - 40, &text, 2, 2, -1);

		text_ClearString(&text);
		text_AppendString(&text, "canPos: ");
		text_AppendInt(&text, canvasPos.left);
		text_AppendString(&text, ", ");
		text_AppendInt(&text, canvasPos.top);
		text_AppendString(&text, " - ");
		text_AppendInt(&text, canvasPos.right);
		text_AppendString(&text, ", ");
		text_AppendInt(&text, canvasPos.bottom);
		frame_DrawText(&frame, 10, frame.height - 60, &text, 2, 2, -1);

		// mouse data
		text_ClearString(&text);
		text_AppendString(&text, "mousePos: ");
		text_AppendInt(&text, mousePos.x);
		text_AppendString(&text, " , ");
		text_AppendInt(&text, mousePos.y);
		frame_DrawText(&frame, 10, frame.height - 80, &text, 2, 2, -1);

		text_ClearString(&text);
		text_AppendString(&text, "mouseC: ");
		text_AppendInt(&text, mouse_cX);
		text_AppendString(&text, " , ");
		text_AppendInt(&text, mouse_cY);
		frame_DrawText(&frame, 10, frame.height -100, &text, 2, 2, -1);
		
		// ball data
		float totalVel = ballRadius * sqrt((ball.velX * ball.velX) + (ball.velY * ball.velY));
		text_ClearString(&text);
		text_AppendString(&text, "vel: ");
		text_AppendInt(&text, totalVel);
		frame_DrawText(&frame, 10, 40, &text, 2, 2, -1);

		text_ClearString(&text);
		text_AppendString(&text, "pos: ");
		text_AppendInt(&text, ball.x);
		text_AppendString(&text, " , ");
		text_AppendInt(&text, ball.y);
		frame_DrawText(&frame, 10, 20, &text, 2, 2, -1);



		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);

		Sleep(20);

	};

	return 0;

}



LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {

	case (WM_QUIT): {
		SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		PostQuitMessage(0);
		break;
	}

	case (WM_CLOSE): {
		DestroyWindow(hWnd);
		running = 0;
		break;
	}

	case (WM_SIZE): {

		frame_bitmapInfo.bmiHeader.biWidth = LOWORD(lParam);
		frame_bitmapInfo.bmiHeader.biHeight = HIWORD(lParam);

		frame_hBitmap = CreateDIBSection(NULL, &frame_bitmapInfo, DIB_RGB_COLORS, &frame.pixels, 0, 0);

		SelectObject(frame_hdc, frame_hBitmap);

		frame.width = LOWORD(lParam);
		frame.height = HIWORD(lParam);

		break;

	}

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

	}

	default: {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	};

	return 0;

};
