#include <stdio.h>
#include <Windows.h>
#include <Math.h>


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int running = 1;

struct {
	int width;
	int height;
	int* pixels;
} frame = { 0 };

static BITMAPINFO frame_mapInfo;
static HBITMAP frame_hBitmap = 0;
static HDC frame_dc = 0;

int keys[128] = { 0 };

struct {
	int x;
	int y;
	int left;
	int right;
	int in;
	int dx;
	int dy;
} mouse = { 0 };

struct {
	int x;
	int y;
} map_anchor = { 0 };
// to move the map

	// create map array
#define map_width 500
#define map_height 500

int map[map_width * map_height] = { 0 };

int map_mCubes[map_width * map_height] = { 0 };


int map_densities[map_width * map_height] = { 0 };


#define color_Darken(color, percent) (RGB(GetRValue(color) - (GetRValue(color) * percent / 100), GetGValue(color) - (GetGValue(color) * percent / 100), GetBValue(color) - (GetBValue(color) * percent / 100)))


#define pen_size 5

#define string_maxLength 100


int text_bitmap[128][25] = {

	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//space
	{0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },	//!
	{0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//"
	{0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0 },	//#
	{0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0 },	//$
	{0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, },	//%
	{0, 0, 1, 0, 0,	0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, },	//&
	{0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//'
	{0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, },	//(
	{0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, },	//)
	{0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, },	//*
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, },	//+
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },	//,
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },	//-
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },	//.
	{0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, },	//slash (/)

	{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 },	//0
	{0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0 },	//1
	{0, 1, 1, 0, 0,	0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, },	//2
	{0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0 },	//3
	{0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, },	//4
	{0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, },	//5
	{ 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0 },	//6
	{0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, },	//7
	{0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0 },	//8
	{0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0 },	//9

	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },	//:
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },	//;
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },	//<
	{0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0 },	//=
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },	//>
	{0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },	//?
	{0, 1, 1, 1, 0,	1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0 },	//@

	{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, },	//A
	{1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, },	//B
	{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, },	//C
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, },	//D
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, },	//E
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, },	//F
	{0, 1, 1, 1, 1,	1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//G	
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, },	//H
	{1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, },	//I
	{1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//J
	{1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, },	//K
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, },	//L
	{1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, },	//M
	{1, 0, 0, 0, 1,	1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, },	//N
	{0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//O
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, },	//P
	{0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, },	//Q
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, },	//R
	{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, },	//S
	{1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, },	//T
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//U
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, },	//V
	{1, 0, 0, 0, 1,	1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, },	//W
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, },	//X
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, },	//Y
	{1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, },	//Z

	{0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0 },	//[
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, },	//backslash (\)
	{0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0 },	//]
	{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//^
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 },	//_
	{0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//`

	{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, },	//a
	{1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, },	//b
	{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, },	//c
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, },	//d
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, },	//e
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, },	//f
	{0, 1, 1, 1, 1,	1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//g	
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, },	//h
	{1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, },	//i
	{1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//j
	{1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, },	//k
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, },	//l
	{1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, },	//m
	{1, 0, 0, 0, 1,	1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, },	//n
	{0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//o
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, },	//p
	{0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, },	//q
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, },	//r
	{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, },	//s
	{1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, },	//t
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//u
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 },	//v
	{1, 0, 0, 0, 1,	1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0 },	//w
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1 },	//x
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },	//y
	{1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1 },	//z

	{0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0 },	//{
	{0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },	//|
	{0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0 },	//}
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }	//~

};


