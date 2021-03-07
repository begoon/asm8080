/*	*************************************************************************
 *	Module Name:	exp_parser.h
 *	Description:	Expression Parser module.
 *	Copyright(c):	See below...
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2010
 *	Last modified:	4 January 2012
 *	Notes:
 *	************************************************************************* */

/*
 * Copyright (c) <2007-2017> Jay Cotton<lbmgmusic@gmail.com>
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

#ifndef _EXP_PARSER_H
#define _EXP_PARSER_H


/*	*************************************************************************
 *	                              INCLUDE FILES
 *	************************************************************************* */

#include "project.h"


/*	*************************************************************************
 *	                          FUNCTIONS DECLARATION
 *	************************************************************************* */

int extract_byte(char *text);
int extract_word(char *text);
int exp_parser(char *text);





#endif


