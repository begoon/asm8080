/*	*************************************************************************
 *	Module Name:	msg.h
 *	Description:	Manage Messages.
 *	Copyright(c):	See below...
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:	1 January 2012
 *	Notes:
 *	************************************************************************* */

/*
 * Copyright (c) <2007-2012> <jay.cotton@oracle.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


/*	*************************************************************************
 *	************************************************************************* */

#ifndef _MSG_H
#define _MSG_H


/*	*************************************************************************
 *	                              INCLUDE FILES
 *	************************************************************************* */

#include "project.h"


/*	*************************************************************************
 *	                          FUNCTIONS DECLARATION
 *	************************************************************************* */

void msg_error(char *msg, int err_code);
void msg_error_c(char *msg, int err_code, char c);
void msg_error_s(char *msg, int err_code, char *s);
void msg_error_d(char *msg, int err_code, int d);

void msg_warning(char *msg, int war_code);
void msg_warning_c(char *msg, int war_code, char c);
void msg_warning_s(char *msg, int war_code, char *s);




#endif