void frame_DrawText(int x1, int y1, char text[], int text_xScale, int text_yScale, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	int letter_spacing = 2;
	
	if (text_xScale < 0) {
		text_xScale = 2;
	};

	if (text_yScale < 0) {
		text_yScale = text_xScale;
	};


	letter_spacing = 1;

	int letter_width = 5;	// these stay constant
	int letter_height = 5;

	int letter_id = 0;
	int letter_line = 0;
	int letter_column = -1;

	int text_length = text_GetSize(text);

	for (int letter = 0; letter < text_length; letter++) {

		letter_id = (int)(text[letter]) - 32;
		letter_line = 0;
		letter_column = -1;

		for (int i = 0; i < (letter_width * letter_height); i++) {

			if (text[letter] == 0) {
				continue;
			};

			letter_column++;

			if (letter_column >= 5) {
				letter_line++;
				letter_column = 0;
			};

			if (!text_bitmap[letter_id][i]) {
				continue;
			};


			int x = x1;
			x += letter_column * text_xScale;	// small rectangles
			x += letter * (letter_width + letter_spacing) * text_xScale;	// letter x position

			for (int sX = 0; sX < text_xScale; sX++) {

				x++;

				if (x < 0 || x >= frame.width) {
					continue;
				};

				for (int sY = 0; sY < text_yScale; sY++) {

					int y = y1;
					y += (letter_height * text_yScale) - (letter_line * text_yScale + sY);	// flip upside down
					y -= letter_height * text_yScale;	// shift down

					if (y < 0 || y >= frame.height) {
						continue;
					};

					frame.pixels[y * frame.width + x] = color;


				};

			};



		};

	};

	return;
};

void text_ClearString(char string[]) {

	for (int i = 0; i < string_maxLength; i++) {

		string[i] = 0;

	};

	return;
};

int text_GetSize(char string[]) {

	int size = 0;
	for (int i = 0; i < string_maxLength; i++) {

		if (string[i] == 0) {
			size = i;
			break;
		};

	};

	return (size);
};


void text_AppendString(char string1[], char string2[]) {

	if (string_maxLength < 0) {
		return;
	};

	int iEnd = 0;
	for (int i = 0; i < string_maxLength; i++) {

		if (!string1[i]) {
			iEnd = i;
			break;
		};

	};

	if (string_maxLength <= 0) {
		iEnd = 0;
	};

	for (int i = 0; i < string_maxLength; i++) {

		if (string_maxLength == 1) {
			string1[iEnd + i] = string2;
			continue;
		};

		if (string2[i] == 0) {	//when string2 over
			break;
		};

		string1[iEnd + i] = string2[i];

	};

	return;
}

// string_length should be MAX length of char array
void text_AppendInt(char string[], int num) {

	int iEnd = 0;
	for (int i = 0; i < string_maxLength; i++) {

		if (!string[i]) {
			iEnd = i;
			break;
		};

	};

	if (num == 0) {
		string[iEnd] = '0';
		return;
	};

	int num_length = 0;
	int tempNum = abs(num);

	while (tempNum > 0) {

		num_length++;
		tempNum /= 10;

	};

	if (num < 0) {
		string[iEnd] = '-';
		iEnd++;
	};

	num = abs(num);

	for (int i = num_length - 1; i >= 0; i--) {

		string[iEnd + i] = (char)(num % 10 + 48);

		num /= 10;
	};

	return;
}

double DeltasToDegrees(double deltaX, double deltaY) {

	if (deltaX == 0 && deltaY == 0) {
		return -1;
	};

	if (deltaY == 0 && deltaX > 0) {
		return 0;
	};

	if (deltaY == 0 && deltaX < 0) {
		return 180;
	};

	if (deltaX == 0 && deltaY > 0) {
		return 90;
	};

	if (deltaX == 0 && deltaY < 0) {
		return 270;
	};

	double angle = atan(deltaY / deltaX) * 180 / 3.14;
	if (deltaX < 0 && deltaY < 0) {	//Quadrant III
		return angle + 180;
	};

	if (deltaX < 0 && deltaY > 0) {	//Quadrant I
		return angle + 180;
	};

	return angle;
}

void frame_DrawRectFilled(int x1, int y1, int x2, int y2, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	if (x1 > x2) {
		int temp = x1;
		x1 = x2;
		x2 = temp;
	};

	if (y1 > y2) {
		int temp = y1;
		y1 = y2;
		y2 = temp;
	};

	for (int x = x1; x < x2; x++) {

		if (x < 0 || x >= frame.width) {
			continue;
		};

		for (int y = y1; y < y2; y++) {

			if (y < 0 || y >= frame.height) {
				continue;
			}

			frame.pixels[y * frame.width + x] = color;

		};

	};

	return;
};


	// cell_marchingCubes data (bitmap of each cell situation)
