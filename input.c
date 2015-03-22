/*
 * =====================================================================================
 *
 *       Filename:  input.c
 *
 *    Description:  文件映射机制
 *
 *        Version:  1.0
 *        Created:  03/22/2015 02:17:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WYL (502), ylwzzu@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */


#include "input.h"
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct Input Input;
unsigned char END_OF_FILE = 255;


void readSourceFile(char *filename)
{
 #if define(_WCC)
	Input.file = fopen(filename, "r");

	if(Input.file <= 0)
	{
		Fatal("can't open the file: %s", filename);
	}	

	fseek(Input.file, 0, SEEK_END);
	Input.size = ftell(Input.file);  //文件开头到文件尾的偏移量
	Input.base = malloc(sizeof(Input.size) + 1);
	
	if(Input.base <= 0)
	{
		Fatal("can't malloc the %s space, too big\n", Input.filename);
		fclose(Input.filename);
	}

	fseek(Input.file, 0, SEEK_SET); //文件头0(SEEK_SET)，当前位置1(SEEK_CUR)，文件尾2(SEEK_END)
	Input.size = fread(Input.base, 1, Input.size, Input.file);
	fclose(Input.filename);
#else
	struct stat st;
	int fno;

	fno = fopen(filename, "O_RDWR");
	if(-1 == fno)
	{
		Fatal("can't open the file: %s", filename);
	}
	if(-1 == fstat(fno, &st))
	{
		Fatal("can't stat the file: %s", filename);
		fclose(fno);
	}	

	Input.size = st.size;

	Input.base = mmap(NULL, Input.size + 1, PROT_WRITE, MAP_PRIVATE, fno, 0);
	if(Input.base == MAP_FAILED)
	{
		Fatal("cann't mmap the file %s\n", filename);
		fclose(fno);
	}
	Input.file = (void *)fno;
#endif

	Input.filename = filname;
	Input.base[Input.size] = END_OF_FILE;
	Input.cursor = Input.linehead = Input.base;
	Input.line = 1;

	return;
}	

void closeSourceFile(void)
{
#if define(WCC)
   close(Input.file);
#else
	close(Input.file);
	munmap(Input.base, Input.size + 1);
#endif	
}

