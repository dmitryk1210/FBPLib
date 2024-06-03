// PatternsLibrary.h
//

#pragma once

#include <numeric>

#define		UP3_SIZE		3u
#define		UP5_SIZE		5u
#define		UP7_SIZE		7u
#define		UP9_SIZE		9u
#define		UP11_SIZE		11u

#define		PATTERN_MAX_SIZE	UP11_SIZE

#define		UP3_OFFSET		0
#define		UP5_OFFSET		(UP3_OFFSET  + UP3_SIZE  * 2)
#define		UP7_OFFSET		(UP5_OFFSET  + UP5_SIZE  * 2)
#define		UP9_OFFSET		(UP7_OFFSET  + UP7_SIZE  * 2)
#define		UP11_OFFSET		(UP9_OFFSET  + UP9_SIZE  * 2)
#define		PATTERNS_COUNT	(UP11_OFFSET + UP11_SIZE * 2)

struct PatternsLibrary {
	float SF [PATTERNS_COUNT];
	float SFF[PATTERNS_COUNT];
	float C_a[PATTERNS_COUNT];
	float C_b[PATTERNS_COUNT];

	static const uint8_t up3 [UP3_SIZE  * 2][UP3_SIZE  * UP3_SIZE ];
	static const uint8_t up5 [UP5_SIZE  * 2][UP5_SIZE  * UP5_SIZE ];
	static const uint8_t up7 [UP7_SIZE  * 2][UP7_SIZE  * UP7_SIZE ];
	static const uint8_t up9 [UP9_SIZE  * 2][UP9_SIZE  * UP9_SIZE ];
	static const uint8_t up11[UP11_SIZE * 2][UP11_SIZE * UP11_SIZE];


	void Init();
};