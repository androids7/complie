/*
 * =====================================================================================
 *
 *       Filename:  error.h
 *
 *    Description:  错误处理头文件
 *
 *        Version:  1.0
 *        Created:  03/22/2015 01:56:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WYL (502), ylwzzu@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  error_INC
#define  error_INC

void error(char *reson, int num);

void Fatal(const char *format, ...);

#endif   /* ----- #ifndef error_INC  ----- */
