/*
 * =====================================================================================
 *
 *       Filename:  error.c
 *
 *    Description:  t
 *
 *        Version:  1.0
 *        Created:  03/22/2015 05:07:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WYL (502), ylwzzu@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ucl.h"

void Fatal(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	exit(-1);
}
