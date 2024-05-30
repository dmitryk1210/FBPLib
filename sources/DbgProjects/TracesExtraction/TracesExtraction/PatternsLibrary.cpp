#include "PatternsLibrary.h"

const uint8_t PatternsLibrary::up3[UP3_SIZE * 2][UP3_SIZE * UP3_SIZE] = {
	{ //0 background
		0,0,0,
		0,0,0,
		0,0,0},
	{ //1 star
		0,0,0,
		0,8,0,
		0,0,0},
	{ //2 track backlash
		6,1,0,
		1,6,1,
		0,1,6},
	{ //3 track |
		0,8,0,
		0,8,0,
		0,8,0},
	{ //4 track /
		0,1,6,
		1,6,1,
		6,1,0},
	{ //5 track -
		0,0,0,
		8,8,8,
		0,0,0},
};

const uint8_t PatternsLibrary::up5[UP5_SIZE * 2][UP5_SIZE * UP5_SIZE] = {
	{
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,0,0,0},
	{
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,8,0,0,
		0,0,0,0,0,
		0,0,0,0,0},
	{
		0,0,0,0,0,
		8,4,0,0,0,
		0,4,8,4,0,
		0,0,0,4,8,
		0,0,0,0,0},
	{
		6,1,0,0,0,
		1,6,1,0,0,
		0,1,6,1,0,
		0,0,1,6,1,
		0,0,0,1,6},
	{
		0,8,0,0,0,
		0,4,4,0,0,
		0,0,8,0,0,
		0,0,4,4,0,
		0,0,0,8,0},
	{
		0,0,8,0,0,
		0,0,8,0,0,
		0,0,8,0,0,
		0,0,8,0,0,
		0,0,8,0,0},
	{
		0,0,0,8,0,
		0,0,4,4,0,
		0,0,8,0,0,
		0,4,4,0,0,
		0,8,0,0,0},
	{
		0,0,0,1,6,
		0,0,1,6,1,
		0,1,6,1,0,
		1,6,1,0,0,
		6,1,0,0,0},
	{
		0,0,0,0,0,
		0,0,0,4,8,
		0,4,8,4,0,
		8,4,0,0,0,
		0,0,0,0,0},
	{
		0,0,0,0,0,
		0,0,0,0,0,
		8,8,8,8,8,
		0,0,0,0,0,
		0,0,0,0,0},
};

const uint8_t PatternsLibrary::up7[UP7_SIZE * 2][UP7_SIZE * UP7_SIZE] = {
	{
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0},
	{
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,8,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0},
	{
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		8,5,3,0,0,0,0,
		0,3,5,8,5,3,0,
		0,0,0,0,3,5,8,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0},
	{
		1,0,0,0,0,0,0,
		6,3,0,0,0,0,0,
		1,5,5,1,0,0,0,
		0,0,3,6,3,0,0,
		0,0,0,1,5,5,1,
		0,0,0,0,0,3,6,
		0,0,0,0,0,0,1},
	{
		6,1,0,0,0,0,0,
		1,6,1,0,0,0,0,
		0,1,6,1,0,0,0,
		0,0,1,6,1,0,0,
		0,0,0,1,6,1,0,
		0,0,0,0,1,6,1,
		0,0,0,0,0,1,6},
	{
		1,6,1,0,0,0,0,
		0,3,5,0,0,0,0,
		0,0,5,3,0,0,0,
		0,0,1,6,1,0,0,
		0,0,0,3,5,0,0,
		0,0,0,0,5,3,0,
		0,0,0,0,1,6,1},
	{
		0,0,8,0,0,0,0,
		0,0,5,3,0,0,0,
		0,0,3,5,0,0,0,
		0,0,0,8,0,0,0,
		0,0,0,5,3,0,0,
		0,0,0,3,5,0,0,
		0,0,0,0,8,0,0},
	{
		0,0,0,8,0,0,0,
		0,0,0,8,0,0,0,
		0,0,0,8,0,0,0,
		0,0,0,8,0,0,0,
		0,0,0,8,0,0,0,
		0,0,0,8,0,0,0,
		0,0,0,8,0,0,0},
	{
		0,0,0,0,8,0,0,
		0,0,0,3,5,0,0,
		0,0,0,5,3,0,0,
		0,0,0,8,0,0,0,
		0,0,3,5,0,0,0,
		0,0,5,3,0,0,0,
		0,0,8,0,0,0,0},
	{
		0,0,0,0,1,6,1,
		0,0,0,0,5,3,0,
		0,0,0,3,5,0,0,
		0,0,1,6,1,0,0,
		0,0,5,3,0,0,0,
		0,3,5,0,0,0,0,
		1,6,1,0,0,0,0},
	{
		0,0,0,0,0,1,6,
		0,0,0,0,1,6,1,
		0,0,0,1,6,1,0,
		0,0,1,6,1,0,0,
		0,1,6,1,0,0,0,
		1,6,1,0,0,0,0,
		6,1,0,0,0,0,0},
	{
		0,0,0,0,0,0,1,
		0,0,0,0,0,3,6,
		0,0,0,1,5,5,1,
		0,0,3,6,3,0,0,
		1,5,5,1,0,0,0,
		6,3,0,0,0,0,0,
		1,0,0,0,0,0,0},
	{
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,3,5,8,
		0,3,5,8,5,3,0,
		8,5,3,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0},
	{
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		8,8,8,8,8,8,8,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0},
};