int bitmap_mCubes[27][100] = {

	{	// 0
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0
	},

	{	// 1
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	},
	
	{	// 2
		0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 1, 1, 1, 1, 1, 1
	},

	{	// 3
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0
	},

	{	// 4
		1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 0, 0, 0, 0
	},

	{	// 5
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	},

	{	// 6
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 0, 0, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1
	},

	{	// 7
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 0, 0, 1, 1, 1, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},

	{	// 8
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 0, 1, 1, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	},

	{	// 9
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 0, 1, 1, 1, 0, 0, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	},


	// diagonals

	{	// 10
		0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0
	},
	
	{	// 11
		1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0
	},

	{	// 12
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 0, 0, 0, 0

	},

	{	// 13
		0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 1, 1, 1, 1, 1, 1
	},

	{	// 14
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},

	{	// 15
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},

	{	// 16
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1
	},

	{	// 17
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1
	},

	{	// 18
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1
	},

	{	// 19
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},

	{	// 20
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1
	},

	{	// 21
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1
	},

	{	// 22
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1
	},

	{	// 23
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},

	{	// 24
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1
	},

	{	// 25
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	},

	{	// 26
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 1, 1, 0, 0, 0, 0, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 0, 0, 0, 0, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1
	},

};



frame_DrawCell(int x1, int y1, int x2, int y2, int cell, int color) {
	
	int cell_size = x2 - x1;
	int cellId = map_mCubes[cell];
	

	double increm = cell_size / 10.0;

	for (int x = 0; x < 10; x++) {

		for (int y = 0; y < 10; y++) {
			
			int p = bitmap_mCubes[cellId][(10 - y - 1) * 10 + x];

			if (!p) {
				continue;
			};

			int temp_x = x1 + (ceil)(1.0 * x * increm);
			int temp_y = y1 + (ceil)(1.0 * y * increm);
			
			frame_DrawRectFilled(temp_x, temp_y, temp_x + ceil(increm), temp_y + ceil(increm),
				color_Darken(color, map_densities[cell] * 10)
			);


		
		};

	};
	
	return;
};


int point_inWindow(int x, int y) {

	if (x < 0 || x >= frame.width || y < 0 || y >= frame.height) {
		return 0;
	};

	return 1;
};

#define lg 0x00BBBBBB
#define g 0x00444444
#define dg 0x00111111


