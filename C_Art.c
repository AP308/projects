
#include <stdio.h>
#include <Windows.h>

#include "Drawing.h"
#include "Util.h"



int running = 1;

int keys[128] = { 0 };

static BITMAPINFO frame_bitmapInfo;
static HBITMAP frame_hBitmap = 0;
static HDC frame_hdc = 0;

struct frameStruct frame = { 0 };


#define canvas_length 1000

int arr[canvas_length] = { 0 };

struct {
	int rollLength;
	int rows;
	int* data;
} canvas = { -1, -1, &arr };

struct {
	double segLength;
	int segUniformLengths;
} aiData = { 1 };


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	const wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASS wc = { };
	wc.lpszClassName = CLASS_NAME;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;


	RegisterClass(&wc);

	frame_bitmapInfo.bmiHeader.biSize = sizeof(frame_bitmapInfo.bmiHeader);
	frame_bitmapInfo.bmiHeader.biPlanes = 1;
	frame_bitmapInfo.bmiHeader.biBitCount = 32;
	frame_bitmapInfo.bmiHeader.biCompression = BI_RGB;
	frame_hdc = CreateCompatibleDC(frame_hdc);


	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
	
	RECT rect;
	rect.left = 300;
	rect.top = 150;
	rect.right = 850;
	rect.bottom = 650;

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

	ShowWindow(hWnd, SW_SHOW);



	int canvas_lineThickness = 8;
	int canvas_lineSpacing = 5;

	int canvas_width = frame.width - 100;
	int canvas_height = frame.height - 100;

	canvas.rows = canvas_height / (canvas_lineThickness + canvas_lineSpacing);
	canvas.rollLength = canvas_width * canvas.rows;

	
	int sum = 0;

	for (int i = 0; i < canvas_length; i++) {


		canvas.data[i] = rand() % 20 * 20;	// where the magic happens

		
		sum += canvas.data[i];

		if (sum > canvas.rollLength) {
			
			canvas.data[i] = canvas.rollLength - (sum - canvas.data[i]);

			break;
		};

	};



	double temp_segLength = 1;

	int temp_segUniformLengths = 1;


	#define cooldown1_timer 5
	int cooldown1 = cooldown1_timer;

	MSG msg = { };

	while (running) {

		PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE);

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		for (int i = 0; i < 128; i++) {
			keys[i] = GetKeyState(i) / 10;
		};

		for (int i = 0; i < frame.width * frame.height; i++) {
			frame.pixels[i] = 0;
		};


		if (keys[27]) {	// esc
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		};

		if (cooldown1 < cooldown1_timer) {
			cooldown1++;
		};

		int up = 0;
		if (GetAsyncKeyState(VK_UP) && cooldown1 >= cooldown1_timer) {
			cooldown1 = 0;
			up++;
		};

		int down = 0;
		if (GetAsyncKeyState(VK_DOWN) && cooldown1 >= cooldown1_timer) {
			cooldown1 = 0;
			down++;
		};


		if (up && !down) {

			// save values
			aiData.segLength = temp_segLength;
			aiData.segUniformLengths = temp_segUniformLengths;

		};


		if (up || down) {
			
			int rnd = rand();
			

			temp_segLength = aiData.segLength
				+ (0.1) * (rnd % 2 == 1)
				+ (-0.1) * (rnd % 2 == 0);
			

			temp_segUniformLengths = aiData.segUniformLengths 
				+ (1) * (rnd % 2 == 1)
				+ (-1) * (rnd % 2 == 0);


			
			for (int i = 0; i < canvas_length; i++) {
				canvas.data[i] = 0;
			};
			

			int sum = 0;

			for (int i = 0; i < canvas_length; i++) {

				canvas.data[i] = rand() % (20 + temp_segUniformLengths) * (20 * temp_segLength);	// generate new canvas

				
				sum += canvas.data[i];

				if (sum > canvas.rollLength) {

					canvas.data[i] = canvas.rollLength - (sum - canvas.data[i]);

					break;
				};
				

			};

		};


		

		
		


		
		frame_DrawRect(&frame, 50, 50, 50 + canvas_width, frame.height - 50, 0x00222222);
		

		// draw canvas
		int row = 0;
		int rect_x1 = 0;
		int rect_x2 = 0;
		int on = 0;
		int segmentLength = 0;

		for (int i = 0; i < canvas_length; i++) {

			if (canvas.data[i] == 0) {
				continue;
			};

			int color = -1;
			if (i % 2) {
				color = 0;//0x00FF0000;
			};

			segmentLength += canvas.data[i];

			while (segmentLength > canvas_width) {

				rect_x2 = canvas_width;
				int rect_y = row * (canvas_lineThickness + canvas_lineSpacing) + 50;

				frame_DrawRectFilled(&frame, 50 + rect_x1, frame.height - rect_y, 50 + rect_x2, frame.height - (rect_y + canvas_lineThickness), color);
				
				segmentLength -= canvas_width;
				rect_x2 = 0;
				row++;

			};


			rect_x1 = rect_x2;
			rect_x2 = segmentLength;
			
			int rect_y = row * (canvas_lineThickness + canvas_lineSpacing) + 50;

			frame_DrawRectFilled(&frame, 50 + rect_x1, frame.height - rect_y, 50 + rect_x2, frame.height - (rect_y + canvas_lineThickness), color);

			rect_x1 = segmentLength;

		};




		frame_DrawText(&frame, 50, 50, "Current:", 2, 2, -1);
		frame_DrawText(&frame, 200, 50, "Recommended:", 2, -1, -1);


		char text[string_maxLength] = { 0 };

		text_ClearString(&text);
		text_AppendString(&text, "segLength:");
		text_AppendDouble(&text, aiData.segLength);
		frame_DrawText(&frame, 50, 30, &text, 1, 2, -1);
		text_ClearString(&text);
		text_AppendDouble(&text, temp_segLength);
		frame_DrawText(&frame, 200, 30, &text, 1, 2, -1);

		text_ClearString(&text);
		text_AppendString(&text, "Uniformity:");
		text_AppendInt(&text, aiData.segUniformLengths);
		frame_DrawText(&frame, 50, 10, &text, 1, 2, -1);
		text_ClearString(&text);
		text_AppendInt(&text, temp_segUniformLengths);
		frame_DrawText(&frame, 200, 10, &text, 1, 2, -1);


		



		InvalidateRect(hWnd, NULL, 0);
		UpdateWindow(hWnd);

		Sleep(50);

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

		SelectObject(frame_hdc, frame_hBitmap);

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
			frame_hdc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			SRCCOPY
		);

		EndPaint(hWnd, &ps);

	};

	default: {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	};


	
	return 0;

};