const uint8_t PatternsLibrary::up9[UP9_SIZE * 2][UP9_SIZE * UP9_SIZE] = {
	{
		0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,	//-2
		0,0,0,0,0,0,0,0,0,	//-1
		0,0,0,0,0,0,0,0,0,	// 0
		0,0,0,0,0,0,0,0,0,	// 1
		0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,	//-2
		0,0,0,0,0,0,0,0,0,	//-1
		0,0,0,0,8,0,0,0,0,	// 0
		0,0,0,0,0,0,0,0,0,	// 1
		0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,	//-2
		8,7,4,1,0,0,0,0,0,	//-1
		0,1,4,7,8,7,4,1,0,	// 0
		0,0,0,0,0,1,4,7,8,	// 1
		0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,0,0,0,0,	//-4
		1,0,0,0,0,0,0,0,0,	//-3
		6,4,1,0,0,0,0,0,0,	//-2
		1,4,6,4,1,0,0,0,0,	//-1
		0,0,1,4,6,4,1,0,0,	// 0
		0,0,0,0,1,4,6,4,1,	// 1
		0,0,0,0,0,0,1,4,6,	// 2
		0,0,0,0,0,0,0,0,1,	// 3
		0,0,0,0,0,0,0,0,0}, 	// 4
	{
		1,0,0,0,0,0,0,0,0,	//-4
		6,3,0,0,0,0,0,0,0,	//-3
		1,5,4,0,0,0,0,0,0,	//-2
		0,0,4,5,1,0,0,0,0,	//-1
		0,0,0,3,6,3,0,0,0,	// 0
		0,0,0,0,1,5,4,0,0,	// 1
		0,0,0,0,0,0,4,5,1,	// 2
		0,0,0,0,0,0,0,3,6,	// 3
		0,0,0,0,0,0,0,0,1}, 	// 4
	{
		6,1,0,0,0,0,0,0,0,	//-4
		1,6,1,0,0,0,0,0,0,	//-3
		0,1,6,1,0,0,0,0,0,	//-2
		0,0,1,6,1,0,0,0,0,	//-1
		0,0,0,1,6,1,0,0,0,	// 0
		0,0,0,0,1,6,1,0,0,	// 1
		0,0,0,0,0,1,6,1,0,	// 2
		0,0,0,0,0,0,1,6,1, 	// 3
		0,0,0,0,0,0,0,1,6}, 	// 4
	{
		1,6,1,0,0,0,0,0,0,	//-4
		0,3,5,0,0,0,0,0,0,	//-3
		0,0,4,4,0,0,0,0,0,	//-2
		0,0,0,5,3,0,0,0,0,	//-1
		0,0,0,1,6,1,0,0,0,	// 0
		0,0,0,0,3,5,0,0,0,	// 1
		0,0,0,0,0,4,4,0,0,	// 2
		0,0,0,0,0,0,5,3,0,	// 3
		0,0,0,0,0,0,1,6,1}, 	// 4
	{
		0,1,6,1,0,0,0,0,0,	//-4
		0,0,4,4,0,0,0,0,0,	//-3
		0,0,1,6,1,0,0,0,0,	//-2
		0,0,0,4,4,0,0,0,0,	//-1
		0,0,0,1,6,1,0,0,0,	// 0
		0,0,0,0,4,4,0,0,0,	// 1
		0,0,0,0,1,6,1,0,0,	// 2
		0,0,0,0,0,4,4,0,0,	// 3
		0,0,0,0,0,1,6,1,0}, 	// 4
	{
		0,0,0,8,0,0,0,0,0,	//-4
		0,0,0,7,1,0,0,0,0,	//-3
		0,0,0,4,4,0,0,0,0,	//-2
		0,0,0,1,7,0,0,0,0,	//-1
		0,0,0,0,8,0,0,0,0,	// 0
		0,0,0,0,7,1,0,0,0,	// 1
		0,0,0,0,4,4,0,0,0,	// 2
		0,0,0,0,1,7,0,0,0,	// 3
		0,0,0,0,0,8,0,0,0}, 	// 4
	{
		0,0,0,0,8,0,0,0,0,	//-4
		0,0,0,0,8,0,0,0,0,	//-3
		0,0,0,0,8,0,0,0,0,	//-2
		0,0,0,0,8,0,0,0,0,	//-1
		0,0,0,0,8,0,0,0,0,	// 0
		0,0,0,0,8,0,0,0,0,	// 1
		0,0,0,0,8,0,0,0,0,	// 2
		0,0,0,0,8,0,0,0,0,	// 3
		0,0,0,0,8,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,8,0,0,0,	//-4
		0,0,0,0,1,7,0,0,0,	//-3
		0,0,0,0,4,4,0,0,0,	//-2
		0,0,0,0,7,1,0,0,0,	//-1
		0,0,0,0,8,0,0,0,0,	// 0
		0,0,0,1,7,0,0,0,0,	// 1
		0,0,0,4,4,0,0,0,0,	// 2
		0,0,0,7,1,0,0,0,0,	// 3
		0,0,0,8,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,1,6,1,0,	//-4
		0,0,0,0,0,4,4,0,0,	//-3
		0,0,0,0,1,6,1,0,0,	//-2
		0,0,0,0,4,4,0,0,0,	//-1
		0,0,0,1,6,1,0,0,0,	// 0
		0,0,0,4,4,0,0,0,0,	// 1
		0,0,1,6,1,0,0,0,0,	// 2
		0,0,4,4,0,0,0,0,0,	// 3
		0,1,6,1,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,0,1,6,1,	//-4
		0,0,0,0,0,0,5,3,0,	//-3
		0,0,0,0,0,4,4,0,0,	//-2
		0,0,0,0,3,5,0,0,0,	//-1
		0,0,0,1,6,1,0,0,0,	// 0
		0,0,0,5,3,0,0,0,0,	// 1
		0,0,4,4,0,0,0,0,0,	// 2
		0,3,5,0,0,0,0,0,0,	// 3
		1,6,1,0,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,0,0,1,6,	//-4
		0,0,0,0,0,0,1,6,1,	//-3
		0,0,0,0,0,1,6,1,0,	//-2
		0,0,0,0,1,6,1,0,0,	//-1
		0,0,0,1,6,1,0,0,0,	// 0
		0,0,1,6,1,0,0,0,0,	// 1
		0,1,6,1,0,0,0,0,0,	// 2
		1,6,1,0,0,0,0,0,0,	// 3
		6,1,0,0,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,0,0,0,1,	//-4
		0,0,0,0,0,0,0,3,6,	//-3
		0,0,0,0,0,0,4,5,1,	//-2
		0,0,0,0,1,5,4,0,0,	//-1
		0,0,0,3,6,3,0,0,0,	// 0
		0,0,4,5,1,0,0,0,0,	// 1
		1,5,4,0,0,0,0,0,0,	// 2
		6,3,0,0,0,0,0,0,0,	// 3
		1,0,0,0,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,1,	//-3
		0,0,0,0,0,0,1,4,6,	//-2
		0,0,0,0,1,4,6,4,1,	//-1
		0,0,1,4,6,4,1,0,0,	// 0
		1,4,6,4,1,0,0,0,0,	// 1
		6,4,1,0,0,0,0,0,0,	// 2
		1,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,	//-2
		0,0,0,0,0,1,4,7,8,	//-1
		0,1,4,7,8,7,4,1,0,	// 0
		8,7,4,1,0,0,0,0,0,	// 1
		0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0}, 	// 4
	{
		0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,	//-2
		0,0,0,0,0,0,0,0,0,	//-1
		8,8,8,8,8,8,8,8,8,	// 0
		0,0,0,0,0,0,0,0,0,	// 1
		0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0}, 	// 4
};

