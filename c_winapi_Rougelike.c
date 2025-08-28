#include <stdio.h>
#include <Windows.h>
#include <Math.h>


static int running = 1;
static int keys[128];

struct {
	int width;
	int height;
	int* pixels;
} frame = { 0 };

static BITMAPINFO frame_bitmapInfo;
static HBITMAP frame_hBitmap = 0;
static HDC frame_dc = 0;

//--------------------


#define string_maxLength 100

int player_stats[4] = { 10, 4 };
// { health, attack }

int player_gear_stats[2][2] = {
	// hp, atk
	{ 9, 0 },
	{ 0, 4 }
};


char wordList_adjectives[100][100] = { 0 };
char wordList_nouns[100][100] = { 0 };

//int (*pGear_stats)[2] = &player_gear;
// { weapon, armor } { stats of each item}

//char player_gear_names[5][string_maxLength] = { 0 };
// { weaponName, armorName, weaponName, armorName,.. }


struct {
	double x;
	double y;
	double speed;
	int dir;
	int icon;
	int gold;
	int* stats;
	int gear_stats[2][2];
	char gear_names[2][string_maxLength];
} plr = { 0, 0, 4, 90, 0, 100, &player_stats, { 0 }, { 0 } };

static int plr_tile_x;
static int plr_tile_y;


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


int NormalizeAngle(int angle) {


	while (angle >= 360) {
		angle -= 360;
	};

	while (angle < 0) {
		angle += 360;
	};

	return angle;
};


void frame_DrawLine(int x1, int y1, int x2, int y2, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	double deltaX = x2 - x1;
	double deltaY = y2 - y1;

	if ((deltaX == 0) && (deltaY == 0)) { //if given a point
		return;
	};

	double tIncrement = 1 / sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

	//t goes from 0 - 1; 1 is endpoint
	for (double t = 0; t <= 1; t += tIncrement) {

		int x = (int)(x1 + deltaX * t);
		int y = (int)(y1 + deltaY * t);

		if ((x <= 0) || (x >= frame.width)) {
			continue;
		};

		int pixel = y * frame.width + x;

		if ((pixel < 0) || (pixel > frame.width * frame.height)) {
			continue;
		};

		frame.pixels[pixel] = color;

	};

	return;
}


frame_DrawLineA(int x1, int y1, int angle, int dist, int color) {


	int deltaX = dist * cos(angle * 3.14 / 180);
	int deltaY = dist * sin(angle * 3.14 / 180);

	frame_DrawLine(x1, y1, x1 + deltaX, y1 + deltaY, color);

	return;
};


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

inline void frame_DrawRectFilledInline(int x1, int y1, int x2, int y2, int color()) {

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

void frame_DrawRectA(int xAnchor, int yAnchor, int horizontalShift, int verticalShift, double angle, int width, int height, int color) {

	int x1 = xAnchor + horizontalShift * cos((angle - 90) * 3.14 / 180) + verticalShift * cos(angle * 3.14 / 180);
	int y1 = yAnchor + horizontalShift * sin((angle - 90) * 3.14 / 180) + verticalShift * sin(angle * 3.14 / 180);

	int x2 = x1 + height * cos(angle * 3.14 / 180);
	int y2 = y1 + height * sin(angle * 3.14 / 180);

	int x4 = x1 + width * cos((angle - 90) * 3.14 / 180);
	int y4 = y1 + width * sin((angle - 90) * 3.14 / 180);

	int x3 = x1 + height * cos(angle * 3.14 / 180) + width * cos((angle - 90) * 3.14 / 180);
	int y3 = y1 + height * sin(angle * 3.14 / 180) + width * sin((angle - 90) * 3.14 / 180);

	frame_DrawLine(x1, y1, x2, y2, color);
	frame_DrawLine(x2, y2, x3, y3, color);
	frame_DrawLine(x3, y3, x4, y4, color);
	frame_DrawLine(x4, y4, x1, y1, color);

	return;
};


void frame_DrawRectAFilled(int xAnchor, int yAnchor, int horizontalShift, int verticalShift, int angle, int width, int height, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	if (angle == 90) {
		frame_DrawRectFilled(xAnchor + horizontalShift, yAnchor + verticalShift, xAnchor + width + horizontalShift, yAnchor + height + verticalShift, color);
		return;
	};


	if (angle == 270) {
		frame_DrawRectFilled(xAnchor - horizontalShift, yAnchor - verticalShift, xAnchor - width - horizontalShift, yAnchor - height - verticalShift, color);
		return;
	};

	int x1 = xAnchor + horizontalShift * cos((angle - 90) * 3.14 / 180) + verticalShift * cos(angle * 3.14 / 180);
	int y1 = yAnchor + horizontalShift * sin((angle - 90) * 3.14 / 180) + verticalShift * sin(angle * 3.14 / 180);

	int x3 = x1 + height * cos(angle * 3.14 / 180) + width * cos((angle - 90) * 3.14 / 180);
	int y3 = y1 + height * sin(angle * 3.14 / 180) + width * sin((angle - 90) * 3.14 / 180);

	int angle1 = NormalizeAngle(angle);
	int angle2 = NormalizeAngle(angle - 90);

	int angle3 = NormalizeAngle(angle + 180);
	int angle4 = NormalizeAngle(angle + 90);

	int longestSide = height;
	if (width > height) {
		longestSide = width;
	};

	for (int x = x1 - longestSide * 1.5; x < x1 + longestSide * 1.5; x++) {

		if (x < 0 || x >= frame.width) {
			continue;
		};

		for (int y = y1 - longestSide * 1.5; y < y1 + longestSide * 1.5; y++) {

			if (y < 0 || y >= frame.height) {
				continue;
			};

			int fill = 1;
			int pointAngle = NormalizeAngle(DeltasToDegrees(x - x1, y - y1));

			if (pointAngle < angle1 != pointAngle < angle2) {
				fill = -1;
			};

			if (NormalizeAngle(angle) >= 90) {
				fill *= -1;
			};

			if (fill == -1) {
				continue;
			};

			fill = 1;
			pointAngle = NormalizeAngle(DeltasToDegrees(x - x3, y - y3));

			if (pointAngle <= angle3 == pointAngle <= angle4) {
				fill = -1;
			};

			if (NormalizeAngle(angle3) < 90) {
				fill *= -1;
			};

			if (fill == -1) {
				continue;
			};

			frame.pixels[y * frame.width + x] = color;

		};

	};

	return;
};


void frame_DrawCircleFilled(int x1, int y1, int radius, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	for (int x = x1 - radius; x < x1 + radius; x++) {

		if (x < 0 || x >= frame.width) {
			continue;
		};

		for (int y = y1 - radius; y < y1 + radius; y++) {

			if ((y < 0) || (y >= frame.height)) {
				continue;
			};

			// (y-h)^2 + (x-h)^2 = r^2
			if (sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1)) > radius) {
				continue;
			};

			frame.pixels[y * frame.width + x] = color;

		};

	};

	return;
}



#define w 0x00FFFFFF

#define r 0x00FF0000	//red
#define lr 0x00FF4400	//red
#define dr 0x00AA0000	//darkred

#define yl 0x00BB99933	//yellow
#define pyl 0x00BB9944	//paleyellow
#define lyl 0x00DDCC55	//lightyellow
#define dyl 0x00997733	//darkyellow

#define tn 0x00FFEE99	//tan


#define g 0x00008822	//green
#define lg 0x0000AA22	//lightgreen
#define dg 0x00006622	//darkgreen
#define ddg 0x00003315	//darkgreen


#define b 0x000033AA	//blue
#define lb 0x000055FF	//lightblue
#define db 0x00002277	//darkblue
#define ddb 0x00001755	//darkerblue

#define pu 0x00FF00FF	//purple
#define dpu 0x00AA00AA	//darkpurple

#define br 0x00442200	//brown
#define lbr 0x00665500	//lightbrown
#define dbr 0x00332200	//darkbrown

#define gry 0x00333333//gray
#define lgry 0x00555555	//lightgray
#define dgry 0x00111111	//darkgray

#define color_Opacity(color1, color2, opacity) ( RGB(GetRValue(color2) * opacity + GetRValue(color1) * (1 - opacity), GetGValue(color2) * opacity + GetGValue(color1) * (1 - opacity), GetBValue(color2) * opacity + GetBValue(color1) * (1 - opacity)) )

#define color_Darken(color, percentage) RGB(GetRValue(color) * (percentage / 100.0), GetGValue(color) * (percentage / 100.0), GetBValue(color) * (percentage / 100.0))

//#define b 0x00112211	//black
#define _ -1


int image_player_normal[] = {
	13, 17,
	_, _, _, _, _, 0, 0, 0, 0, _, _, _, _,
	_, _, _, _, 0, yl, yl, yl, 0, _, 0, _, _,
	_, _, _, 0, yl, yl, tn, tn, 0, 0, br, 0, _,
	_, _, _, 0, yl, tn, tn, tn, tn, 0, br, br, 0,
	_, _, 0, yl, yl, tn, tn, tn, 0, 0, br, br, 0,
	_, _, _, 0, yl, tn, tn, tn, 0, 0, br, 0, _,
	_, _, _, 0, g, g, dg, dg, 0, tn, dbr, 0, _,
	_, _, 0, g, g, g, g, dg, dg, tn, dbr, 0, _,
	_, _, 0, g, g, g, g, dg, dg, 0, br, 0, _,
	_, 0, g, g, g, g, g, dg, 0, 0, br, 0, _,
	_, 0, g, g, g, g, g, g, dg, 0, br, 0, _,
	_, 0, g, g, g, g, g, g, dg, 0, 0, _, _,
	_, 0, g, g, g, g, g, g, dg, 0, _, _, _,
	0, g, g, g, g, g, g, dbr, g, 0, _, _, _,
	0, g, br, dbr, g, dbr, g, dbr, dbr, 0, _, _, _,
	0, 0, br, dbr, dbr, 0, 0, br, br, 0, _, _, _,
	_, _, 0, 0, 0, _, _, 0, 0, _, _, _, _
};

