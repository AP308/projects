

#include <Windows.h>
#include "Drawing.h"	//	includes Util.h and string_maxLength
#include <Math.h>
#include "Util.h"

HBITMAP hBitmap;
BITMAPINFO bitmapInfo;
HDC frame_hdc;
struct frameStruct frame;

int keys[128] = { 0 };
POINT mouse;
int mouseL = 0;
int mouseR = 0;

int running = 1;


#define tower_maxLen 20
float towers[tower_maxLen][12] = { { 0 } };
enum {
	TWR_TYPE,
	TWR_STATE,
	TWR_X,
	TWR_Y,
	TWR_DIR,
	TWR_CODE,
	TWR_ARM1,
	TWR_ARM2,
	TWR_ARM3,
	TWR_COOLDOWN,
	TWR_COUNTER,
	TWR_NUM
};

enum {
	STATE_IDLE = 1,
	STATE_MOVING,
	STATE_SENDING
};

enum {
	TYPE_NORMAL = 0,
	TYPE_SENDER,
	TYPE_RECEIVER
};

int codeBook[37][3] = { 
{ 90, 270, 90 },		// NULL
{ 0, 90, 90 },		// A
{ 135, 45, 45 },	// B
{ 90, 0, 0 },		// C
{ 225, 315, 315 },	// D
{ 0, 270, 270 },	// E
{ 135, 225, 225 },	// F
{ 90, 180, 180 },	// G
{ 225, 135, 135 },	// H
{ 0, 90, 270 },		// I
{ 135, 315, 45 },	// J
{ 135, 225, 45 },	// K
{ 90, 180, 0 },		// L
{ 225, 315, 135 },	// M
{ 0, 270, 90 },		// N
{ 135, 45, 225 },	// O
{ 90, 0, 180 },		// P
{ 225, 135, 315 },	// Q
{ 0, 180, 90 },		// R
{ 135, 45, 135 },	// S
{ 90, 0, 90 },		// T
{ 225, 45, 315 },	// U
{ 0, 270, 0 },		// V
{ 135, 315, 225 },	// W
{ 90, 270, 180 },	// X
{ 225, 135, 225 },	// Y
{ 0, 90, 0 },		// Z
{ 90, 270, 0 },		// 1
{ 225, 315, 225 },	// 2
{ 0, 180, 270 },	// 3
{ 135, 225, 135 },	// 4
{ 90, 180, 90 },	// 5
{ 225, 45, 135 },	// 6
{ 0, 180, 0 },		// 7
{ 135, 315, 135 },	// 8
{ 90, 270, 90 },	// 9
{ 225, 45, 225 }	// 0
};




#define tower_selectionRadius 15
#define tower_range 50
#define tower_rangeAngle 45



#define tower_armTime 10
#define towerCooldown (tower_armTime * 3)

#define senderText_maxLen 6
char senderText[senderText_maxLen][string_maxLength] = {
	"pls send 2 muffins ",
	"hows the weather ",
	"can i get some shrimp prices ",
	"joes cart service open 24 7 ",
	"pls send food ",
	"grandma sends her love "
};

char receiverText[string_maxLength] = { 0 };




