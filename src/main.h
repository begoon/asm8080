/*	*************************************************************************
 *	Module Name:	main.h
 *	Description:	Main module for Intel 8080 Assembler.
 *	Copyright(c):	See below...
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
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

#ifndef _MAIN_H
#define _MAIN_H


/*	*************************************************************************
 *	                              INCLUDE FILES
 *	************************************************************************* */

#include <stdio.h>

#include "project.h"


/*	*************************************************************************
 *											  CONSTANTS
 *	************************************************************************* */

/*	KeyWord Processing Status.
 *	-------------------------- */	
#define KWPS_NONE						0			/*	No Status. */
#define BINARY_TO_DUMP				1
#define PROCESSED_END				2
#define COMMENT						3
#define TEXT							4
#define LIST_ONLY						5
#define LIST_DS						6			/*	List Data Storage ("DS"). */
#define LIST_BYTES					7
#define LIST_WORDS					8
#define LIST_STRINGS					9


/*	*************************************************************************
 *												 CONST
 *	************************************************************************* */

extern const char	*name_pgm;		/*	Program Name. */


/*	*************************************************************************
 *	                                VARIABLES
 *	************************************************************************* */

extern int	if_true[10];
extern int	if_nest;

extern int	addr;
extern int	data_size;

extern int	b1;
extern int	b2;
extern int	b3;
extern int	b4;

extern int	asm_pass;		/* Assembler Pass. */

extern FILE	*list;

extern STACK	*ByteWordStack;
extern TARG		target;

extern int	type;
extern int	file_level;
extern FILE	*in_fp[FILES_LEVEL_MAX];
extern char	*in_fn[FILES_LEVEL_MAX];		/*	Input File Name. */
extern int codeline[FILES_LEVEL_MAX];

extern SYMBOL	*Symbols;

extern char	*empty_string;


/*	*************************************************************************
 *	                          FUNCTIONS DECLARATION
 *	************************************************************************* */

int set_pc(int pc_value);
int get_file_from_path(char *fn, char* fn_path, size_t fn_path_size);
void ProcessDumpBin(void);
void ProcessDumpHex(char end_of_asm);



#endif


