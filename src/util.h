/*	*************************************************************************
 *	Module Name:	util.h
 *	Description:	Utilities.
 *	Copyright(c):	See below...
 *	Author(s):		Claude Sylvain
 *	Created:			23 December 2010
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

#ifndef _UTIL_H
#define _UTIL_H


/*	*************************************************************************
 *	                              INCLUDE FILES
 *	************************************************************************* */

#include "project.h"


/*	*************************************************************************
 *	                          FUNCTIONS DECLARATION
 *	************************************************************************* */

int islabelchar(int c);
int check_evor(int value, int limit);
int check_oor(int value, int limit);
SYMBOL *FindLabel(char *text);
int AddLabel(char *label);
void process_label(char *label);
int util_get_number_base_inc(char *text);
char *AdvanceTo(char *text, char x);
char *AdvancePast(char *text, char x);
char *AdvanceToAscii(char *text);
char *AdvancePastSpace(char *text);
int util_is_cs_enable(void);
void byte_to_hex(uint8_t byte, char *string);
void word_to_hex(uint16_t word, char *string);


#endif