const uint8_t PatternsLibrary::up11[UP11_SIZE * 2][UP11_SIZE * UP11_SIZE] = {
	{
		0,0,0,0,0,0,0,0,0,0,0,	//-5
		0,0,0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,0,0,	//-2
		0,0,0,0,0,0,0,0,0,0,0,	//-1
		0,0,0,0,0,0,0,0,0,0,0,	// 0
		0,0,0,0,0,0,0,0,0,0,0,	// 1
		0,0,0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0,0,0,	// 4
		0,0,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,0,0,	//-5
		0,0,0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,0,0,	//-2
		0,0,0,0,0,0,0,0,0,0,0,	//-1
		0,0,0,0,0,8,0,0,0,0,0,	// 0
		0,0,0,0,0,0,0,0,0,0,0,	// 1
		0,0,0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0,0,0,	// 4
		0,0,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,0,0,	//-5
		0,0,0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,0,0,	//-2
		8,7,4,4,1,0,0,0,0,0,0,	//-1
		0,1,4,4,7,8,7,4,4,1,0,	// 0
		0,0,0,0,0,0,1,4,4,7,8,	// 1
		0,0,0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0,0,0,	// 4
		0,0,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,0,0,	//-5
		0,0,0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,0,0,	//-3
		8,5,1,0,0,0,0,0,0,0,0,	//-2
		0,3,7,7,3,0,0,0,0,0,0,	//-1
		0,0,0,1,5,8,5,1,0,0,0,	// 0
		0,0,0,0,0,0,3,7,7,3,0,	// 1
		0,0,0,0,0,0,0,0,1,5,8,	// 2
		0,0,0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0,0,0,	// 4
		0,0,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,0,0,	//-5
		1,0,0,0,0,0,0,0,0,0,0,	//-4
		6,3,0,0,0,0,0,0,0,0,0,	//-3
		1,5,7,1,0,0,0,0,0,0,0,	//-2
		0,0,1,7,5,1,0,0,0,0,0,	//-1
		0,0,0,0,3,6,3,0,0,0,0,	// 0
		0,0,0,0,0,1,5,7,1,0,0,	// 1
		0,0,0,0,0,0,0,1,7,5,1,	// 2
		0,0,0,0,0,0,0,0,0,3,6,	// 3
		0,0,0,0,0,0,0,0,0,0,1,	// 4
		0,0,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		1,0,0,0,0,0,0,0,0,0,0,	//-5
		6,1,0,0,0,0,0,0,0,0,0,	//-4
		1,7,4,0,0,0,0,0,0,0,0,	//-3
		0,0,4,4,0,0,0,0,0,0,0,	//-2
		0,0,0,4,7,1,0,0,0,0,0,	//-1
		0,0,0,0,1,6,1,0,0,0,0,	// 0
		0,0,0,0,0,1,7,4,0,0,0,	// 1
		0,0,0,0,0,0,0,4,4,0,0,	// 2
		0,0,0,0,0,0,0,0,4,7,1,	// 3
		0,0,0,0,0,0,0,0,0,1,6,	// 4
		0,0,0,0,0,0,0,0,0,0,1}, 	// 5
	{
		6,1,0,0,0,0,0,0,0,0,0,	//-5
		1,6,1,0,0,0,0,0,0,0,0,	//-4
		0,1,6,1,0,0,0,0,0,0,0,	//-3
		0,0,1,6,1,0,0,0,0,0,0,	//-2
		0,0,0,1,6,1,0,0,0,0,0,	//-1
		0,0,0,0,1,6,1,0,0,0,0,	// 0
		0,0,0,0,0,1,6,1,0,0,0,	// 1
		0,0,0,0,0,0,1,6,1,0,0,	// 2
		0,0,0,0,0,0,0,1,6,1,0,	// 3
		0,0,0,0,0,0,0,0,1,6,1, 	// 4
		0,0,0,0,0,0,0,0,0,1,6}, 	// 5
	{
		1,6,1,0,0,0,0,0,0,0,0,	//-5
		0,1,7,0,0,0,0,0,0,0,0,	//-4
		0,0,4,4,0,0,0,0,0,0,0,	//-3
		0,0,0,4,4,0,0,0,0,0,0,	//-2
		0,0,0,0,7,1,0,0,0,0,0,	//-1
		0,0,0,0,1,6,1,0,0,0,0,	// 0
		0,0,0,0,0,1,7,0,0,0,0,	// 1
		0,0,0,0,0,0,4,4,0,0,0,	// 2
		0,0,0,0,0,0,0,4,4,0,0,	// 3
		0,0,0,0,0,0,0,0,7,1,0,	// 4
		0,0,0,0,0,0,0,0,1,6,1}, 	// 5
	{
		0,1,6,1,0,0,0,0,0,0,0,	//-5
		0,0,3,5,0,0,0,0,0,0,0,	//-4
		0,0,0,7,1,0,0,0,0,0,0,	//-3
		0,0,0,1,7,0,0,0,0,0,0,	//-2
		0,0,0,0,5,3,0,0,0,0,0,	//-1
		0,0,0,0,1,6,1,0,0,0,0,	// 0
		0,0,0,0,0,3,5,0,0,0,0,	// 1
		0,0,0,0,0,0,7,1,0,0,0,	// 2
		0,0,0,0,0,0,1,7,0,0,0,	// 3
		0,0,0,0,0,0,0,5,3,0,0,	// 4
		0,0,0,0,0,0,0,1,6,1,0}, 	// 5
	{
		0,0,0,8,0,0,0,0,0,0,0,	//-5
		0,0,0,5,3,0,0,0,0,0,0,	//-4
		0,0,0,1,7,0,0,0,0,0,0,	//-3
		0,0,0,0,7,1,0,0,0,0,0,	//-2
		0,0,0,0,3,5,0,0,0,0,0,	//-1
		0,0,0,0,0,8,0,0,0,0,0,	// 0
		0,0,0,0,0,5,3,0,0,0,0,	// 1
		0,0,0,0,0,1,7,0,0,0,0,	// 2
		0,0,0,0,0,0,7,1,0,0,0,	// 3
		0,0,0,0,0,0,3,5,0,0,0,	// 4
		0,0,0,0,0,0,0,8,0,0,0}, 	// 5
	{
		0,0,0,0,8,0,0,0,0,0,0,	//-5
		0,0,0,0,7,1,0,0,0,0,0,	//-4
		0,0,0,0,4,4,0,0,0,0,0,	//-3
		0,0,0,0,4,4,0,0,0,0,0,	//-2
		0,0,0,0,1,7,0,0,0,0,0,	//-1
		0,0,0,0,0,8,0,0,0,0,0,	// 0
		0,0,0,0,0,7,1,0,0,0,0,	// 1
		0,0,0,0,0,4,4,0,0,0,0,	// 2
		0,0,0,0,0,4,4,0,0,0,0,	// 3
		0,0,0,0,0,1,7,0,0,0,0,	// 4
		0,0,0,0,0,0,8,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,8,0,0,0,0,0,	//-5
		0,0,0,0,0,8,0,0,0,0,0,	//-4
		0,0,0,0,0,8,0,0,0,0,0,	//-3
		0,0,0,0,0,8,0,0,0,0,0,	//-2
		0,0,0,0,0,8,0,0,0,0,0,	//-1
		0,0,0,0,0,8,0,0,0,0,0,	// 0
		0,0,0,0,0,8,0,0,0,0,0,	// 1
		0,0,0,0,0,8,0,0,0,0,0,	// 2
		0,0,0,0,0,8,0,0,0,0,0,	// 3
		0,0,0,0,0,8,0,0,0,0,0,	// 4
		0,0,0,0,0,8,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,8,0,0,0,0,	//-5
		0,0,0,0,0,1,7,0,0,0,0,	//-4
		0,0,0,0,0,4,4,0,0,0,0,	//-3
		0,0,0,0,0,4,4,0,0,0,0,	//-2
		0,0,0,0,0,7,1,0,0,0,0,	//-1
		0,0,0,0,0,8,0,0,0,0,0,	// 0
		0,0,0,0,1,7,0,0,0,0,0,	// 1
		0,0,0,0,4,4,0,0,0,0,0,	// 2
		0,0,0,0,4,4,0,0,0,0,0,	// 3
		0,0,0,0,7,1,0,0,0,0,0,	// 4
		0,0,0,0,8,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,8,0,0,0,	//-5
		0,0,0,0,0,0,3,5,0,0,0,	//-4
		0,0,0,0,0,0,7,1,0,0,0,	//-3
		0,0,0,0,0,1,7,0,0,0,0,	//-2
		0,0,0,0,0,5,3,0,0,0,0,	//-1
		0,0,0,0,0,8,0,0,0,0,0,	// 0
		0,0,0,0,3,5,0,0,0,0,0,	// 1
		0,0,0,0,7,1,0,0,0,0,0,	// 2
		0,0,0,1,7,0,0,0,0,0,0,	// 3
		0,0,0,5,3,0,0,0,0,0,0,	// 4
		0,0,0,8,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,1,6,1,0,	//-5
		0,0,0,0,0,0,0,5,3,0,0,	//-4
		0,0,0,0,0,0,1,7,0,0,0,	//-3
		0,0,0,0,0,0,7,1,0,0,0,	//-2
		0,0,0,0,0,3,5,0,0,0,0,	//-1
		0,0,0,0,1,6,1,0,0,0,0,	// 0
		0,0,0,0,5,3,0,0,0,0,0,	// 1
		0,0,0,1,7,0,0,0,0,0,0,	// 2
		0,0,0,7,1,0,0,0,0,0,0,	// 3
		0,0,3,5,0,0,0,0,0,0,0,	// 4
		0,1,6,1,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,1,6,1,	//-5
		0,0,0,0,0,0,0,0,7,1,0,	//-4
		0,0,0,0,0,0,0,4,4,0,0,	//-3
		0,0,0,0,0,0,4,4,0,0,0,	//-2
		0,0,0,0,0,1,7,0,0,0,0,	//-1
		0,0,0,0,1,6,1,0,0,0,0,	// 0
		0,0,0,0,7,1,0,0,0,0,0,	// 1
		0,0,0,4,4,0,0,0,0,0,0,	// 2
		0,0,4,4,0,0,0,0,0,0,0,	// 3
		0,1,7,0,0,0,0,0,0,0,0,	// 4
		1,6,1,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,1,6,	//-5
		0,0,0,0,0,0,0,0,1,6,1,	//-4
		0,0,0,0,0,0,0,1,6,1,0,	//-3
		0,0,0,0,0,0,1,6,1,0,0,	//-2
		0,0,0,0,0,1,6,1,0,0,0,	//-1
		0,0,0,0,1,6,1,0,0,0,0,	// 0
		0,0,0,1,6,1,0,0,0,0,0,	// 1
		0,0,1,6,1,0,0,0,0,0,0,	// 2
		0,1,6,1,0,0,0,0,0,0,0,	// 3
		1,6,1,0,0,0,0,0,0,0,0,	// 4
		6,1,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,0,1,	//-5
		0,0,0,0,0,0,0,0,0,1,6,	//-4
		0,0,0,0,0,0,0,0,4,7,1,	//-3
		0,0,0,0,0,0,0,4,4,0,0,	//-2
		0,0,0,0,0,1,7,4,0,0,0,	//-1
		0,0,0,0,1,6,1,0,0,0,0,	// 0
		0,0,0,4,7,1,0,0,0,0,0,	// 1
		0,0,4,4,0,0,0,0,0,0,0,	// 2
		1,7,4,0,0,0,0,0,0,0,0,	// 3
		6,1,0,0,0,0,0,0,0,0,0,	// 4
		1,0,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,0,0,	//-5
		0,0,0,0,0,0,0,0,0,0,1,	//-4
		0,0,0,0,0,0,0,0,0,3,6,	//-3
		0,0,0,0,0,0,0,1,7,5,1,	//-2
		0,0,0,0,0,1,5,7,1,0,0,	//-1
		0,0,0,0,3,6,3,0,0,0,0,	// 0
		0,0,1,7,5,1,0,0,0,0,0,	// 1
		1,5,7,1,0,0,0,0,0,0,0,	// 2
		6,3,0,0,0,0,0,0,0,0,0,	// 3
		1,0,0,0,0,0,0,0,0,0,0,	// 4
		0,0,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,0,0,	//-5
		0,0,0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,1,5,8,	//-2
		0,0,0,0,0,0,3,7,7,3,0,	//-1
		0,0,0,1,5,8,5,1,0,0,0,	// 0
		0,3,7,7,3,0,0,0,0,0,0,	// 1
		8,5,1,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0,0,0,	// 4
		0,0,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,0,0,	//-5
		0,0,0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,0,0,	//-2
		0,0,0,0,0,0,1,4,4,7,8,	//-1
		0,1,4,4,7,8,7,4,4,1,0,	// 0
		8,7,4,4,1,0,0,0,0,0,0,	// 1
		0,0,0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0,0,0,	// 4
		0,0,0,0,0,0,0,0,0,0,0}, 	// 5
	{
		0,0,0,0,0,0,0,0,0,0,0,	//-5
		0,0,0,0,0,0,0,0,0,0,0,	//-4
		0,0,0,0,0,0,0,0,0,0,0,	//-3
		0,0,0,0,0,0,0,0,0,0,0,	//-2
		0,0,0,0,0,0,0,0,0,0,0,	//-1
		8,8,8,8,8,8,8,8,8,8,8,	// 0
		0,0,0,0,0,0,0,0,0,0,0,	// 1
		0,0,0,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,0,0,0,	// 4
		0,0,0,0,0,0,0,0,0,0,0}, 	// 5

};

