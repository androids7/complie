/*
 * =====================================================================================
 *
 *       Filename:  lex.h
 *
 *    Description:  词法分析头文件
 *
 *        Version:  1.0
 *        Created:  03/25/2015 01:08:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WYL (502), ylwzzu@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

enum token
{
	TK_BEGIN,
#define TOKEN(k, s) k,
#include "token.h"
#undef TOKEN
};   //定义关键词

union value
{
	int i[2];
	float f;
	double d;
	void *p;
};

#define IsDigit(c)		(c >= '0' && c <= '9')
#define IsOctDigit(c)	(c >= '0' && c <= '7')
#define IsHexDigit(c)	(IsDigit(c) || c >= 'A' && c <= 'F')
#define IsLetter(c)		(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_')
#define IsLetterOrDigit(c) (IsDigit(c) || IsLetter(c)) 
#define ToUpper(c)		(c & ~0x20)
#define HIGH_4BIT(v)	((v) >> (8*sizeof(int) - 4) & 0xof)
#define HIGH_3BIT(v)	((v) >> (8*sizeof(int) - 3) & 0x07)
#define HIGH_1BIT(v)	((v) >> (8*sizeof(int) - 1) & 0x01)	
void InitScanner(void);
int GetNextToken(void);


extern union value TokenValue;
extern struct coord Token Coord;
extern char* TokenStrings[];