int image_player_wet[] = {
	13, 17,
	_, _, _, _, _, 0, 0, 0, 0, _, _, _, _,
	_, _, _, _, 0, yl, yl, yl, 0, _, 0, _, _,
	_, _, _, 0, yl, yl, tn, tn, 0, 0, br, 0, _,
	_, _, _, 0, yl, tn, tn, tn, tn, 0, br, br, 0,
	_, _, 0, yl, yl, tn, tn, tn, 0, 0, br, br, 0,
	_, _, _, 0, yl, tn, tn, tn, 0, 0, br, 0, _,
	_, _, _, 0, g, g, g, g, 0, tn, dbr, 0, _,
	_, _, 0, g, g, g, g, g, g, tn, dbr, 0, _,
	_, _, 0, g, g, g, g, g, g, 0, br, 0, _,
	_, 0, g, g, g, g, g, g, 0, 0, br, 0, _,
	_, 0, g, g, g, g, g, g, g, 0, br, 0, _,
	_, g, w, w, g, g, w, g, w, w, w, _, _,
	w, w, w, w, w, w, w, w, w, w, _, _, _,
	_, _, w, _, _, _, w, w, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _
};

int image_player_kneedeep[] = {
	13, 17,
	_, _, _, _, _, 0, 0, 0, 0, _, _, _, _,
	_, _, _, _, 0, yl, yl, yl, 0, _, 0, _, _,
	_, _, _, 0, yl, yl, tn, tn, 0, 0, br, 0, _,
	_, _, _, 0, yl, tn, tn, tn, tn, 0, br, br, 0,
	_, _, 0, yl, yl, tn, tn, tn, 0, 0, br, br, 0,
	_, _, _, 0, yl, tn, tn, tn, 0, 0, br, 0, _,
	_, _, _, 0, g, g, g, g, 0, tn, dbr, 0, _,
	_, _, 0, g, g, g, g, w, g, tn, dbr, 0, _,
	_, _, w, g, g, g, w, w, w, 0, br, w, _,
	w, w, w, w, w, w, w, w, w, w, w, w, w,
	_, _, _, _, w, w, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _
};

int image_player_submerged[] = {
	13, 17,
	_, _, _, _, _, 0, 0, 0, 0, _, _, _, _,
	_, _, _, _, 0, yl, yl, yl, 0, _, 0, _, _,
	_, _, _, 0, yl, yl, tn, tn, 0, 0, w, 0, _,
	_, _, w, w, w, tn, tn, tn, tn, w, w, br, 0,
	w, w, w, w, w, w, w, w, w, w, w, w, w,
	_, _, _, _, w, w, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _
};

int image_player_inGrass[] = {
	13, 17,
	_, _, _, _, _, 0, 0, 0, 0, _, _, _, _,
	_, _, _, _, 0, yl, yl, yl, 0, _, 0, _, _,
	_, _, _, 0, yl, yl, tn, tn, 0, 0, br, 0, _,
	_, _, _, 0, yl, tn, tn, tn, tn, 0, br, br, 0,
	_, _, 0, yl, yl, tn, tn, tn, 0, 0, br, br, 0,
	_, _, _, 0, yl, tn, tn, tn, 0, 0, br, 0, _,
	_, _, _, 0, g, g, g, g, 0, tn, dbr, 0, _,
	_, _, 0, g, g, g, g, g, g, tn, dbr, 0, _,
	_, _, 0, g, g, g, g, g, g, 0, br, 0, _,
	_, 0, g, g, g, g, g, g, 0, 0, br, 0, _,
	_, 0, g, g, g, g, g, g, g, 0, br, 0, _,
	_, 0, g, g, g, g, g, g, g, 0, 0, _, _,
	_, 0, g, g, g, g, g, g, g, 0, _, _, _,
	dg, dg, dg, g, g, g, g, dg, g, 0, _, _, _,
	_, _, dg, dg, dg, dg, dg, dg, dg, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _,
	_, _, _, _, _, _, _, _, _, _, _, _, _
};


int image_shallowwater_seagrass[] = {
	5, 5,
	_, _, _, lg, _,
	lg, _, lg, _, _,
	lg, _, g, _, _,
	_, lg, _, g, _,
	_, g, _, g, _
};

int image_water_lilypad[] = {

	5, 5,
	_, _, _, lg, _,
	lg, _, lg, _, _,
	lg, _, g, _, lg,
	_, g, _, g, _,
	_, g, _, g, _

};

int image_deepwater_ripple[] = {
	5, 5,
	_, g, _, _, _,
	_, _, g, _, g,
	g, _, dg, _, dg,
	_, dg, _, dg, _,
	_, dg, _, dg, _

};

int image_grass_flower[] = {
	5, 5,
	_, _, _, dr, _,
	_, r, dr, dr, dr,
	r, yl, r, dr, _,
	_, r, _, dg, _,
	_, dg, _, dg, _
};

int image_lushgrass_fern[] = {
	5, 5,
	_, _, _, _, _,
	_, _, lg, _, _,
	g, _, lg, _, g,
	_, g, g, g, _,
	_, _, lg, _, _
};

int image_sand_shell[] = {
	5, 5,
	_, _, _, _, _,
	_, _, pu, _, _,
	_, pu, pu, pu, _,
	_, dpu, pu, dpu, _,
	_, _, dpu, _, _
};

int image_treasure_chest[] = {
	5, 5,
	br, lbr, lbr, lbr, br,
	br, lbr, yl, lbr, br,
	dbr, br, yl, br, dbr,
	dbr, br, br, br, dbr,
	dbr, dbr, dbr, dbr, dbr
};

int image_treasure_brokenchest[] = {
	5, 5,
	g, g, g, g, g,
	g, g, g, g, g,
	dbr, yl, yl, g, g,
	dbr, br, br, yl, dbr,
	dbr, dbr, dbr, dbr, dbr
};


/*

		_, _, _, _, _, 0, 0, 0, 0, _, _, _, _,
	_, _, _, _, 0, yl, yl, yl, 0, _, _, _, _,
	_, _, _, 0, yl, yl, tn, tn, 0, 0, _, _, _,
	_, _, _, 0, yl, tn, tn, tn, tn, 0, _, _, _,
	_, _, 0, yl, yl, tn, tn, tn, 0, 0, _, _, _,
	_, _, _, 0, yl, tn, tn, tn, 0, _, _, _, _,
	_, _, _, 0, g, dg, dg, dg, 0, 0, _, _, _,
	_, _, 0, g, g, g, g, dg, dg, 0, 0, 0, _,
	_, _, 0, g, g, g, g, dg, dg, tn, tn, 0, _,
	_, _, 0, g, g, tn, g, dg, dg, tn, 0, 0, _,
	_, _, 0, g, tn, tn, g, g, dg, 0, 0, _, _,
	_, _, 0, g, tn, dg, g, g, dg, 0, _, _, _,
	_, _, 0, g, dg, g, g, g, g, 0, _, _, _,
	_, _, 0, g, g, g, g, dbr, g, 0, _, _, _,
	_, _, 0, dbr, g, dbr, g, dbr, dbr, 0, _, _, _,
	_, _, 0, dbr, dbr, 0, 0, br, br, 0, _, _, _,
	_, _, 0, 0, 0, _, _, 0, 0, _, _, _, _

_, _, _, _, yl, yl, yl, _, _, _, _, _, _,
	_, _, _, _, tn, tn, yl, yl, _, _, _, _, _,
	_, _, _, tn, tn, tn, tn, yl, _, _, _, _, _,
	_, _, _, _, tn, tn, tn, yl, yl, _, _, _, _,
	_, _, _, _, tn, tn, tn, yl, _, _, _, _, _,*/


int image_alert[] = {
	7, 17,
	_, _, 0, 0, 0, _, _,
	_, 0, lr, lr, lr, 0, _,
	0, lr, lr, lr, r, r, 0,
	0, lr, lr, r, r, r, 0,
	0, lr, lr, r, r, r, 0,
	_, 0, lr, r, r, 0, _,
	_, 0, lr, r, r, 0, _,
	_, 0, r, r, r, 0, _,
	_, _, 0, r, 0, _, _,
	_, _, 0, r, 0, _, _,
	_, _, 0, r, 0, _, _,
	_, _, 0, 0, 0, _, _,
	_, _, _, _, _, _, _,
	_, _, 0, 0, 0, _, _,
	_, 0, lr, lr, r, 0, _,
	_, 0, lr, r, r, 0, _,
	_, _, 0, 0, 0, _, _
};



int image_heart[] = {

	5, 5,
	r, r, _, r, r,
	r, r, r, r, r,
	r, r, r, r, r,
	_, r, r, r, _,
	_, _, r, _, _,

};

