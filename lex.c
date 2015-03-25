/*
 * =====================================================================================
 *
 *       Filename:  lex.c
 *
 *    Description:  词法分析
 *
 *        Version:  1.0
 *        Created:  03/25/2015 01:52:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WYL (502), ylwzzu@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ucl.h"
#include "keyworld.h"
#include "lex.h"

#define CURSOR 		(Input.cursor)
#define LINE		(Input.line)
#define LINEHEAD	(Input.linehead)

typedef int (*Scanner)(void);
static Scanner Scanners[256];

union value TokenValue;
struct coord TokenCoord;
struct coord prevCoord;
struct coord PrevCoord;

char *TokenString[] = 
{
#define TOKEN(k, s) s,
#include "token.h"
#undef TOKEN
}

static void ScanPPLine(void)
{
	int line = 0;
	CURSOR++;
	while(*CURSOR == ' ' || *CURSOR == '\t')
	{
		CURSOR++;
	}

	if(IsDigit(*CURSOR))
	{
		goto read_line;
	}
	else if(strcmp(CURSOR, "line", 4) == 0)
	{
		CURSOR += 4;
		while(*CURSOR == ' ' || *CURSOR == '\t')
		{
			CURSOR++;
		}
read_line:
		while(IsDigit(*CURSOR))
		{
			line = 10 * line + *CURSOR - '0';
			CURSOR++;
		}
		TokenCoord.ppline = line - 1;

		while(*CURSOR == ' ' || *CURSOR == '\t')
		{
			*CURSOR++;
		}
		TokenCoord.filename += CURSOR;
		while(*CURSOR != '"' && *CURSOR != END_OF_FILE)
		{
			CURSOR++;
		}
		TokenCoord.filename = InternName(TokenCoord.filename, (char *)CURSOR - TokenCoord.filename);
	}
	while(*CURSOR != '\n' && *CURSOR != END_OF_FILE)
	{
		CURSOR++;
	}
}

static int ScanIntLiteral(unsigned char *start, int len, int base)
{
	unsigned char *p = start;
	unsigned char *end = start + len;
	unsigned int i[2] = {0, 0};
	int tok = TK_INTCONST;
	int d = 0;
	int carry0 = 0, carry1 = 0;
	int overflow = 0;

	while(p != end)
	{
		if(base == 16)
		{
			if((*p >= 'A' && *p <= 'F')|| 
				(*p >= 'a' && *p <= 'f'))
			{
				d = ToUpper(*p) - 'A' + 10;
			}
			else
			{
				d = *p - '0';
			}
		}
		else
		{
			d = *p - '0';
		}

		switch(base)
		{
			case 16:
				carry0 = HIGH_4BIT(i[0]);
				carry1 = HIGH_4BIT(i[1]);
				i[0] = i[0] << 4;
				i[1] = i[1] << 4;
				break;
			case 8:
				carry0 = HIGH_3BIT(i[0]);
				carry1 = HIGH_3BIT(i[1]);
				i[0] = i[0] << 3;
				i[1] = i[1] << 3;
				break;
			case 10:
				{
					unsigned int t1, t2;

					carry0 = HIGH_3BIT(i[0]) + HIGH_1BIT(i[0]);
					carry1 = HIGH_3BIT(i[1]) + HIGH_1BIT(i[1]);
					t1 = i[0] << 3;
					t2 = i[0] << 1;
					if(t1 > UINT_MAX - t2)
					{
						carry0++;
					}
					i[0] = t1 + t2;
				}
				break;
		}
		if(i[0] > UINT_MAX - d)
		{
			carry0 += i[0] - (UINT_MAX - d);
		}
		if(carry1 || (i[1] > UINT_MAX - carry0))
		{
			overflow = 1;
		}
		i[0] += d;
		i[1] += carry0;
		p++;
	}

	if(overflow || i[1] != 0)
	{
		Warning(&TokenCoord, "Integer literal is too big");
	}
	TokenValue.i[1] = 0;
	TokenValue.i[0] = i[0];
	tok = TK_INTCONST;

	if(*CURSOR == 'U' || *CURSOR == 'u')
	{
		CURSOR++;
		if(tok == TK_INTCONST)
		{
			tok = TK_UINTCONST;
		}
		else if(tok == TK_LLONGCONST)
		{
			tok = TK_ULLONGCONST;
		}
	}
	if(*CURSOR == 'L' || *CURSOR == 'l')
	{
		CURSOR++;
		if(tok == TK_INTCONST)
		{
			tok = TK_LONGCONST;
		}
		else if(tok == TK_UINTCONST)
		{
			tok = TK_ULONGCONST;
		}
		if(*CURSOR == 'L' || *CURSOR == 'l')
		{
			CURSOR++;
			if(tok < TK_LLONGCONST)
			{
				tok = TK_LLONGCONST;
			}
		}
	}
	return tok;
}

static int ScanNumbericLiteral(void)
{
	unsigned char *start = CURSOR;
	int base = 10;

	if(*CURSOR == '.')
	{
		return ScanFloatLiteral(start);
	}

	if(*CURSOR == '0' && (CURSOR[1] == 'x') || CURSOR[1] == 'X')
	{
		CURSOR += 2;
		start = CURSOR;
		base = 16;
		if( !IsHexDigit(*CURSOR))
		{
			Error(&TokenCoord, "Expect hex digit");
			TokenValue.i[0] = 0;
			return TK_INTCONST;		
		}
		while(IsHexDigit(*CURSOR))
		{
			CURSOR++;
		}
	}
	else if(*CURSOR == '0')
	{
		CURSOR++;
		base = 8;
		while(IsOctDigit(*CURSOR))
		{
			CURSOR++;
		}
	}
	else
	{
		CURSOR++;
		while(IsDigit(*CURSOR))
		{
			CURSOR++;
		}
	}

	if(base == 16 || (*CURSOR != '.' && *CURSOR != 'e' && *CURSOR != 'E'))
	{
		return ScanIntLiteral(start, (int)(CURSOR - start), base);
	}
	else
	{
		return ScanFloatLiteral(start);
	}
}

static int ScanFloatLiteral(unsigned char *start)
{
	double d;
	if(*CURSOR == '.')
	{
		CURSOR++;
		while(IsDigit(*CURSOR))
		{
			CURSOR++;
		}
	}
	if(*CURSOR == 'e' || *CURSOR == 'E')
	{
		CURSOR++;
		if(*CURSOR == '+' || *CURSOR == '-')
		{
			CURSOR++;
		}
		if( !IsDigit(*CURSOR))
		{
			Error(&TokenCoord, "Expect exponent value");
		}
		else
		{
			while(IsDigit(*CURSOR))
			{
				CURSOR++;
			}
		}
	}

	errno = 0;
	d = strtod((char*)start, NULL);
	if(errno == ERANGE)
	{
		Warning(&TokenCoord, "Float literal overflow");
	}
	TokenValue.d = d;
	if(*CURSOR == 'f' || *CURSOR == 'F')
	{
		CURSOR++;
		TokenValue.f = (float)d;
		return TK_FLOATCONST;
	}
	else if(*CURSOR == 'L' || *CURSOR == 'l')
	{
		CURSOR++;
		return TK_LDOUBLECONST;
	}
	else
	{
		return TK_DOUBLECONST;
	}
}

void InitScanner(void)
{
	int i;

	for(i=0; i< END_OF_FILE+1; i++)
	{
		if(IsDigit(i))
		{
			Scanners[i] = ScanNumbericLiteral;
		}
		else
		{
			Scanners[i] = ScanBadChar;
		}
	}

}

static void SkipWhiteSpace(void)
{
	int ch;
again:
	ch = *CURSOR;
	while(ch == '\t' || ch == '\v' || ch == '\f' || ch == ' ' ||
		  ch == '\r' || ch == '\n' || ch == '/' || ch =='#'	)
	{
		switch(ch)
		{
			case '\n':
				TokenCoord.ppline++;
				LINE++;
				LINEHEAD = ++CURSOR;
				break;
			case '#':
				ScanPPLine();
				break;
			case '/':
				if(CURSOR[1] != '/' && CURSOR[1] != '*')
					return;
				CURSOR++;
				if(*CURSOR == '/') //单行注释
				{
					CURSOR++;
					while(*CURSOR != '\n' && *CURSOR != END_OF_FILE)
					{
						CURSOR++;
					}
				}
				else //多行注释
				{
					CURSOR += 2;
					while(CURSOR[0] != '*' || CURSOR[1] != '/')
					{
						if(*CURSOR == '\n')
						{
							TokenCoord.ppline++;
							LINE++;
						}
						else if(CURSOR[0] == END_OF_FILE || CURSOR[1] == END_OF_FILE)
						{
							Error(&TokenCoord, "Comment is not closed");
							return;
						}
						CURSOR++;
					}
					CURSOR += 2;
				}
				break;
			default:
				CURSOR++;
				break;
		}
		ch = *CURSOR;
	}  //end while

	return;
}

int GetNextToken(void)
{
	int tok;
	PrevCoord = TokenCoord;
   	SkipWhiteSpace();
	TokenCoord.line = LINE;
	TokenCoord.col = (int)(CURSOR - LINEHEAD);

	tok = (*Scanners[*CURSOR])();
	return tok; 	
}
