/*
 * =====================================================================================
 *
 *       Filename:  ucl.c
 *
 *    Description:  主程序
 *
 *        Version:  1.0
 *        Created:  03/22/2015 06:36:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WYL (502), ylwzzu@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */


/* 测试每个函数 */

#include "ucl.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf("usage complie <filename>\n");
		exit(0);
	}
	readSourceFile(argv[1]);

	return 0
}