int image_sword[] = {

	5, 5,
	_, _, _, b, b,
	_, _, b, b, b,
	b, b, b, b, _,
	_, b, b, _, _,
	b, _, b, _, _,

};

int image_gold[] = {

	5, 5,
	_, yl, yl, yl, _,
	yl, yl, yl, yl, yl,
	yl, yl, yl, yl, yl,
	yl, yl, yl, yl, yl,
	_, yl, yl, yl, _,

};

int image_monster_elf[] = {

	5, 10,
	_, _, _, _, _,
	_, _, _, _, _,
	_, ddg, ddg, ddg, _,
	g, g, g, ddg, _,
	_, g, g, g, _,
	br, br, g, br, br,
	br, br, br, br, br,
	_, br, dbr, dbr, _,
	_, dbr, dbr, dbr, _,
	_, dbr, _, dbr, _,

};

int image_monster_elfBoss[] = {

	5, 10,
	_, _, _, _, _,
	_, ddg, ddg, ddg, _,
	g, g, dg, g, ddg,
	_, g, g, g, _,
	br, br, g, br, br,
	br, br, br, br, br,
	br, br, dbr, dbr, br,
	_, dbr, dbr, dbr, _,
		_, dbr, dbr, dbr, _,
	_, dbr, _, dbr, _,

};

int image_icon_sword[] = {

	6, 6,
	_, _, _, _, gry, gry,
	_, _, _, gry, dgry, gry,
	_, _, gry, dgry, gry, _,
	dgry, dgry, gry, gry, _, _,
	_, dgry, dgry, _, _, _,
	gry, _, dgry, _, _, _


};

int image_icon_armor[] = {

	6, 6,

	_, gry, _, _, gry, _,
	gry, gry, gry, gry, gry, dgry,
	gry, gry, gry, gry, gry, dgry,
	gry, gry, gry, gry, dgry, dgry,
	_, gry, gry, dgry, dgry, _,
	_, gry, dgry, dgry, dgry, _


};

static int* image_files[] = { &image_player_normal, &image_player_wet, &image_player_kneedeep, &image_player_submerged, &image_player_inGrass, &image_shallowwater_seagrass, &image_water_lilypad, &image_deepwater_ripple, &image_grass_flower, &image_lushgrass_fern, &image_sand_shell, &image_treasure_chest, &image_treasure_brokenchest, &image_alert, &image_heart, &image_sword, &image_gold, &image_monster_elf, &image_monster_elfBoss, &image_icon_sword, &image_icon_armor };

static int tile_colors[] = { 0, 0, 0, 0, 0, color_Opacity(0x00BB9944, 0x00004496, 0.5), color_Opacity(0x00BB9944, 0x00004496, 0.75), color_Opacity(0x00BB9944, 0x00004496, 0.85), g, dg, pyl, g, 0, dg, 0 };


static const int TILE_NULL = -1;

static const int PLAYER_NORMAL = 0;
static const int PLAYER_WET = 1;
static const int PLAYER_KNEEDEEP = 2;
static const int PLAYER_SUBMERGED = 3;
static const int PLAYER_INGRASS = 4;
static const int TILE_SHALLOWWATER = 5;
static const int TILE_WATER = 6;
static const int TILE_DEEPWATER = 7;
static const int TILE_GRASS = 8;
static const int TILE_LUSHGRASS = 9;
static const int TILE_SAND = 10;
static const int TILE_TREASURE = 11;
static const int TILE_BROKENTREASURE = 12;
static const int IMAGE_ALERT = 13;
static const int IMAGE_HEART = 14;
static const int IMAGE_SWORD = 15;
static const int IMAGE_GOLD = 16;
static const int IMAGE_MONSTER_ELF = 17;
static const int IMAGE_MONSTER_ELFBOSS = 18;
static const int IMAGE_ICON_SWORD = 19;
static const int IMAGE_ICON_ARMOR = 20;


void frame_DrawImage(int image_id, int image_xPos, int image_yPos, int image_scale, int image_layer) {

	if (image_id < 0) {
		return;
	};

	if (image_scale < 0) {
		image_scale = 2;
	};

	int image_width = image_files[image_id][0];
	int image_height = image_files[image_id][1];

	if (image_layer < 0) {
		image_layer = image_height;
	};

	int image_line = 0;
	int image_column = -1;
	for (int p = 2; p < image_width * image_layer + 2; p++) {

		image_column++;

		if ((p - 2) % image_width == 0) {
			image_line++;
			image_column = 0;
		};

		if (image_files[image_id][p] == -1) {
			continue;
		};

		for (int innerX = 0; innerX < image_scale; innerX++) {

			for (int innerY = 0; innerY < image_scale; innerY++) {

				int x = image_xPos + image_column * image_scale + innerX;

				int y = image_yPos + (image_height - image_line) * image_scale + innerY;

				if (x < 0 || x >= frame.width) {
					continue;
				};

				if (y < 0 || y >= frame.height) {
					continue;
				};

				frame.pixels[y * frame.width + x] = image_files[image_id][p];

			};

		};

	};

	return;
};


void frame_DrawImageFlipped(int image_id, int image_xPos, int image_yPos, int image_scale) {

	if (image_id < 0) {
		return;
	};

	if (image_scale < 0) {
		image_scale = 2;
	};

	int image_width = image_files[image_id][0];
	int image_height = image_files[image_id][1];

	int image_line = 0;
	int image_column = -1;
	for (int p = 2; p < image_width * image_height + 2; p++) {

		image_column++;

		if ((p - 2) % image_width == 0) {
			image_line++;
			image_column = 0;
		};

		if (image_files[image_id][p] == -1) {
			continue;
		};

		for (int innerX = 0; innerX < image_scale; innerX++) {

			for (int innerY = 0; innerY < image_scale; innerY++) {

				int x = image_xPos + (image_width - image_column) * image_scale + innerX;

				int y = image_yPos + (image_height - image_line) * image_scale + innerY;

				if (x < 0 || x >= frame.width) {
					continue;
				};

				if (y < 0 || y >= frame.height) {
					continue;
				};

				frame.pixels[y * frame.width + x] = image_files[image_id][p];

			};

		};

	};

	return;
};


// set map size
#define map_xSize 500
#define map_ySize 500
static int map[map_xSize][map_ySize] = { -1 };
static int map_overlays[map_xSize][map_ySize] = { -1 };




int tile_aroundRadius(int tile_x, int tile_y, int tile_id, int radius) {	// returns how many tiels of tile_id surround the tile in a certain range

	int times = 0;

	for (int x = tile_x - radius; x <= tile_x + radius; x++) {

		if (x < 0 || x >= map_xSize) {
			continue;
		};

		for (int y = tile_y - radius; y <= tile_y + radius; y++) {

			if (y < 0 || y >= map_ySize) {
				continue;
			};

			if ((x == tile_x) && (y == tile_y)) {
				continue;
			};

			if (map[x][y] == tile_id) {
				times++;
			};

		};

	};

	return times;
};


int tile_around(int tile_x, int tile_y, int tile_id) {	// returns how many tile_ids surround the tile

	return tile_aroundRadius(tile_x, tile_y, tile_id, 1);

};





void game_DrawBattle(int game_battle_enemies[3][3], int game_battle_playerHp) {


	for (int p = 0; p < frame.width * frame.height; p++) {

		frame.pixels[p] = 0;

	};


	frame_DrawImage(0, frame.width / 2 - 18, 100, 4, -1);

	frame_DrawImage(IMAGE_HEART, frame.width / 2 + 35, 160, 2, -1);
	char hp[100] = { 0 };
	text_AppendInt(&hp, game_battle_playerHp);
	frame_DrawText(frame.width / 2 + 50, 170, hp, 2, 2, 0x00FF3333);

	frame_DrawImage(IMAGE_SWORD, frame.width / 2 + 35, 145, 2, -1);
	char atk[100] = { 0 };
	text_AppendInt(&atk, plr.stats[1]);
	frame_DrawText(frame.width / 2 + 50, 155, atk, 2, 2, 0x003333FF);


	if (game_battle_enemies[0][0] > 0) {
		frame_DrawImage(game_battle_enemies[0][2], frame.width / 2 - 150, frame.height - 100, 4, -1);

		frame_DrawImage(IMAGE_HEART, frame.width / 2 - 120, frame.height - 80, 2, -1);
		text_ClearString(&hp);
		text_AppendInt(&hp, game_battle_enemies[0][0]);
		frame_DrawText(frame.width / 2 - 105, frame.height - 70, hp, 2, 2, 0x00FF3333);

		frame_DrawImage(IMAGE_SWORD, frame.width / 2 - 120, frame.height - 95, 2, -1);
		text_ClearString(&atk);
		text_AppendInt(&atk, game_battle_enemies[0][1]);
		frame_DrawText(frame.width / 2 - 105, frame.height - 85, atk, 2, 2, 0x003333FF);

	};



	if (game_battle_enemies[2][0] > 0) {
		frame_DrawImage(game_battle_enemies[2][2], frame.width / 2 + 150, frame.height - 100, 4, -1);

		frame_DrawImage(IMAGE_HEART, frame.width / 2 + 180, frame.height - 80, 2, -1);
		text_ClearString(&hp);
		text_AppendInt(&hp, game_battle_enemies[2][0]);
		frame_DrawText(frame.width / 2 + 195, frame.height - 70, hp, 2, 2, 0x00FF3333);

		frame_DrawImage(IMAGE_SWORD, frame.width / 2 + 180, frame.height - 95, 2, -1);
		text_ClearString(&atk);
		text_AppendInt(&atk, game_battle_enemies[2][1]);
		frame_DrawText(frame.width / 2 + 195, frame.height - 85, atk, 2, 2, 0x003333FF);
	};


	if (game_battle_enemies[1][0] > 0) {
		frame_DrawImage(game_battle_enemies[1][2], frame.width / 2, frame.height - 150, 4, -1);

		frame_DrawImage(IMAGE_HEART, frame.width / 2 + 30, frame.height - 130, 2, -1);
		text_ClearString(&hp);
		text_AppendInt(&hp, game_battle_enemies[1][0]);
		frame_DrawText(frame.width / 2 + 45, frame.height - 120, hp, 2, 2, 0x00FF3333);

		frame_DrawImage(IMAGE_SWORD, frame.width / 2 + 30, frame.height - 145, 2, -1);
		text_ClearString(&atk);
		text_AppendInt(&atk, game_battle_enemies[1][1]);
		frame_DrawText(frame.width / 2 + 45, frame.height - 135, atk, 2, 2, 0x003333FF);
	};



	return;
};



