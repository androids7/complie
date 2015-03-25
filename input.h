/*
 * =====================================================================================
 *
 *       Filename:  input.h
 *
 *    Description:  输入源程序
 *
 *        Version:  1.0
 *        Created:  03/22/2015 02:07:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WYL (502), ylwzzu@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  input_INC
#define  input_INC

typedef struct coord
{
	char *filename;
	int ppline;
	int line;
	int col;
}*Coord;    //行列信息

struct input
{
	char *filename;
	unsigned char *base;  	 /* 映射的文件基指针 */
	unsigned char *cursor;	 /* 读取文件当前位置 */
	unsigned char *linehead; /* 读取一行*/
	int line;
	void *file;
	unsigned long size;
};


extern struct input Input;
extern unsigned char END_OF_FILE;

void readSourceFile(char *filename);
void closeSourceFile(void);

#endif   /* ----- #ifndef input_INC  ----- */
					