void PatternsLibrary::Init()
{
	{
		int offset = UP3_OFFSET;
		int WL = UP3_SIZE;
		int S = WL * WL;
		for (int k = 0; k < WL * 2; ++k) {

			SF [offset + k] = 0;
			SFF[offset + k] = 0;
			for (int p = 0; p < WL; ++p) {
				for (int q = 0; q < WL; ++q) {
					SF [offset + k] += up3[k][p * WL + q];
					SFF[offset + k] += up3[k][p * WL + q] * up3[k][p * WL + q];
				}
			}
			C_a[offset + k] = 1.f / (SFF[offset + k] * S - SF[offset + k] * SF[offset + k]);
			C_b[offset + k] = 1.f / S;
		}
	}

	{
		int offset = UP5_OFFSET;
		int WL = UP5_SIZE;
		int S = WL * WL;
		for (int k = 0; k < WL * 2; ++k) {

			SF [offset + k] = 0;
			SFF[offset + k] = 0;
			for (int p = 0; p < WL; ++p) {
				for (int q = 0; q < WL; ++q) {
					SF [offset + k] += up3[k][p * WL + q];
					SFF[offset + k] += up3[k][p * WL + q] * up3[k][p * WL + q];
				}
			}
			C_a[offset + k] = 1.f / (SFF[offset + k] * S - SF[offset + k] * SF[offset + k]);
			C_b[offset + k] = 1.f / S;
		}
	}

	{
		int offset = UP7_OFFSET;
		int WL = UP7_SIZE;
		int S = WL * WL;
		for (int k = 0; k < WL * 2; ++k) {

			SF [offset + k] = 0;
			SFF[offset + k] = 0;
			for (int p = 0; p < WL; ++p) {
				for (int q = 0; q < WL; ++q) {
					SF [offset + k] += up7[k][p * WL + q];
					SFF[offset + k] += up7[k][p * WL + q] * up7[k][p * WL + q];
				}
			}
			C_a[offset + k] = 1.f / (SFF[offset + k] * S - SF[offset + k] * SF[offset + k]);
			C_b[offset + k] = 1.f / S;
		}
	}

	{
		int offset = UP9_OFFSET;
		int WL = UP9_SIZE;
		int S = WL * WL;
		for (int k = 0; k < WL * 2; ++k) {

			SF [offset + k] = 0;
			SFF[offset + k] = 0;
			for (int p = 0; p < WL; ++p) {
				for (int q = 0; q < WL; ++q) {
					SF [offset + k] += up9[k][p * WL + q];
					SFF[offset + k] += up9[k][p * WL + q] * up9[k][p * WL + q];
				}
			}
			C_a[offset + k] = 1.f / (SFF[offset + k] * S - SF[offset + k] * SF[offset + k]);
			C_b[offset + k] = 1.f / S;
		}
	}

	{
		int offset = UP11_OFFSET;
		int WL = UP11_SIZE;
		int S = WL * WL;
		for (int k = 0; k < WL * 2; ++k) {

			SF [offset + k] = 0;
			SFF[offset + k] = 0;
			for (int p = 0; p < WL; ++p) {
				for (int q = 0; q < WL; ++q) {
					SF [offset + k] += up11[k][p * WL + q];
					SFF[offset + k] += up11[k][p * WL + q] * up11[k][p * WL + q];
				}
			}
			C_a[offset + k] = 1.f / (SFF[offset + k] * S - SF[offset + k] * SF[offset + k]);
			C_b[offset + k] = 1.f / S;
		}
	}

}