int map_Neighbours(int x1, int y1) {

	int num = 0;

	for (int x = x1 - 1; x <= x1 + 1; x++) {

		if (x < 0 || x >= map_width) {
			continue;
		};

		for (int y = y1 - 1; y <= y1 + 1; y++) {

			if (y < 0 || y >= map_height) {
				continue;
			};

			if (map[y * map_width + x]) {

				num++;

			};

		};

	};

	if (map[y1 * map_width + x1]) {
		num -= 1;
	};


	return num;
};



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASS wc = { };
	wc.lpszClassName = CLASS_NAME;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;

	RegisterClass(&wc);

	frame_mapInfo.bmiHeader.biSize = sizeof(frame_mapInfo.bmiHeader);
	frame_mapInfo.bmiHeader.biPlanes = 1;
	frame_mapInfo.bmiHeader.biBitCount = 32;
	frame_mapInfo.bmiHeader.biCompression = BI_RGB;
	frame_dc = CreateCompatibleDC(frame_dc);

	DWORD style = WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_CAPTION;

	RECT rect;
	rect.left = 200;
	rect.top = 150;
	rect.right = rect.left + 400;
	rect.bottom = rect.top + 300;

	HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"AP - Cell Game (See instructions on top-right, and Enjoy!)", style, rect.left, rect.top, rect.right, rect.bottom, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);


	
	#define map_scale 20


	// set spawn & zoom
	double map_zoom = .75;
	map_anchor.x = (map_anchor.x + map_width * (map_zoom * map_scale)) / -2;
	map_anchor.y = (map_anchor.y + map_height * (map_zoom * map_scale)) / -2;



	/*
	for (int m = 0; m < map_width * map_height; m++) {


		map[m] = rand() % 2;

		if (rand() % 2) {
			map[m] = 0;
		};
		
		if (rand() % 2) {
			map[m] = 0;
		};


	};
	*/


	int game_running = 0;
	int run_counter1 = 10;


	int game_shadersOn = 1;
	int game_sCounter = 10;


	int game_debugMode = 0;
	int debug_counter = 10;
	
	int game_cCounter = 10;

	int game_tickSpeed = 20;
	int tick_counter = game_tickSpeed;


	int game_rCounter = 10;

	MSG msg = { };

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	while (running == 1) {


		PeekMessage(&msg, hWnd, 0u, 0u, PM_REMOVE);
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);


		/*
		while (PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE) != 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		};*/

		

		if (msg.message == WM_LBUTTONUP) {
			mouse.left = 0;
		};

		if (msg.message == WM_LBUTTONDOWN) {
			mouse.left = 1;
		};
		
		if (msg.message == WM_RBUTTONUP) {
			mouse.right = 0;
		};
		
		if (msg.message == WM_RBUTTONDOWN) {
			mouse.right = 1;
		};


		for (int i = 0; i < 128; i++) {
			keys[i] = 0;
			if (GetKeyState(i) / 10) {
				keys[i] = 1;
			};
		};

		if (keys[27]) {
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		};

			// debug mode
		if (keys[68] && debug_counter >= 10) {	// D
			game_debugMode = !game_debugMode;
			debug_counter = 0;
		};

		debug_counter++;


			// shadersOn
		if (keys[83] && game_sCounter >= 10) {	// S
			game_shadersOn = !game_shadersOn;
			game_sCounter = 0;
		}

		game_sCounter++;


		// play game shortcut
		if (keys[82] && (run_counter1 >= 10)) {	// R
			game_running = !game_running;
			run_counter1 = 0;
		};
		run_counter1++;


		// clr screen shortcut
		if (keys[67] && (game_cCounter >= 10)) {	// C
			
			for (int p = 0; p < map_width * map_height; p++) {
				map[p] = 0;
			};
			
			game_cCounter = 0;
		};
		game_cCounter++;


		// randomize center of map
		if (keys[80] && (game_rCounter >= 10)) {	// P

			for (int x = map_width * .475; x < map_width * .525; x++) {

				for (int y = map_height * .475; y < map_height * .525; y++) {

					if (rand() % 2) {
						continue;
					};

					map[y * map_width + x] = rand() % 2;
				};

			};

			game_rCounter = 0;
		};
		game_rCounter++;


		double map_anchorRatio_x = -1.0 * map_anchor.x / (map_width * map_scale * map_zoom);	// dist from anchor x to end x
		double map_anchorRatio_y = -1.0 * map_anchor.y / (map_height * map_scale * map_zoom);	// dist from anchor y to end y

		// zooming
		if (keys[73] && (map_zoom < 10)) {	// I
			map_zoom += .01;
			map_anchor.x -= (int)(map_anchorRatio_x * map_width * (map_scale / 100.0));
			map_anchor.y -= (int)(map_anchorRatio_y * map_height * (map_scale / 100.0));
		};

		if (keys[79] && (map_zoom > .1)) {	// O
			map_zoom -= .01;
			map_anchor.x += (int)(map_anchorRatio_x * map_width * (map_scale / 100.0));
			map_anchor.y += (int)(map_anchorRatio_y * map_height * (map_scale / 100.0));
		};



		if (GetAsyncKeyState(VK_UP)) {
			game_tickSpeed++;
		};

		if (GetAsyncKeyState(VK_DOWN) && (game_tickSpeed > 1)) {
			game_tickSpeed--;
		};


		for (int i = 0; i < frame.width * frame.height; i++) {
			frame.pixels[i] = 0;
		};


		GetWindowRect(hWnd, &rect);

		// get mouse pos and dx
		int mouse_tempDX = mouse.x;
		int mouse_tempDY = mouse.y;

		GetCursorPos(&mouse);
		mouse.y = frame.height - mouse.y;
		mouse.y += rect.top + 30;
		mouse.x -= rect.left;
		mouse.x -= 8;

		mouse_tempDX -= mouse.x;
		mouse_tempDY -= mouse.y;

		mouse.dx = mouse_tempDX;
		mouse.dy = mouse_tempDY;


		// check if mouse in
		mouse.in = 0;

		if (!(mouse.y < pen_size || mouse.y >= frame.height - pen_size || mouse.x < pen_size || mouse.x >= frame.width - pen_size)) {
			mouse.in = 1;
		};

		// set map_anchor pos
		if (mouse.right && mouse.in) {
			map_anchor.x -= mouse.dx;
			map_anchor.y -= mouse.dy;
		};



		char map_temp[map_width * map_height] = { 0 };

		// run game
		for (int x = 0; x < map_width; x++) {

			for (int y = 0; y < map_height; y++) {

				if (!game_running) {
					break;
				};

				if (tick_counter < game_tickSpeed) {
					break;
				};

				int p = y * map_width + x;

				//map_temp[p] = map[p];

				
				if (map[p] && map_Neighbours(x, y) < 2) {
					map_temp[p] = 0;
					continue;
				};

				if (map[p] && (map_Neighbours(x, y) == 2 || map_Neighbours(x, y) == 3)) {
					map_temp[p] = 1;
					continue;
				};

				if (map[p] && map_Neighbours(x, y) > 3) {
					map_temp[p] = 0;
					continue;
				};
				
				if (!map[p] && map_Neighbours(x, y) == 3) {
					map_temp[p] = 1;
				};


			};

		};

		// copy temp_map to amp
		for (int m = 0; m < map_width * map_height; m++) {

			if (!game_running) {
				break;
			};

			if (tick_counter < game_tickSpeed) {
				break;
			};

			map[m] = map_temp[m];
		};

		if (tick_counter >= game_tickSpeed) {
			tick_counter = 0;
		};

		tick_counter++;



		// drawing with mouse
		int mouseT_x = (1.0 * mouse.x - map_anchor.x) - frame.width / 2;
		mouseT_x = mouseT_x / (1.0 * map_scale * map_zoom);

		int mouseT_y = (1.0 * mouse.y - map_anchor.y) - frame.height / 2;
		mouseT_y = mouseT_y / (1.0 * map_scale * map_zoom);


		int p = mouseT_y * map_width + mouseT_x;

		if (mouseT_x < 0 || mouseT_x >= map_width) {
			p = -1;
		};

		if (mouseT_y < 0 || mouseT_y >= map_height) {
			p = -1;
		};


		if (mouse.left && p > 0) {
			map[p] = 1;
		};


		if (mouse.right && p > 0) {
			map[p] = 0;
		};


		int map_screenEnd_x = ((frame.width / 2) - map_anchor.x + 30) / (1.0 * map_scale * map_zoom);

		if (map_screenEnd_x < 0 || map_screenEnd_x >= map_width) {
			map_screenEnd_x = map_width - 1;
		};

		int map_screenEnd_y = ((frame.height / 2) - map_anchor.y + 30) / (1.0 * map_scale * map_zoom);

		if (map_screenEnd_y < 0 || map_screenEnd_y >= map_height) {
			map_screenEnd_y = map_height - 1;
		};


		int map_screenStart_x = ((frame.width / 2) - map_anchor.x - 30 - frame.width) / (1.0 * map_scale * map_zoom);

		if (map_screenStart_x < 0 || map_screenStart_x >= map_width) {
			map_screenStart_x = 0;
		};

		int map_screenStart_y = ((frame.height / 2) - map_anchor.y - 30 - frame.height) / (1.0 * map_scale * map_zoom);

		if (map_screenStart_y < 0 || map_screenStart_y >= map_height) {
			map_screenStart_y = 0;
		};
		

			// calculate marching cubes/shaders
		for (int x = map_screenStart_x; x < map_screenEnd_x; x++) {

			for (int y = map_screenStart_y; y < map_screenEnd_y; y++) {


				if (!map[y * map_width + x]) {
					continue;
				};


				int u = 1;
				int d = 1;
				int l = 1;
				int r = 1;

				if (y + 1 >= 0 && y < map_height) {
					u = map[(y + 1) * map_width + x];
				};

				if (y - 1 >= 0 && y < map_height) {
					d = map[(y - 1) * map_width + x];
				};

				if (x - 1 >= 0 && y < map_width) {
					l = map[y * map_width + (x - 1)];
				};
				
				if (x + 1 >= 0 && y < map_width) {
					r = map[y * map_width + (x + 1)];
				};

				
				if (!(u + l + d) && r) {
					map_mCubes[y * map_width + x] = 2;
					continue;
				};

				if (!(l + d + r) && u) {
					map_mCubes[y * map_width + x] = 3;
					continue;
				};

				if (!(u + r + d) && l) {
					map_mCubes[y * map_width + x] = 4;
					continue;
				};

				if (!(l + u + r) && d) {
					map_mCubes[y * map_width + x] = 5;
					continue;
				};

				if (!(l + d) && ((u + r) == 2)) {
					map_mCubes[y * map_width + x] = 6;
					continue;
				};

				if (!(d + r) && ((l + u) == 2)) {
					map_mCubes[y * map_width + x] = 7;
					continue;
				};
				
				if (!(u + r) && ((l + d) == 2)) {
					map_mCubes[y * map_width + x] = 8;
					continue;
				};

				if (!(l + u) && ((d + r) == 2)) {
					map_mCubes[y * map_width + x] = 9;
					continue;
				};
				

				if (u + d + l + r == 4) {
					map_mCubes[y * map_width + x] = 1;
					continue;
				};

				if ((u + d) && !(l + r)) {	// top & bottom
					map_mCubes[y * map_width + x] = 25;
					continue;
				};

				if (!(u + d) && (l + r)) {	// left & right
					map_mCubes[y * map_width + x] = 26;
					continue;
				};

				

				if (u + d + l + r) {	// if any side, then dont process diagonals
					map_mCubes[y * map_width + x] = 1;
					continue;
				};

				// diagonal mCubes

				int ul = 1;
				int dl = 1;
				int ur = 1;
				int dr = 1;

				if ((x - 1 >= 0 && x - 1 < map_width) && (y + 1 >= 0 && y + 1 < map_height)) {
					ul = map[(y + 1) * map_width + (x - 1)];
				};

				if ((x - 1 >= 0 && x - 1 < map_width) && (y - 1 >= 0 && y - 1 < map_height)) {
					dl = map[(y - 1) * map_width + (x - 1)];
				};

				if ((x + 1 >= 0 && x + 1 < map_width) && (y + 1 >= 0 && y + 1 < map_height)) {
					ur = map[(y + 1) * map_width + (x + 1)];
				};

				if ((x + 1 >= 0 && x + 1 < map_width) && (y - 1 >= 0 && y - 1 < map_height)) {
					dr = map[(y - 1) * map_width + (x + 1)];
				};
				

				if (!(ul + dl + dr) && ur) {
					map_mCubes[y * map_width + x] = 10;
					continue;
				};

				if (!(dl + dr + ur) && ul) {
					map_mCubes[y * map_width + x] = 11;
					continue;
				};

				if (!(ul + dr + ur) && dl) {
					map_mCubes[y * map_width + x] = 12;
					continue;
				};

				if (!(ul + dl + ur) && dr) {
					map_mCubes[y * map_width + x] = 13;
					continue;
				};

				if (!(dl + dr) && ((ul + ur) == 2)) {
					map_mCubes[y * map_width + x] = 14;
					continue;
				};

				if (!(ur + dr) && ((ul + dl) == 2)) {
					map_mCubes[y * map_width + x] = 15;
					continue;
				};

				if (!(ul + ur) && ((dl + dr) == 2)) {
					map_mCubes[y * map_width + x] = 16;
					continue;
				};

				if (!(ul + dl) && ((ur + dr) == 2)) {
					map_mCubes[y * map_width + x] = 17;
					continue;
				};

				if (((ul + ur + dr) == 3) && !dl) {
					map_mCubes[y * map_width + x] = 18;
					continue;
				};

				if (((dl + ul + ur) == 3) && !dr) {
					map_mCubes[y * map_width + x] = 19;
					continue;
				};

				if (((ul + dl + dr) == 3) && !ur) {
					map_mCubes[y * map_width + x] = 20;
					continue;
				};

				if (((dl + dr + ur) == 3) && !ul) {
					map_mCubes[y * map_width + x] = 21;
					continue;
				};

				if (!(dl + ur) && (ul + dr)) {
					map_mCubes[y * map_width + x] = 22;
					continue;
				};

				if (!(ul + dr) && (dl + ur)) {
					map_mCubes[y * map_width + x] = 23;
					continue;
				};

				if (ul + dl + dr + ur == 4 && !(u + l + d + r)) {
					map_mCubes[y * map_width + x] = 24;
					continue;
				};


				map_mCubes[y * map_width + x] = 0;


			};

		};

		
			// calculate cell densities (> neighbors = darker color)
		for (int x = map_screenStart_x; x < map_screenEnd_x; x++) {

			for (int y = map_screenStart_y; y < map_screenEnd_y; y++) {


				map_densities[y * map_width + x] = map_Neighbours(x, y);

			};

		};


			// draw map
		frame_DrawRectFilled(map_anchor.x + frame.width / 2, map_anchor.y + frame.height / 2, map_anchor.x + (map_width - 1) * (map_zoom * map_scale) + frame.width / 2, map_anchor.y + (map_height - 1) * (map_zoom * map_scale) + frame.height / 2, 0x00111111);

		for (int x = map_screenStart_x; x < map_screenEnd_x; x++) {

			for (int y = map_screenStart_y; y < map_screenEnd_y; y++) {

				int scale = map_zoom * map_scale;

				int temp_x = (x * map_zoom * map_scale) + map_anchor.x + frame.width / 2;
				int temp_y = (y * map_zoom * map_scale) + map_anchor.y + frame.height / 2;



				if (mouseT_x == x && mouseT_y == y) {
					frame_DrawRectFilled(temp_x, temp_y, temp_x + (map_zoom * map_scale), temp_y + (map_zoom * map_scale), g);
					continue;
				};

					// draw background grid markers
				if (x * y % 2) {
					frame_DrawRectFilled(temp_x, temp_y, temp_x + (map_zoom * map_scale), temp_y + (map_zoom * map_scale), 0x00131313);	// optimize this?
				};

				
					// draw cells
				if (!map[y * map_width + x]) {
					continue;
				};
				
				if (game_shadersOn) {
					frame_DrawCell(temp_x, temp_y, temp_x + (map_zoom * map_scale), temp_y + (map_zoom * map_scale), (y * map_width + x), 0x00558855);
					continue;
				};

				if (!game_shadersOn) {
					frame_DrawRectFilled(temp_x, temp_y, temp_x + (map_zoom * map_scale), temp_y + (map_zoom * map_scale), 0x00555555);
				};


			};


		};



		// draw mouse
		int mouse_color = 0x00333333;

		if (mouse.left) {
			mouse_color = 0x00FFFFFF;
		};

		if (mouse.in) {
			frame_DrawRectFilled(mouse.x - ((pen_size - 1) / 2), mouse.y - ((pen_size - 1) / 2), mouse.x + ((pen_size + 1) / 2), mouse.y + ((pen_size - 1) / 2), mouse_color);
		};



		// draw screen anchor
		frame_DrawRectFilled(map_anchor.x - 3 + frame.width / 2, map_anchor.y - 3 + frame.height / 2, map_anchor.x + 3 + frame.width / 2, map_anchor.y + 3 + frame.height / 2, 0x0000FF00);

		frame_DrawRectFilled(frame.width / 2 - 1, frame.height / 2 - 3, frame.width / 2 + 1, frame.height / 2 + 3, 0x00FF0000);
		frame_DrawRectFilled(frame.width / 2 - 3, frame.height / 2 - 1, frame.width / 2 + 3, frame.height / 2 + 1, 0x00FF0000);



		frame_DrawText(5, frame.height - 5, "Debug Mode [D]", 1, 1, 0x00FFFFFF);
		frame_DrawText(5, frame.height - 15, "Pan [rBtn] / Draw [lBtn]", 1, 1, 0x00FFFFFF);
		frame_DrawText(5, frame.height - 25, "Tickspeed [up/down arw]", 1, 1, 0x00FFFFFF);
		frame_DrawText(5, frame.height - 35, "Shader Toggle [S]", 1, 1, 0x00FFFFFF);

		if (!game_running) {
			frame_DrawText(frame.width - 99, 19, "Run (R)", 2, 2, 0x00003300);
			frame_DrawText(frame.width - 100, 20, "Run (R)", 2, 2, 0x0000AA00);
		};

		if (game_running) {
			frame_DrawText(frame.width - 119, 19, "Pause (R)", 2, 2, 0x00440000);
			frame_DrawText(frame.width - 120, 20, "Pause (R)", 2, 2, 0x00FF0000);
		};


		char text[string_maxLength] = { 0 };
		text_AppendString(&text, "Tickspeed: ");
		text_AppendInt(&text, game_tickSpeed);
		frame_DrawText(frame.width - 105, 35, &text, 1, 1, 0x00FFFFFF);

		if (!game_debugMode) {

			InvalidateRect(hWnd, NULL, 0);
			UpdateWindow(hWnd);

			Sleep(20);
			continue;
		};

		text_ClearString(&text);
		text_AppendString(&text, "Anchor: ");
		text_AppendInt(&text, map_anchor.x);
		text_AppendString(&text, ",");
		text_AppendInt(&text, map_anchor.y);
		frame_DrawText(10, 20, &text, 2, 2, 0x00FFFFFF);

		text_ClearString(&text);
		text_AppendString(&text, "zoom: ");
		text_AppendInt(&text, map_zoom * 100);
		frame_DrawText(10, 40, &text, 2, 2, 0x00FFFFFF);

		text_ClearString(&text);
		text_AppendString(&text, "end: ");
		text_AppendInt(&text, map_anchor.x + map_width * (map_zoom * map_scale));
		text_AppendString(&text, ",");
		text_AppendInt(&text, map_anchor.y + map_height * (map_zoom * map_scale));
		frame_DrawText(10, 60, &text, 2, 2, 0x00FFFFFF);

		text_ClearString(&text);
		text_AppendString(&text, "anchor:end ");
		text_AppendInt(&text, abs(map_anchorRatio_x * 100.0));
		text_AppendString(&text, ",");
		text_AppendInt(&text, abs(map_anchorRatio_y * 100.0));
		frame_DrawText(250, 20, &text, 2, 2, 0x00FFFFFF);

		text_ClearString(&text);
		text_AppendString(&text, "mouse: ");
		text_AppendInt(&text, (int)(mouse.x - frame.width / 2));
		text_AppendString(&text, ",");
		text_AppendInt(&text, (int)(mouse.y - frame.height / 2));
		frame_DrawText(250, 40, &text, 2, 2, 0x00FFFFFF);

		text_ClearString(&text);
		text_AppendString(&text, "mouseT: ");
		text_AppendInt(&text, mouseT_x);
		text_AppendString(&text, ",");
		text_AppendInt(&text, mouseT_y);
		frame_DrawText(250, 60, &text, 2, 2, 0x00FFFFFF);


		text_ClearString(&text);
		text_AppendInt(&text, map_anchor.x + map_width * (map_zoom * map_scale));
		text_AppendString(&text, ",");
		text_AppendInt(&text, map_anchor.y + map_height * (map_zoom * map_scale));
		frame_DrawText(map_anchor.x + map_width * (map_zoom * map_scale) + frame.width / 2, map_anchor.y + map_height * (map_zoom * map_scale) + frame.height / 2, &text, 2, 2, 0x00FFFFFF);


		text_ClearString(&text);
		text_AppendString(&text, "neighbors: ");
		text_AppendInt(&text, map_Neighbours(mouseT_x, mouseT_y));
		frame_DrawText(250, 80, &text, 2, 2, 0x00FFFFFF);



		InvalidateRect(hWnd, NULL, 0);
		UpdateWindow(hWnd);

		Sleep(20);


	};


	return 0;
};



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

		frame_mapInfo.bmiHeader.biWidth = LOWORD(lParam);
		frame_mapInfo.bmiHeader.biHeight = HIWORD(lParam);

		if (frame_hBitmap) {
			DeleteObject(frame_hBitmap);
		};

		frame_hBitmap = CreateDIBSection(NULL, &frame_mapInfo, DIB_RGB_COLORS, &frame.pixels, 0, 0);

		SelectObject(frame_dc, frame_hBitmap);

		frame.width = LOWORD(lParam);
		frame.height = HIWORD(lParam);


		break;
	};


	case (WM_PAINT): {

		static PAINTSTRUCT ps;
		static HDC hdc;
		hdc = BeginPaint(hWnd, &ps);

		BitBlt(
			hdc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			(ps.rcPaint.right - ps.rcPaint.left),
			(ps.rcPaint.bottom - ps.rcPaint.top),
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

	return 0;

};