#define bgColor 0x66aa66

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	
	WNDCLASS wc = { };
	wc.lpszClassName = L"Class Name";
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;

	RegisterClass(&wc);

	RECT rect = { };
	rect.left = 500;
	rect.top = 200;
	rect.right = 1000;
	rect.bottom = 700;


	HWND hWnd = CreateWindowEx(
		0,
		L"Class Name",
		L"Semaphore Sim",
		WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		hInstance,
		NULL		
	);


	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biPlanes = 1;
	frame_hdc = CreateCompatibleDC(frame_hdc);

	ShowWindow(hWnd, SW_SHOW);

	RECT wRect;	// window rect
	RECT cRect;	// client rect
	GetWindowRect(hWnd, &wRect);
	GetClientRect(hWnd, &cRect);

	towers[0][TWR_TYPE] = TYPE_SENDER;
	towers[0][TWR_STATE] = STATE_SENDING;
	towers[0][TWR_X] = 180;
	towers[0][TWR_Y] = 230;
	towers[0][TWR_ARM1] = 90;
	towers[0][TWR_ARM2] = 270;
	towers[0][TWR_ARM3] = 90;
	towers[0][TWR_NUM] = rand() % senderText_maxLen;

	towers[1][TWR_TYPE] = TYPE_RECEIVER;
	towers[1][TWR_STATE] = STATE_IDLE;
	towers[1][TWR_X] = 290;
	towers[1][TWR_Y] = 250;
	towers[1][TWR_DIR] = 0;
	towers[1][TWR_ARM1] = 90;
	towers[1][TWR_ARM2] = 270;
	towers[1][TWR_ARM3] = 90;

	towers[2][TWR_STATE] = STATE_IDLE;
	towers[2][TWR_X] = 220;
	towers[2][TWR_Y] = 230;
	towers[2][TWR_DIR] = 0;
	towers[2][TWR_ARM1] = 90;
	towers[2][TWR_ARM2] = 270;
	towers[2][TWR_ARM3] = 90;

	towers[3][TWR_STATE] = STATE_IDLE;
	towers[3][TWR_X] = 250;
	towers[3][TWR_Y] = 240;
	towers[3][TWR_DIR] = 0;
	towers[3][TWR_ARM1] = 90;
	towers[3][TWR_ARM2] = 270;
	towers[3][TWR_ARM3] = 90;

	int iTowerMoving = tower_maxLen - 1;
	int moving = 0;

	int towerSelected = TYPE_NORMAL;



	int maxChars = frame.width / 13;

	if (maxChars > 100) {
		maxChars = 100;
	};

	for (int i = 0; i < maxChars; i++) {
		receiverText[i] = '_';
	};

	MSG msg;
	while (running) {

		PeekMessage(&msg, hWnd, 0u, 0u, PM_REMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		for (int i = 0; i < 128; i++) {
			keys[i] = HIWORD(GetKeyState(i));
		};

		if (keys[27]) {
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
		};

		if (keys['1']) {
			towerSelected = TYPE_NORMAL;
		};

		if (keys['2']) {
			towerSelected = TYPE_SENDER;
		};

		if (keys['3']) {
			towerSelected = TYPE_RECEIVER;
		};

		for (int i = 0; i < frame.width * frame.height; i++) {
			frame.pixels[i] = bgColor;
		};


		GetWindowRect(hWnd, &wRect);
		GetClientRect(hWnd, &cRect);
		GetCursorPos(&mouse);

		mouseL = HIWORD(GetAsyncKeyState(VK_LBUTTON));
		mouseR = HIWORD(GetAsyncKeyState(VK_RBUTTON));


		char text[string_maxLength] = { 0 };

		int mouseX = mouse.x - wRect.left;
		int mouseY = wRect.bottom - mouse.y;

		text_ClearString(text);
		text_AppendString(text, "mousePos: ");
		text_AppendInt(text, mouseX);
		text_AppendString(text, ", ");
		text_AppendInt(text, mouseY);
		frame_DrawText(&frame, 10, frame.height - 20, text, 2, 2, 0xffffff, 0);


		int iEnd = 0;
		for (int t = 0; t < tower_maxLen; t++) {
			if (towers[t][TWR_STATE] == 0) {
				iEnd = t;
				break;
			};
		};

		int iTowerHot = -1;
		double smallestDist = 10000;
		for (int t = 0; t < tower_maxLen; t++) {

			if (towers[t][TWR_STATE] == 0) {
				continue;
			};

			double dist = sqrt((towers[t][TWR_X] - mouseX) * (towers[t][TWR_X] - mouseX) + (towers[t][TWR_Y] - mouseY) * (towers[t][TWR_Y] - mouseY));

			if (dist > tower_selectionRadius) {
				continue;
			};

			if (dist < smallestDist) {
				iTowerHot = t;
				smallestDist = dist;
			};

		};

		
		if (HIWORD(GetAsyncKeyState(VK_LBUTTON)) > 0 && (iTowerHot < 0) && !moving) {	// create new tower & initiate move on it
			iTowerHot = iEnd;
			towers[iTowerHot][TWR_TYPE] = towerSelected;
		};

		if (HIWORD(GetAsyncKeyState(VK_LBUTTON)) > 0 && (iTowerHot >= 0) && !moving) {	// initiate move on existing tower
			iTowerMoving = iTowerHot;
			towers[iTowerMoving][TWR_STATE] = STATE_MOVING;
			towers[iTowerMoving][TWR_ARM1] = 90;
			towers[iTowerMoving][TWR_ARM2] = 270;
			towers[iTowerMoving][TWR_ARM3] = 90;
			moving = 1;
		};

		if (moving) {	// moving tower
			towers[iTowerMoving][TWR_X] = mouseX;
			towers[iTowerMoving][TWR_Y] = mouseY;
		};

		if (moving && keys['Q']) {
			towers[iTowerHot][TWR_DIR] += 8;
		};

		if (moving && keys['E']) {
			towers[iTowerHot][TWR_DIR] -= 8;
		};

		if (HIWORD(GetAsyncKeyState(VK_LBUTTON)) == 0 && moving) {	// finish placing tower
			towers[iTowerMoving][TWR_STATE] = STATE_SENDING;
			moving = 0;
			iTowerMoving = -1;
		};

		towers[iTowerMoving][TWR_DIR] = (int)(towers[iTowerMoving][TWR_DIR]) % 360;

		if (moving) {
			text_ClearString(text);
			text_AppendString(text, "towerInfo: ");
			text_AppendInt(text, towers[iTowerMoving][TWR_X]);
			text_AppendString(text, ", ");
			text_AppendInt(text, towers[iTowerMoving][TWR_Y]);
			text_AppendString(text, " - ");
			text_AppendInt(text, towers[iTowerMoving][TWR_DIR]);
			frame_DrawText(&frame, 10, frame.height - 60, text, 2, 2, 0xffffff, 0);
		};
		

		if (mouseR && iTowerHot >= 0) {		// delete towers when in mouse range
			towers[iTowerHot][TWR_STATE] = 0;
			towers[iTowerHot][TWR_X] = 0;
			towers[iTowerHot][TWR_Y] = 0;
			towers[iTowerHot][TWR_DIR] = 0;
			moving = 0;
		};


		for (int t = 0; t < tower_maxLen; t++) {	// generate sender messages

			if (towers[t][TWR_TYPE] != TYPE_SENDER) {
				continue;
			};

			if (towers[t][TWR_STATE] != STATE_IDLE) {
				continue;
			};

			towers[t][TWR_COOLDOWN]--;
			if (towers[t][TWR_COOLDOWN] > 0) {
				continue;
			};

			int iMessage = (int)towers[t][TWR_NUM];

			int textLen = 0;
			for (int i = 0; i < string_maxLength; i++) {

				if (senderText[iMessage][i] == 0) {
					textLen = i + 1;
					break;
				};
				
			};
			
			
			int iLetter = (int)(towers[t][TWR_COUNTER]);

			if (iLetter >= textLen) {
				iMessage = rand() % senderText_maxLen;
				towers[t][TWR_NUM] = iMessage;
				iLetter = 0;
				towers[t][TWR_COUNTER] = 0;
			};

			int letter = senderText[iMessage][iLetter];

			int code = 0;

			if (letter >= 'a' && letter <= 'z') {
				code = letter - 'a' + 1;
			};
			if (letter >= '1' && letter <= '9') {
				code = letter - '1' + 27;
			};
			if (letter == '0') {
				code = 36;
			};


			towers[t][TWR_CODE] = code;
			towers[t][TWR_COUNTER]++;

			towers[t][TWR_STATE] = STATE_SENDING;
			towers[t][TWR_COOLDOWN] = towerCooldown;

		};

		
		for (int t = 0; t < tower_maxLen; t++) {	// detect other tower(s)

			if (t == iTowerMoving) {
				continue;
			};

			if (towers[t][TWR_TYPE] == TYPE_RECEIVER) {	// receiver towers don't send messages
				continue;
			};

			if (towers[t][TWR_STATE] != STATE_IDLE) {	// 1st tower must be idle
				continue;
			};

			for (int u = 0; u < tower_maxLen; u++) {

				if (towers[u][TWR_TYPE] == TYPE_SENDER) {	// sender towers don't receive messages
					continue;
				};

				if (towers[t][TWR_CODE] == towers[u][TWR_CODE]) {
					continue;
				};

				if (towers[u][TWR_STATE] == 0) {
					continue;
				};
				
				if (t == u) {
					continue;
				};
				
				if (u == iTowerMoving) {
					continue;
				};

				float dist = sqrt(
					(towers[t][TWR_X] - towers[u][TWR_X]) * (towers[t][TWR_X] - towers[u][TWR_X])
					+ (towers[t][TWR_Y] - towers[u][TWR_Y]) * (towers[t][TWR_Y] - towers[u][TWR_Y])
				);

				if (dist > tower_range) {	// both towers must be in range
					continue;
				};

				int angle1 = towers[t][TWR_DIR] + (tower_rangeAngle / 2);	// tower must be within range Angle
				int angle2 = towers[t][TWR_DIR] - (tower_rangeAngle / 2);

				if ((angle1 > 360 && angle2 > 360) || (angle1 < 0 && angle2 < 0)) {
					angle1 = NormalizeAngle(angle1);
					angle2 = NormalizeAngle(angle2);
				};

				int angle = DeltasToDegrees(towers[u][TWR_X] - towers[t][TWR_X], towers[u][TWR_Y] - towers[t][TWR_Y]);

				if (angle1 < 0 == angle2 < 0) {
					angle = NormalizeAngle(angle);
				};

				if (angle < angle1 == angle < angle2) {
					continue;
				};


				towers[u][TWR_CODE] = towers[t][TWR_CODE];
				towers[u][TWR_STATE] = STATE_SENDING;

				if (towers[u][TWR_TYPE] != TYPE_RECEIVER) {	// receiver gets messages
					continue;
				};				


				int code = towers[t][TWR_CODE];

				char letter = '_';

				if (code > 0 && code <= 26) {
					letter = code + 'a' - 1;
				};
				if (code >= 27 && code <= 35) {
					letter = code + '1' - 27;
				};
				if (code == 36) {
					letter = '0';
				};
				

				text_AppendChar(receiverText, letter);

				towers[u][TWR_COOLDOWN] = 100;


			};


		};


		for (int t = 0; t < tower_maxLen; t++) {	// animate tower arms

			if (towers[t][TWR_STATE] != STATE_IDLE && towers[t][TWR_STATE] != STATE_SENDING) {
				continue;
			};

			int code = towers[t][TWR_CODE];


			int arm1_goal = codeBook[code][0];
			int arm2_goal = codeBook[code][1];
			int arm3_goal = codeBook[code][2];

			towers[t][TWR_ARM1] += (1.0 * arm1_goal - towers[t][TWR_ARM1]) / tower_armTime;

			if (num_CompareTolerance(1.0 * towers[t][TWR_ARM1], 1.0 * arm1_goal, 5.0)) {
				towers[t][TWR_ARM1] = arm1_goal;
			};

			towers[t][TWR_ARM2] += (1.0 * arm2_goal - towers[t][TWR_ARM2]) / tower_armTime;

			if (num_CompareTolerance(1.0 * towers[t][TWR_ARM2], 1.0 * arm2_goal, 5.0)) {
				towers[t][TWR_ARM2] = arm2_goal;
			};

			towers[t][TWR_ARM3] += (1.0 * arm3_goal - towers[t][TWR_ARM3]) / tower_armTime;

			if (num_CompareTolerance(1.0 * towers[t][TWR_ARM3], 1.0 * arm3_goal, 5.0)) {
				towers[t][TWR_ARM3] = arm3_goal;
			};


			towers[t][TWR_STATE] = STATE_SENDING;

			if (towers[t][TWR_ARM1] != arm1_goal) {
				continue;
			};

			if (towers[t][TWR_ARM2] != arm2_goal) {
				continue;
			};

			if (towers[t][TWR_ARM3] != arm3_goal) {
				continue;
			};

			towers[t][TWR_STATE] = STATE_IDLE;


		};



		for (int t = 0; t < tower_maxLen; t++) {	// draw towers

			if (towers[t][TWR_STATE] == 0) {
				continue;
			};

			int width = 15;
			int height = 12;


			if (moving) {
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y], 10, 0xff0000, .6);
			}
			
			if (moving && towers[t][TWR_TYPE] != TYPE_RECEIVER) {
				frame_DrawSemiCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y], tower_range, tower_rangeAngle, towers[t][TWR_DIR], 0x004400, .8);	// draw range
				frame_DrawSemiCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y], tower_range * .75, tower_rangeAngle, towers[t][TWR_DIR], 0x004400, .9);
			};

			if (t == iTowerHot) {
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y], 10, 0xaa0000, .5);
			};

			int arm1_radius = 15;

			if (towers[t][TWR_TYPE] == TYPE_SENDER) {	// draw sender
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y] - 20, 10, 0x111111, 0);
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y] - 15, 10, 0x222222, 0);
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y] - 15, 6, 0x111111, 0);
				frame_DrawRectFilled(&frame, towers[t][TWR_X] - 6, towers[t][TWR_Y] - 2, towers[t][TWR_X] + 6, towers[t][TWR_Y] - 13, 0x111111, 0);
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y], 6, 0x222222, 0);
			};

			int code = towers[t][TWR_CODE];
			char letter[2] = { 0 };
			letter[0] = ' ';
			if (code > 0 && code <= 26) {
				letter[0] = code + 'a' - 1;
			};
			if (code >= 27 && code <= 35) {
				letter[0] = code + '1' - 27;
			};
			if (code == 36) {
				letter[0] = '0';
			};

			if (towers[t][TWR_TYPE] == TYPE_RECEIVER) {	// draw receiver

				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y] - 20, 10, 0x111111, 0);
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y] - 15, 10, 0x222222, 0);

				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y] - 15, 6, 0x111111, 0);
				frame_DrawRectFilled(&frame, towers[t][TWR_X] - 6, towers[t][TWR_Y] - 7, towers[t][TWR_X] + 6, towers[t][TWR_Y] - 13, 0x111111, 0);
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y] - 5, 6, 0x222222, 0);

				
				float opacity = 1.0 - (towers[t][TWR_COOLDOWN] / 100.0);
				
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y] + 5, 7, 0xcc0000, opacity);
				frame_DrawRectFilled(&frame, towers[t][TWR_X] - 7, towers[t][TWR_Y] + 5, towers[t][TWR_X] + 7, towers[t][TWR_Y] + 18, 0xcc0000, opacity);
				frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y] + 20, 7, 0xff0000, opacity);

				frame_DrawText(&frame, towers[t][TWR_X] - 6, towers[t][TWR_Y] + 15, letter, 2, 2, 0xffffff, opacity * 2 - .01);


				towers[t][TWR_COOLDOWN] -= 2;

				if ((towers[t][TWR_COOLDOWN] < 50)) {
					towers[t][TWR_COOLDOWN] = 50;
				};

				continue;
			};


			int armColor = 0;

			if (towers[t][TWR_STATE] != STATE_IDLE) {
				armColor = 0x333333;
			};

			frame_DrawCircleFilled(&frame, towers[t][TWR_X], towers[t][TWR_Y], 3, 0x333333, 0);	// draw tower base
			frame_DrawRectFilled(&frame, towers[t][TWR_X] - 1, towers[t][TWR_Y], towers[t][TWR_X] + 1, towers[t][TWR_Y] + 30, 0x333333, 0);

			frame_DrawLineA(&frame, towers[t][TWR_X], towers[t][TWR_Y] + 30, arm1_radius, towers[t][TWR_ARM1], 0);	// draw arm 1
			frame_DrawLineA(&frame, towers[t][TWR_X], towers[t][TWR_Y] + 30, arm1_radius, towers[t][TWR_ARM1] + 180, armColor);

			int deltaX = arm1_radius * cos(towers[t][TWR_ARM1] * 3.14 / 180);
			int deltaY = arm1_radius * sin(towers[t][TWR_ARM1] * 3.14 / 180);

			int arm23_radius = 10;

			frame_DrawRectAFilled(&frame, towers[t][TWR_X] + deltaX, towers[t][TWR_Y] + 30 + deltaY, 0, -2, towers[t][TWR_ARM2] + 90, arm1_radius, 4, armColor);	// draw arm 2
			frame_DrawRectAFilled(&frame, towers[t][TWR_X] - deltaX, towers[t][TWR_Y] + 30 - deltaY, 0, -2, towers[t][TWR_ARM3] + 90, arm1_radius, 4, armColor);	// draw arm 3
		
			

			if (towers[t][TWR_TYPE] != TYPE_SENDER) {
				//continue;
			};

			if (towers[t][TWR_STATE] != STATE_IDLE) {
				continue;
			};

			if (code <= 0) {
				continue;
			};

			frame_DrawText(&frame, towers[t][TWR_X] - 6, towers[t][TWR_Y] - 8, letter, 2, 2, 0xffffff, 0);

		};


		frame_DrawCircleFilled(&frame, mouseX, mouseY, 3,
			(mouseL > 0) * (0xffffff)
			+ (mouseR > 0) * (0xff0000)
			+ (mouseL + mouseR == 0) * (0x555555),
			0
		);


		int textLen = text_GetSize(receiverText);
		if (textLen > maxChars) {
			text_Shift(receiverText, 0, -1 * abs(textLen - maxChars));
		};
		frame_DrawText(&frame, 10, 20, receiverText, 2, 2, 0xffffff, 0);


		text_ClearString(text);
		text_AppendString(text, "Selected: ");

		switch (towerSelected) {

		case (TYPE_NORMAL): {
			text_AppendString(text, "Normal (1)");
			break;
		};

		case (TYPE_SENDER): {
			text_AppendString(text, "Sender (2)");
			break;
		};

		case (TYPE_RECEIVER): {
			text_AppendString(text, "Receiver (3)");
			break;
		};

		};

		frame_DrawText(&frame, 10, frame.height - 40, text, 2, 2, 0xffffff, 0);


		InvalidateRect(hWnd, NULL, 1);
		UpdateWindow(hWnd);
		

		Sleep(20);

	};


	return 0;
};


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


	switch (uMsg) {

	case (WM_QUIT): {
		SendMessage(hWnd, WM_DESTROY, NULL, NULL);
		break;
	}

	case (WM_DESTROY): {		
		DestroyWindow(hWnd);
		running = 0;
		break;
	}

	case (WM_SIZE): {

		bitmapInfo.bmiHeader.biWidth = LOWORD(lParam);
		bitmapInfo.bmiHeader.biHeight = HIWORD(lParam);

		hBitmap = CreateDIBSection(NULL, &bitmapInfo, DIB_RGB_COLORS, &frame.pixels, 0, 0);

		SelectObject(frame_hdc, hBitmap);

		frame.width = LOWORD(lParam);
		frame.height = HIWORD(lParam);

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

	case (WM_SETCURSOR): {
		
		if (LOWORD(lParam) == HTCLIENT) {	// hide mouse when over client area
			SetCursor(NULL);
		};
	};

	default: {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	};

	};

	return 0;	
};