char game_message_string[14][100] = { 0 };
int game_message_info[14][2] = { 0 };


void message_AddMessage(char message[100], int color) {

	for (int i = 12; i >= 0; i--) {

		text_ClearString(&game_message_string[i + 1]);
		text_AppendString(game_message_string[i + 1], game_message_string[i]);

		game_message_info[i + 1][1] = game_message_info[i][1];
		game_message_info[i + 1][0] = game_message_info[i][0];

	};

	text_ClearString(&game_message_string[0]);
	text_AppendString(&game_message_string[0], message);

	game_message_info[0][1] = 100;
	game_message_info[0][0] = color;



	return;
};






LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hprevInstance, PWSTR pCmdLine, int nCmdShow) {

	wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASS wc = { };
	wc.lpszClassName = CLASS_NAME;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;

	RegisterClass(&wc);

	frame_bitmapInfo.bmiHeader.biSize = sizeof(frame_bitmapInfo.bmiHeader);
	frame_bitmapInfo.bmiHeader.biPlanes = 1;
	frame_bitmapInfo.bmiHeader.biBitCount = 32;
	frame_bitmapInfo.bmiHeader.biCompression = BI_RGB;
	frame_dc = CreateCompatibleDC(frame_dc);


	DWORD style = WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX;

	int window_width = 400;
	int window_height = 400;

	RECT rect;
	rect.left = 250;
	rect.top = 100;
	rect.bottom = rect.top + window_height;
	rect.right = rect.left + window_width;

	HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"Title", style, rect.left, rect.top, rect.right, rect.bottom, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);


	// generate map

	srand(874532);

	int map_lushgrassAmount = 4;	// higher = more wispy lushgrass formations
	int map_seagrassAmount = 20;
	int map_kelpAmount = 50;
	int map_waterrippleAmount = 50;
	int map_shellAmount = 20;
	int map_fernAmount = 30;
	int map_flowerAmount = 100;
	int map_landSize = 5;	// higher = smaller land masses (> 7, islands)
	int map_oceanSize = 4;	// higher = smaller oceans (< 5, oceans ; >=5, deep lakes)
	int map_deepwaterSize = 4;	// higher = smaller deep water (> 4 starts looking artificial)
	int map_shallowwaterSize = 2;	// higher = smaller deep water (> 4 starts looking artificial)
	int map_islandAmount = 50;
	int map_islandSize = 25;	//higher = smaller islands (25 is best; < gets too big; > gets too scraggly too quickly)
	int map_treasureAmount = 1000;
	int map_monsterCap = 1000;


	for (int i = 0; i < frame.width * frame.height; i++) {
		frame.pixels[i] = 0;
	};
	frame_DrawText(10, 30, "Generating water and land sources..", 2, 2, 0x00FFFFFF);
	InvalidateRect(hWnd, NULL, 0);
	UpdateWindow(hWnd);

	// generate water & tile sources
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			map[x][y] = TILE_SHALLOWWATER;

			if (rand() % 200) {
				continue;
			};

			map[x][y] = TILE_GRASS;

		};

	};


	for (int i = 0; i < frame.width * frame.height; i++) {
		frame.pixels[i] = 0;
	};
	frame_DrawText(10, 30, "Generating grass..", 2, 2, 0x00FFFFFF);
	InvalidateRect(hWnd, NULL, 0);
	UpdateWindow(hWnd);


	// generate grass
	for (int p = 0; p < 11; p++) {

		for (int i = 0; i < frame.width * frame.height; i++) {
			frame.pixels[i] = 0;
		};
		char temp_text[100] = { 0 };
		text_AppendString(&temp_text, "Generating grass.. pass ");
		text_AppendInt(&temp_text, (p + 1));
		text_AppendString(&temp_text, " / 12");
		frame_DrawText(10, 30, temp_text, 2, 2, 0x00FFFFFF);
		InvalidateRect(hWnd, NULL, 0);
		UpdateWindow(hWnd);

		for (int x = 0; x < map_xSize; x++) {

			for (int y = 0; y < map_ySize; y++) {

				if ((map[x][y] == TILE_SHALLOWWATER) && tile_around(x, y, TILE_SHALLOWWATER) < 2) {
					map[x][y] = TILE_GRASS;
				};


				if (tile_around(x, y, TILE_GRASS) == 4) {
					map[x][y] = TILE_GRASS;
				};

				if (rand() % map_landSize) {
					continue;
				};

				if (tile_around(x, y, TILE_GRASS) > 0) {
					map[x][y] = TILE_GRASS;
				};


			};

		};

	};


	//clean up water spots
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (tile_around(x, y, TILE_GRASS) > 4) {

				map[x][y] = TILE_GRASS;

			};

		};

	};


	for (int i = 0; i < frame.width * frame.height; i++) {
		frame.pixels[i] = 0;
	};
	frame_DrawText(10, 30, "Generating shallow water..", 2, 2, 0x00FFFFFF);
	InvalidateRect(hWnd, NULL, 0);
	UpdateWindow(hWnd);


	// generate water
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (!(rand() % 5000)) {
				map[x][y] = TILE_NULL;
			};

		};

	};

	for (int p = 0; p < 10; p++) {

		for (int x = 0; x < map_xSize; x++) {

			for (int y = 0; y < map_ySize; y++) {

				if (tile_around(x, y, TILE_NULL) == 0) {
					continue;
				};

				if (tile_around(x, y, TILE_NULL) > 3) {
					map[x][y] = TILE_NULL;
				};

				if (rand() % map_oceanSize) {
					continue;
				};

				map[x][y] = TILE_NULL;

			};

		};

	};

	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_NULL) {
				continue;
			};

			map[x][y] = TILE_SHALLOWWATER;


		};

	};


	for (int i = 0; i < frame.width * frame.height; i++) {
		frame.pixels[i] = 0;
	};
	frame_DrawText(10, 30, "Generating water..", 2, 2, 0x00FFFFFF);
	InvalidateRect(hWnd, NULL, 0);
	UpdateWindow(hWnd);


	// generate water inside shallowwater
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_SHALLOWWATER) {
				continue;
			};

			if (tile_aroundRadius(x, y, TILE_GRASS, map_shallowwaterSize) + tile_aroundRadius(x, y, TILE_LUSHGRASS, map_shallowwaterSize) > 0) {
				continue;
			};

			map[x][y] = TILE_WATER;


		};

	};


	for (int i = 0; i < frame.width * frame.height; i++) {
		frame.pixels[i] = 0;
	};
	frame_DrawText(10, 30, "Generating deep water..", 2, 2, 0x00FFFFFF);
	InvalidateRect(hWnd, NULL, 0);
	UpdateWindow(hWnd);


	// generate deep water inside water
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_WATER) {
				continue;
			};

			if (tile_aroundRadius(x, y, TILE_GRASS, map_deepwaterSize) + tile_aroundRadius(x, y, TILE_LUSHGRASS, map_deepwaterSize) + tile_aroundRadius(x, y, TILE_SHALLOWWATER, map_deepwaterSize) > 0) {
				continue;
			};

			map[x][y] = TILE_DEEPWATER;


		};

	};


	for (int i = 0; i < frame.width * frame.height; i++) {
		frame.pixels[i] = 0;
	};
	frame_DrawText(10, 30, "Generating islands..", 2, 2, 0x00FFFFFF);
	InvalidateRect(hWnd, NULL, 0);
	UpdateWindow(hWnd);


	// generate islands inside deep water
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_DEEPWATER) {
				continue;
			};

			if (tile_aroundRadius(x, y, TILE_SAND, 10) + tile_aroundRadius(x, y, TILE_GRASS, 10) > 0) {
				continue;
			};


			if (!(rand() % map_islandAmount)) {
				map[x][y] = TILE_GRASS;
			};

		};

	};

	for (int p = 0; p < 20; p++) {

		for (int x = 0; x < map_xSize; x++) {

			for (int y = 0; y < map_ySize; y++) {

				if (map[x][y] != TILE_DEEPWATER) {
					continue;
				};


				if (tile_around(x, y, TILE_GRASS) == 0) {
					continue;
				};

				if (!(rand() % map_islandSize)) {
					map[x][y] = TILE_GRASS;
				};

			};

		};

	};

	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_DEEPWATER) {
				continue;
			};

			if (tile_around(x, y, TILE_GRASS) < 4) {
				continue;
			};

			map[x][y] = TILE_GRASS;

		};

	};


	for (int i = 0; i < frame.width * frame.height; i++) {
		frame.pixels[i] = 0;
	};
	frame_DrawText(10, 30, "Generating tile overlays..", 2, 2, 0x00FFFFFF);
	InvalidateRect(hWnd, NULL, 0);
	UpdateWindow(hWnd);


	// generate seagrass
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_SHALLOWWATER) {
				continue;
			};

			if (!(rand() % map_seagrassAmount)) {
				map_overlays[x][y] = TILE_SHALLOWWATER;
			};

		};

	};


	// generate kelp
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_WATER) {
				continue;
			};


			if (!(rand() % map_kelpAmount)) {
				map_overlays[x][y] = TILE_WATER;
			};


		};

	};


	// generate water ripples
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_DEEPWATER) {
				continue;
			};

			if (!(rand() % map_waterrippleAmount)) {
				map_overlays[x][y] = TILE_DEEPWATER;
			};

		};

	};


	//generate flowers
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] == TILE_DEEPWATER) {
				continue;
			};

			if (map[x][y] == TILE_WATER) {
				continue;
			};

			if (map[x][y] == TILE_SHALLOWWATER) {
				continue;
			};

			if (!(rand() % map_flowerAmount)) {
				map_overlays[x][y] = TILE_GRASS;
			};

		};

	};


	// generate sand
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_GRASS) {
				continue;
			};

			if (tile_around(x, y, TILE_SHALLOWWATER) + tile_around(x, y, TILE_WATER) + tile_around(x, y, TILE_DEEPWATER) == 0) {
				continue;
			};

			map[x][y] = TILE_SAND;

			if (!(rand() % map_shellAmount)) {
				map_overlays[x][y] = TILE_SAND;
			};

		};

	};


	for (int i = 0; i < frame.width * frame.height; i++) {
		frame.pixels[i] = 0;
	};
	frame_DrawText(10, 30, "Generating lush grass..", 2, 2, 0x00FFFFFF);
	InvalidateRect(hWnd, NULL, 0);
	UpdateWindow(hWnd);


	// generate lush grass

	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_GRASS) {
				continue;
			};

			if (rand() % 150) {
				continue;
			};

			map[x][y] = TILE_LUSHGRASS;

		};

	};

	for (int p = 0; p < 15; p++) {

		for (int x = 0; x < map_xSize; x++) {

			for (int y = 0; y < map_ySize; y++) {

				if (map[x][y] != TILE_GRASS) {
					continue;
				};

				if (tile_around(x, y, TILE_LUSHGRASS) == 0) {
					continue;
				};

				if (rand() % 10) {
					continue;
				};

				map[x][y] = TILE_LUSHGRASS;

			};

		};

	};

	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_GRASS) {
				continue;
			};

			if (tile_around(x, y, TILE_LUSHGRASS) < 5) {
				continue;
			};

			map[x][y] = TILE_LUSHGRASS;

		};

	};

	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {

			if (map[x][y] != TILE_LUSHGRASS) {
				continue;
			};

			if (!(rand() % map_fernAmount)) {
				map_overlays[x][y] = TILE_LUSHGRASS;
			};

		};

	};


	for (int i = 0; i < frame.width * frame.height; i++) {
		frame.pixels[i] = 0;
	};
	frame_DrawText(10, 30, "Generating treasure..", 2, 2, 0x00FFFFFF);
	InvalidateRect(hWnd, NULL, 0);
	UpdateWindow(hWnd);


	//generate treasure
	for (int x = 0; x < map_xSize; x++) {

		for (int y = 0; y < map_ySize; y++) {


			if (map[x][y] != TILE_GRASS) {
				continue;
			};

			if (!(rand() % map_treasureAmount)) {
				map[x][y] = TILE_TREASURE;
				map_overlays[x][y] = TILE_TREASURE;
			};


		};

	};


	//map display settings
	int tile_size = 3;
	int tile_despawnBuffer = 0;	// > 0, when tiles should despawn


	//set player spawn
	plr.x = map_xSize / 2.0 * (5 * tile_size);
	plr.y = map_ySize / 2.0 * (5 * tile_size);

	int found = 0;

	for (int x = map_xSize / 2; x < map_xSize; x++) {

		for (int y = map_ySize / 2; y < map_ySize; y++) {

			if (map[x][y] == TILE_GRASS) {

				plr.x = x * (5 * tile_size);
				plr.y = y * (5 * tile_size);

				found = 1;
				break;

			};

		};

		if (found) {
			break;
		};

	};

	#define wordList_adjectives_maxLength 11
	#define wordList_nouns_maxLength 12


	text_AppendString(&wordList_adjectives[0], "Basic");
	text_AppendString(&wordList_adjectives[1], "Strange");
	text_AppendString(&wordList_adjectives[2], "Fantastic");
	text_AppendString(&wordList_adjectives[3], "Crude");
	text_AppendString(&wordList_adjectives[4], "Unnatural");
	text_AppendString(&wordList_adjectives[5], "Terrifying");
	text_AppendString(&wordList_adjectives[6], "Awful");
	text_AppendString(&wordList_adjectives[7], "Golden");
	text_AppendString(&wordList_adjectives[8], "Wizened");
	text_AppendString(&wordList_adjectives[9], "Forgotten");
	text_AppendString(&wordList_adjectives[10], "Corrupted");


	text_AppendString(&wordList_nouns[0], "Vengeance");
	text_AppendString(&wordList_nouns[1], "the Brave");
	text_AppendString(&wordList_nouns[2], "Destruction");
	text_AppendString(&wordList_nouns[3], "Ba'lor");
	text_AppendString(&wordList_nouns[4], "Kohran");
	text_AppendString(&wordList_nouns[5], "the Pyramids");
	text_AppendString(&wordList_nouns[6], "the Elves");
	text_AppendString(&wordList_nouns[7], "Ages");
	text_AppendString(&wordList_nouns[8], "Scyntheus");
	text_AppendString(&wordList_nouns[9], "Aldruis");
	text_AppendString(&wordList_nouns[10], "the Cavedwellers");
	text_AppendString(&wordList_nouns[11], "the Hidden");


	message_AddMessage("Welcome!", 0x00777777);

	int game_battle_in = 0;
	int game_battle_difficulty = 0;

	int game_inventory_open = 0;

	int game_battle_playerHp = plr.stats[0];
	int game_battle_enemies[][3] = {	// { hp, atk, icon}
		{0, 0, -1},
		{0, 0, -1},
		{0, 0, -1}
	};

	
	plr.gear_stats[0][0] = 8;
	plr.gear_stats[0][1] = 0;

	plr.gear_stats[1][0] = 0;
	plr.gear_stats[1][1] = 4;

	text_AppendString(&plr.gear_names[0], "Basic Plate");
	text_AppendString(&plr.gear_names[1], "Basic Sword");


	plr.stats[0] = 0;
	plr.stats[1] = 0;
	for (int i = 0; i < 2; i++) {

		plr.stats[0] += plr.gear_stats[i][0];
		plr.stats[1] += plr.gear_stats[i][1];

	};



	int inv_counter1 = 0;
	int inv_counter2 = 0;
	
	MSG msg = { };
	while (running == 1) {


		while (PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE)) {
		};

		TranslateMessage(&msg);
		DispatchMessage(&msg);


		for (int i = 0; i < 128; i++) {
			keys[i] = 0;
			if (GetKeyState(i) / 10) {
				keys[i] = 1;
			};
		};

		if (keys[27]) {	//esc
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		};



		// inventory open/close
		if (keys[69] && !game_inventory_open && (inv_counter2 > 10)) {
			game_inventory_open = 1;
			keys[69] = 0;
			inv_counter1 = 0;
		};

		if (keys[69] && game_inventory_open && (inv_counter1 > 10)) {
			game_inventory_open = 0;
			inv_counter2 = 0;
		};
	
		inv_counter1++;
		inv_counter2++;
		


		// tile effects
		plr.icon = 0;
		plr.speed = 3;
		switch (map[plr_tile_x][plr_tile_y]) {

		case (5): {	//TILE_SHALLOWWATER
			plr.icon = 1;
			plr.speed -= 1.5;
			break;
		};

		case (6): {	//TILE_WATER
			plr.icon = 2;
			plr.speed -= 2;
			break;
		};

		case (7): {	//TILE_DEEPWATER
			plr.icon = 3;
			plr.speed -= 2.5;
			break;
		};

		case (11): {	//TILE_TREASURE

			int reward = 1;
			for (int i = 0; i < 10; i++) {

				if (rand() % 2) {
					break;
				};
				reward += i;

			};

			plr.gold += reward;
			map[plr_tile_x][plr_tile_y] = TILE_GRASS;
			map_overlays[plr_tile_x][plr_tile_y] = TILE_BROKENTREASURE;

			char msg[string_maxLength] = { 0 };
			msg[0] = '+';
			text_AppendInt(&msg, reward);
			text_AppendString(&msg, " gold");
			message_AddMessage(&msg, yl);

			break;

		};

		};




		//process input
		if (keys[65] || GetAsyncKeyState(VK_LEFT)) {	//A or leftArrow
			plr.dir += 5;
		};

		if (keys[68] || GetAsyncKeyState(VK_RIGHT)) {	//D or rightArrow
			plr.dir -= 5;
		};

		plr_tile_x = plr.x / (5 * tile_size);
		plr_tile_y = plr.y / (5 * tile_size);






		// lost battle
		if (game_battle_in && game_battle_playerHp <= 0) {

			message_AddMessage("Tough loss..", r);

			if (plr.gold > 0) {

				char s_goldLoss[100] = { 0 };				
				int goldLoss = rand() % 5;

				s_goldLoss[0] = '-';
				text_AppendInt(&s_goldLoss, goldLoss);
				text_AppendString(&s_goldLoss, " gold");

				message_AddMessage(s_goldLoss, dr);
				plr.gold -= 1;
			};
			
			game_battle_in = 0;
		};

		// won battle
		if (game_battle_in && (game_battle_enemies[0][0] <= 0 && game_battle_enemies[1][0] <= 0 && game_battle_enemies[2][0] <= 0)) {

			int reward = 1;

			for (int i = 0; i < 10; i++) {

				if (!(rand() % 3)) {
					break;
				};

				reward += i;

			};

				// gold reward
			plr.gold += reward;
			char msg[100] = { 0 };
			msg[0] = '+';
			text_AppendInt(&msg, reward);
			text_AppendString(&msg, " gold");
			message_AddMessage(msg, yl);


			char item_name_temp[100] = { 0 };
			

				// sword reward
			if (!(rand() % (10 / game_battle_difficulty))) {

				text_AppendString(item_name_temp, wordList_adjectives[rand() % wordList_adjectives_maxLength]);
				text_AppendString(item_name_temp, " Plate of ");
				text_AppendString(item_name_temp, wordList_nouns[rand() % wordList_nouns_maxLength]);

				text_ClearString(&plr.gear_names[0]);
				text_AppendString(&plr.gear_names[0], item_name_temp);

				plr.gear_stats[0][0] += rand() % (10 + game_battle_difficulty) + 1;	// boost hp
				
				text_ClearString(&msg);
				msg[0] = '+';
				text_AppendString(&msg, plr.gear_names[0]);
				message_AddMessage(msg, db);

			};
			
				// plate reward
			if (!(rand() % (10 / game_battle_difficulty))) {

				text_ClearString(&item_name_temp);
				text_AppendString(item_name_temp, wordList_adjectives[rand() % wordList_adjectives_maxLength]);
				text_AppendString(item_name_temp, " Sword of ");
				text_AppendString(item_name_temp, wordList_nouns[rand() % wordList_nouns_maxLength]);

				text_ClearString(&plr.gear_names[1]);
				text_AppendString(&plr.gear_names[1], item_name_temp);

				plr.gear_stats[1][1] += rand() % (4 + game_battle_difficulty) + 1;	// boost attack
				
				text_ClearString(&msg);
				msg[0] = '+';
				text_AppendString(&msg, plr.gear_names[1]);
				message_AddMessage(msg, db);

			};


			plr.stats[0] = 0;
			plr.stats[1] = 0;
			for (int i = 0; i < 2; i++) {

				plr.stats[0] += plr.gear_stats[i][0];
				plr.stats[1] += plr.gear_stats[i][1];

			};

;

			game_battle_in = 0;

		};



		if (game_battle_in) {


			// player turn
			game_DrawBattle(game_battle_enemies, game_battle_playerHp);

			char battle_difficulty[100] = { 0 };
			text_AppendString(&battle_difficulty, "Difficulty: ");
			text_AppendInt(&battle_difficulty, game_battle_difficulty);
			frame_DrawText(10, 15, &battle_difficulty, 1, 1, gry);

			char text_attackPrompt[100] = { 0 };

			text_AppendString(&text_attackPrompt, "Which monster to attack? ");

			if (game_battle_enemies[0][0] > 0) {
				text_AppendString(&text_attackPrompt, "[1] ");
			};

			if (game_battle_enemies[1][0] > 0) {
				text_AppendString(&text_attackPrompt, "[2] ");
			};

			if (game_battle_enemies[2][0] > 0) {
				text_AppendString(&text_attackPrompt, "[3]");
			};

			frame_DrawText(frame.width / 2 - text_GetSize(text_attackPrompt) * 6, 50, text_attackPrompt, 2, 2, -1);

			InvalidateRect(hWnd, NULL, 0);
			UpdateWindow(hWnd);

			if (!((keys[49] && game_battle_enemies[0][0] > 0) || (keys[51] && game_battle_enemies[2][0] > 0) || (keys[50] && game_battle_enemies[1][0] > 0))) {
				continue;
			};

			game_DrawBattle(game_battle_enemies, game_battle_playerHp);

			for (double h = 0.1; h <= 1; h += 0.1) {

				game_DrawBattle(game_battle_enemies, game_battle_playerHp);

				frame_DrawRectFilled(frame.width / 2 - 30, 90, frame.width / 2 + 35, 190, 0);
				frame_DrawImage(0, frame.width / 2 - 18, 100 + 7.0 * sin(3.14 * h), 4, -1);

				int height_offset = (int)(3.0 * sin(3.14 * h));

				if (keys[49]) {
					frame_DrawRectFilled(frame.width / 2 - 180, frame.height - 25, frame.width / 2 - 120, frame.height - 150, 0);
					frame_DrawImage(game_battle_enemies[0][2], frame.width / 2 - 150, frame.height - 100 + height_offset, 4, -1);

				};

				if (keys[51]) {
					frame_DrawRectFilled(frame.width / 2 + 120, frame.height - 25, frame.width / 2 + 180, frame.height - 150, 0);
					frame_DrawImage(game_battle_enemies[2][2], frame.width / 2 + 150, frame.height - 100 + height_offset, 4, -1);
				};

				if (keys[50]) {
					frame_DrawRectFilled(frame.width / 2 - 50, frame.height - 75, frame.width / 2 + 30, frame.height - 200, 0);
					frame_DrawImage(game_battle_enemies[1][2], frame.width / 2, frame.height - 150 + height_offset, 4, -1);
				};

				if (h < 0.5) {
					InvalidateRect(hWnd, NULL, 0);
					UpdateWindow(hWnd);
					Sleep(25);
					continue;
				};

				if (keys[49]) {
					char atk_dmg[100] = { 0 };
					text_AppendInt(&atk_dmg, -1 * plr.stats[1]);
					frame_DrawText(frame.width / 2 - 150, frame.height - 100, atk_dmg, 2, 2, 0x0000FF00);
				};

				if (keys[51]) {
					char atk_dmg[100] = { 0 };
					text_AppendInt(&atk_dmg, -1 * plr.stats[1]);
					frame_DrawText(frame.width / 2 + 150, frame.height - 100, atk_dmg, 2, 2, 0x0000FF00);
				};

				if (keys[50]) {
					char atk_dmg[100] = { 0 };
					text_AppendInt(&atk_dmg, -1 * plr.stats[1]);
					frame_DrawText(frame.width / 2, frame.height - 150, atk_dmg, 2, 2, 0x0000FF00);
				};


				if (h != 0.5) {
					InvalidateRect(hWnd, NULL, 0);
					UpdateWindow(hWnd);
					Sleep(25);
					continue;
				};

				if (keys[49]) {
					game_battle_enemies[0][0] -= plr.stats[1];
				};

				if (keys[51]) {
					game_battle_enemies[2][0] -= plr.stats[1];
				};

				if (keys[50]) {
					game_battle_enemies[1][0] -= plr.stats[1];
				};



				InvalidateRect(hWnd, NULL, 0);
				UpdateWindow(hWnd);
				Sleep(25);

			};


			Sleep(500);



			// enemy turn
			for (int enemy = 0; enemy < 3; enemy++) {

				if (game_battle_enemies[enemy][0] <= 0) {
					continue;
				};

				game_DrawBattle(game_battle_enemies, game_battle_playerHp);

				for (double h = 0.1; h <= 1; h += 0.1) {

					int height_offset = (int)(-7.0 * sin(3.14 * h));


					if (enemy == 0) {
						frame_DrawRectFilled(frame.width / 2 - 180, frame.height - 25, frame.width / 2 - 120, frame.height - 150, 0);
						frame_DrawImage(game_battle_enemies[enemy][2], frame.width / 2 - 150, frame.height - 100 + height_offset, 4, -1);
					};

					if (enemy == 2) {
						frame_DrawRectFilled(frame.width / 2 + 120, frame.height - 25, frame.width / 2 + 180, frame.height - 150, 0);
						frame_DrawImage(game_battle_enemies[enemy][2], frame.width / 2 + 150, frame.height - 100 + height_offset, 4, -1);
					};

					if (enemy == 1) {
						frame_DrawRectFilled(frame.width / 2 - 50, frame.height - 75, frame.width / 2 + 30, frame.height - 200, 0);
						frame_DrawImage(game_battle_enemies[enemy][2], frame.width / 2, frame.height - 150 + height_offset, 4, -1);
					};


					frame_DrawRectFilled(frame.width / 2 - 30, 90, frame.width / 2 + 35, 190, 0);
					frame_DrawImage(0, frame.width / 2 - 18, 100 - 3.0 * sin(3.14 * h), 4, -1);


					if (h < 0.1) {
						InvalidateRect(hWnd, NULL, 0);
						UpdateWindow(hWnd);
						Sleep(25);
						continue;
					};

					if (enemy == 0) {
						char atk_dmg[100] = { 0 };
						text_AppendInt(&atk_dmg, -1 * game_battle_enemies[enemy][1]);
						frame_DrawText(frame.width / 2 - 50, 200, atk_dmg, 2, 2, 0x00FF5555);
					};

					if (enemy == 2) {
						char atk_dmg[100] = { 0 };
						text_AppendInt(&atk_dmg, -1 * game_battle_enemies[enemy][1]);
						frame_DrawText(frame.width / 2 + 50, 200, atk_dmg, 2, 2, 0x00FF5555);
					};

					if (enemy == 1) {
						char atk_dmg[100] = { 0 };
						text_AppendInt(&atk_dmg, -1 * game_battle_enemies[enemy][1]);
						frame_DrawText(frame.width / 2, 210, atk_dmg, 2, 2, 0x00FF5555);
					};

					if (h != 0.1) {
						InvalidateRect(hWnd, NULL, 0);
						UpdateWindow(hWnd);
						Sleep(25);
						continue;
					};

					if (enemy == 0) {
						game_battle_playerHp -= game_battle_enemies[enemy][1];
					};

					if (enemy == 2) {
						game_battle_playerHp -= game_battle_enemies[enemy][1];
					};

					if (enemy == 1) {
						game_battle_playerHp -= game_battle_enemies[enemy][1];
					};

					game_DrawBattle(game_battle_enemies, game_battle_playerHp);

					InvalidateRect(hWnd, NULL, 0);
					UpdateWindow(hWnd);
					Sleep(25);

				};

				Sleep(200);
			};

		};


		if (game_battle_in) {
			InvalidateRect(hWnd, NULL, 0);
			UpdateWindow(hWnd);
			Sleep(20);
			continue;
		};

		// start battle - spawn enemies
		game_battle_in = 1;
		
		if (map[plr_tile_x][plr_tile_y] != TILE_LUSHGRASS) {
			game_battle_in = 0;
		};

		if (!(keys[87] || keys[83] || GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VK_DOWN))) {
			game_battle_in = 0;
		};

		if (rand() % 50) {
			game_battle_in = 0;
		};

		// set enemy & player stats
		if (game_battle_in) {

			game_battle_difficulty = 0;
			game_battle_playerHp = plr.stats[0];

			for (int enemy = 0; enemy < 3; enemy++) {

				game_battle_enemies[enemy][0] = 0;
				game_battle_enemies[enemy][1] = 0;
				game_battle_enemies[enemy][2] = -1;

				if (rand() % 2) {	// spawn normal enemy
					continue;
				};

				game_battle_enemies[enemy][0] = plr.stats[0] * 0.5 * (0.5 + (rand() % 10) / 10.0) + 1;
				game_battle_enemies[enemy][1] = game_battle_enemies[enemy][0] / 4.0 * (0.5 + (rand() % 10) / 10.0) + 1;
				game_battle_enemies[enemy][2] = IMAGE_MONSTER_ELF;
				
				game_battle_difficulty += 1;
				
				if (rand() % 3) {	// spawn boss
					continue;
				};

				game_battle_enemies[enemy][0] = plr.stats[0] * 1.0 * (0.5 + (rand() % 10) / 10.0) + 1;
				game_battle_enemies[enemy][1] = game_battle_enemies[enemy][0] / 4.0 * (0.5 + (rand() % 10) / 10.0) + 1;
				game_battle_enemies[enemy][2] = IMAGE_MONSTER_ELFBOSS;
				
				game_battle_difficulty += 2;

			};

			if (game_battle_enemies[0][0] == 0 && game_battle_enemies[1][0] == 0 && game_battle_enemies[2][0] == 0) {	// mega boss when no other mob spawns
				
				game_battle_enemies[1][0] = plr.stats[0] * 1.2 * (0.5 + (rand() % 10) / 10.0) + 1;
				game_battle_enemies[1][1] = game_battle_enemies[1][0] / 3.0 * (0.5 + (rand() % 10) / 10.0) + 1;
				game_battle_enemies[1][2] = IMAGE_MONSTER_ELFBOSS;

				game_battle_difficulty += 3;


			};

		};



		// battle start animation
		if (game_battle_in) {

			frame_DrawImage(IMAGE_ALERT, frame.width / 2 - 7, frame.height / 2 + 20, 2, -1);

			InvalidateRect(hWnd, NULL, 0);
			UpdateWindow(hWnd);

			int monster_direction = -2;

			if ((NormalizeAngle(plr.dir) < 90 && NormalizeAngle(plr.dir) >= 0) || (NormalizeAngle(plr.dir) > 270)) {

				monster_direction = 2;

			};

			for (int i = 0; i < 11; i++) {

				int tile_x = (plr_tile_x + monster_direction) * (5 * tile_size) + frame.width / 2;

				int tile_y = plr_tile_y * (5 * tile_size) + frame.height / 2;

				frame_DrawRectFilled(tile_x - plr.x, tile_y - plr.y, tile_x - plr.x + 5 * tile_size, tile_y - plr.y + 5 * tile_size, tile_colors[map[plr_tile_x + monster_direction][plr_tile_y]]);

				tile_y = (plr_tile_y + 1) * (5 * tile_size) + frame.height / 2;

				frame_DrawRectFilled(tile_x - plr.x, tile_y - plr.y, tile_x - plr.x + 5 * tile_size, tile_y - plr.y + 5 * tile_size, tile_colors[map[plr_tile_x + monster_direction][plr_tile_y + 1]]);

				tile_y = (plr_tile_y) * (5 * tile_size) + frame.height / 2;

				frame_DrawImage(IMAGE_MONSTER_ELF, tile_x - plr.x, tile_y - plr.y - ((10 - i) * tile_size), tile_size, i);
				InvalidateRect(hWnd, NULL, 0);
				UpdateWindow(hWnd);

				Sleep(50);

			};

			Sleep(500);

			int count = 0;
			while (count < 100) {
				PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE);
				count++;
			};

			for (int i = 0; i < frame.height / 2 + frame.height / 5; i += frame.height / 10) {

				frame_DrawRectFilled(0, frame.height, frame.width, frame.height - i, 0);
				frame_DrawRectFilled(0, 0, frame.width, i, 0);

				InvalidateRect(hWnd, NULL, 0);
				UpdateWindow(hWnd);

				Sleep(5);

			};

			Sleep(500);

			continue;
		};




		if (keys[87] || GetAsyncKeyState(VK_UP)) {	//W	or upArrow
			plr.x += plr.speed * 1.0 * cos(1.0 * plr.dir * 3.14 / 180);
			plr.y += plr.speed * 1.0 * sin(1.0 * plr.dir * 3.14 / 180);
		};

		if (keys[83] || GetAsyncKeyState(VK_DOWN)) {	//S	or downArrow
			plr.x -= plr.speed * 1.0 * cos(plr.dir * 3.14 / 180);
			plr.y -= plr.speed * 1.0 * sin(plr.dir * 3.14 / 180);
		};


		//fill screen
		for (int i = 0; i < frame.width * frame.height; i++) {
			frame.pixels[i] = 0;
		};



		// draw map
		for (int x = plr_tile_x - frame.width / (2 * 5 * tile_size) - (5 * tile_size) + tile_despawnBuffer; x < plr_tile_x + frame.width / (2 * 5 * tile_size) + (5 * tile_size) - tile_despawnBuffer; x++) {

			for (int y = plr_tile_y - frame.height / (2 * 5 * tile_size) - (5 * tile_size) + tile_despawnBuffer; y < plr_tile_y + frame.height / (2 * 5 * tile_size) + (5 * tile_size) - tile_despawnBuffer; y++) {

				if (x < 0 || x >= map_xSize) {
					continue;
				};

				if (y < 0 || y >= map_ySize) {
					continue;
				}

				if (map[x][y] == 0) {
					continue;
				};

				int tile_x = x * (5 * tile_size) + frame.width / 2;
				int tile_y = y * (5 * tile_size) + frame.height / 2;

				int tile_color = tile_colors[map[x][y]];

				frame_DrawRectFilledInline(tile_x - plr.x, tile_y - plr.y, tile_x - plr.x + (5 * tile_size), tile_y - plr.y + (5 * tile_size), tile_color);

				if (!map_overlays[x][y]) {
					continue;
				};

				//draw tile overlays
				frame_DrawImage(map_overlays[x][y], tile_x - plr.x, tile_y - plr.y, tile_size, -1);

			};

		};



		// draw player
		int image_scale = 2;
		frame_DrawImage(plr.icon, frame.width / 2 - (image_files[0][0] * image_scale) / 2.0, frame.height / 2 - (image_files[0][1] * image_scale) / 2.0, image_scale, -1);
		frame_DrawRectAFilled(frame.width / 2, frame.height / 2, -4, 30, plr.dir, 8, 14, 0x00FF0000);
		frame_DrawRectAFilled(frame.width / 2, frame.height / 2, -2, 30, plr.dir, 4, 16, 0x00FF0000);


		// display text
		frame_DrawRectFilled(5, 5, 75, 55, 0x00111111);
		frame_DrawRectFilled(10, 10, 80, 60, 0x00333333);

		frame_DrawImage(IMAGE_GOLD, 15, 45, 2, -1);
		char gold[100] = { 0 };
		text_AppendInt(&gold, plr.gold);
		frame_DrawText(30, 55, gold, 2, 2, 0x00FFFF33);

		frame_DrawText(10, 40, "[E] for Inv", 1, 1, lgry);

		frame_DrawText(10, 30, "[Esc] exit", 1, 1, lgry);

		/*
		frame_DrawImage(IMAGE_HEART, 15, 30, 2, -1);
		char hp[100] = { 0 };
		text_AppendInt(&hp, plr.stats[0]);
		frame_DrawText(30, 40, hp, 2, 2, 0x00FF3333);

		frame_DrawImage(IMAGE_SWORD, 15, 15, 2, -1);
		char atk[100] = { 0 };
		text_AppendInt(&atk, plr.stats[1]);
		frame_DrawText(30, 25, atk, 2, 2, 0x003333FF);
		*/

		frame_DrawRectFilled(5, frame.height - 15, 155, frame.height - 50, 0x00111111);
		frame_DrawRectFilled(10, frame.height - 10, 160, frame.height - 45, 0x00333333);

		char pos[100] = { 0 };
		text_AppendInt(&pos, plr.x);
		text_AppendString(&pos, ",");
		text_AppendInt(&pos, plr.y);
		frame_DrawText(15, frame.height - 15, pos, 2, 2, 0x00555555);

		char tilePos[100] = { 0 };
		text_AppendInt(&tilePos, plr_tile_x);
		text_AppendString(&tilePos, ",");
		text_AppendInt(&tilePos, plr_tile_y);
		frame_DrawText(15, frame.height - 30, tilePos, 2, 2, 0x00555555);

		frame_DrawText(frame.width - 80, 10, "Andrzej Z.P.", 1, 1, 0x00FFFFFF);




		for (int message = 0; message < 14; message++) {

			int yShift = message * 25;

			game_message_info[message][1] -= 1;
			int message_timer = game_message_info[message][1];

			if (message_timer <= 0) {
				continue;
			};

			if (message_timer > 90) {
				frame_DrawRectFilled(8, 82 + yShift, 10 + (text_GetSize(game_message_string[message]) + 1) * 12 / abs(message_timer - 90), 68 + yShift, color_Darken(game_message_info[message][0], 50));
				frame_DrawRectFilled(8, 68 + yShift, 10 + (text_GetSize(game_message_string[message]) + 1) * 12 / abs(message_timer - 90), 65 + yShift, game_message_info[message][0]);
				continue;
			};

			if (message_timer >= 20) {
				frame_DrawRectFilled(8, 82 + yShift, 10 + (text_GetSize(game_message_string[message]) + 1) * 12, 68 + yShift, color_Darken(game_message_info[message][0], 50));
				frame_DrawRectFilled(8, 68 + yShift, 10 + (text_GetSize(game_message_string[message]) + 1) * 12, 65 + yShift, game_message_info[message][0]);
			};

			if (message_timer < 20) {
				frame_DrawRectFilled(8, 82 + yShift, 10 + (text_GetSize(game_message_string[message]) + 1) * 12 / abs((20 - message_timer) * (20 - message_timer)), 68 + yShift, color_Darken(game_message_info[message][0], 50));
				frame_DrawRectFilled(8, 68 + yShift, 10 + (text_GetSize(game_message_string[message]) + 1) * 12 / abs((20 - message_timer) * (20 - message_timer)), 65 + yShift, game_message_info[message][0]);
			};

			if (message_timer > 35) {
				frame_DrawText(10, 79 + yShift, game_message_string[message], 2, 2, game_message_info[message][0]);
				continue;
			};

			if (message_timer >= 20) {
				frame_DrawText(10, 79 + yShift, game_message_string[message], 2, 2, color_Opacity(color_Darken(game_message_info[message][0], 50), game_message_info[message][0], (message_timer - 20) / 15.0));
				continue;
			};

		};

		char atk[100] = { 0 };
		char hp[100] = { 0 };


			// draw inventory
		if (game_inventory_open == 1) {

			frame_DrawRectFilled(frame.width / 2 - 155, frame.height / 2 - 155, frame.width / 2 + 145, frame.height / 2 + 145, 0x00111111);
			frame_DrawRectFilled(frame.width / 2 - 150, frame.height / 2 - 150, frame.width / 2 + 150, frame.height / 2 + 150, 0x00333333);
			frame_DrawText(frame.width / 2 - 140, frame.height / 2 + 135, "Inventory", 3, 3, 0x00555555);

				// exit button
			frame_DrawRectFilled(frame.width / 2 + 97, frame.height / 2 + 117, frame.width / 2 + 137, frame.height / 2 + 137, 0x00AA3333);
			frame_DrawRectFilled(frame.width / 2 + 100, frame.height / 2 + 120, frame.width / 2 + 140, frame.height / 2 + 140, 0x00FF3333);
			frame_DrawText(frame.width /2 + 103, frame.height / 2 + 135, "[E]", 2, -1, 0x00AA3333);
			

			
			frame_DrawRectFilled(frame.width / 2 - 145, frame.height / 2 + 105, frame.width / 2 + 145, frame.height / 2 + 107, 0x00555555);

			frame_DrawRectFilled(frame.width / 2 - 55, frame.height / 2 - 140, frame.width / 2 - 53, frame.height / 2 + 100, 0x00555555);	// vertical seperator bar

			frame_DrawRectFilled(frame.width / 2 - 145, frame.height / 2 - 20, frame.width / 2 - 60, frame.height / 2 - 18, 0x00555555);	// short horizontal seperator bar (plr stats | atk & extra)
			

			frame_DrawText(frame.width / 2 - 130, frame.height / 2 + 90, "Stats", 2, 2, 0x00555555);
			
			frame_DrawImage(PLAYER_NORMAL, frame.width / 2 - 120, frame.height / 2 - 4 , 3, -1);

			char hp[100] = { 0 };
			text_AppendInt(&hp, plr.stats[0]);
			frame_DrawText(frame.width / 2 - 97 - (text_GetSize(hp) * 6), frame.height / 2 + 69, hp, 2, -1, 0x00FF3333);
			frame_DrawImage(IMAGE_HEART, frame.width / 2 - 110 - (text_GetSize(hp) * 6), frame.height / 2 + 60, 2, -1);
			
			/*
			char atk[100] = { 0 };
			text_AppendInt(&atk, plr.stats[1]);
			frame_DrawText(frame.width / 2 - 117 , frame.height / 2 - 38, atk, 2, -1, 0x003333FF);
			frame_DrawImage(IMAGE_SWORD, frame.width / 2 - 130, frame.height / 2 - 47, 2, -1);
			*/
			
			frame_DrawText(frame.width / 2 - 40, frame.height / 2 + 90, "Gear", 2, 2, 0x00555555);
			
			
			// max weapon name length: 25

			frame_DrawRectFilled(frame.width / 2 - 40, frame.height / 2 + 70, frame.width / 2 - 10, frame.height / 2 + 40, 0x00555555);
			
			frame_DrawImage(IMAGE_ICON_SWORD, frame.width / 2 - 35, frame.height / 2 + 45, 3, -1);

			frame_DrawText(frame.width / 2 - 5, frame.height / 2 + 69, plr.gear_names[1], (ceil)(130.0 / (text_GetSize(plr.gear_names[1]) * 6)), 2, 0x00555555);

			//int i_atk = plr.gear_stats[0][0];

			text_ClearString(&atk);
			atk[0] = '+';
			text_AppendInt(&atk, plr.gear_stats[1][1]);
			frame_DrawText(frame.width / 2 + 8, frame.height / 2 + 54, atk, 2, -1, 0x003333FF);
			frame_DrawImage(IMAGE_SWORD, frame.width / 2 - 5, frame.height / 2 + 45, 2, -1);

			


			frame_DrawRectFilled(frame.width / 2 - 40, frame.height / 2 + 30, frame.width / 2 - 10, frame.height / 2 + 0, 0x00555555);

			frame_DrawImage(IMAGE_ICON_ARMOR, frame.width / 2 - 35, frame.height / 2 + 5, 3, -1);

			frame_DrawText(frame.width / 2 - 5, frame.height / 2 + 29, plr.gear_names[0], (ceil)(130.0 / (text_GetSize(plr.gear_names[0]) * 6)), 2, 0x00555555);

			text_ClearString(&hp);
			hp[0] = '+';
			text_AppendInt(&hp, plr.gear_stats[0][0]);
			frame_DrawText(frame.width / 2 + 8, frame.height / 2 + 15, hp, 2, -1, 0x00FF3333);
			frame_DrawImage(IMAGE_HEART, frame.width / 2 - 5, frame.height / 2 + 6, 2, -1);



		};

		

		
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

		frame_bitmapInfo.bmiHeader.biWidth = LOWORD(lParam);
		frame_bitmapInfo.bmiHeader.biHeight = HIWORD(lParam);

		if (frame_hBitmap) {
			DeleteObject(frame_hBitmap);
		};

		frame_hBitmap = CreateDIBSection(NULL, &frame_bitmapInfo, DIB_RGB_COLORS, &frame.pixels, 0, 0);

		SelectObject(frame_dc, frame_hBitmap);

		frame.width = LOWORD(lParam);
		frame.height = HIWORD(lParam);

		break;
	};

	case (WM_PAINT): {

		static PAINTSTRUCT ps;
		static HDC hdc;
		hdc = BeginPaint(hWnd, &ps);

		BitBlt(hdc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			(ps.rcPaint.right - ps.rcPaint.left),
			(ps.rcPaint.bottom - ps.rcPaint.top),
			frame_dc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			SRCCOPY);

		EndPaint(hWnd, &ps);

		break;
	};

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	};



	return 0;
};
