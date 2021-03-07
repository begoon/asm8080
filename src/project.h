/*	*************************************************************************
 *	Module Name:	project.h
 *	Description:	Project header file.
 *	Copyright(c):	See below...
 *	Author(s):		Claude Sylvain
 *	Created:			2010-12-11
 *	Last modified:	2015-08-30
 *	Notes:
 *	************************************************************************* */

/*
 * Copyright (c) <2007-2017> <lbmgmusic@gmail.com>
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

#ifndef _PROJECT_H
#define _PROJECT_H


/*	*************************************************************************
 *												INCLUDE
 *	************************************************************************* */

#if	defined (_TGT_OS_CYGWIN32) || defined (_TGT_OS_CYGWIN64) ||			\
		defined (_TGT_OS_LINUX32) || defined (_TGT_OS_LINUX64) ||			\
		defined (_TGT_OS_SOLARIS32) || defined (_TGT_OS_SOLARIS64)
#include <stdint.h>
#endif

#if	defined (_TGT_OS_CYGWIN32) || defined (_TGT_OS_CYGWIN64) ||			\
		defined (_TGT_OS_LINUX32) || defined (_TGT_OS_LINUX64) ||			\
		defined (_TGT_OS_WIN32) || defined (_TGT_OS_WIN64)
#include <string.h>
#elif defined (_TGT_OS_SOLARIS32) || defined (_TGT_OS_SOLARIS64)
#include <strings.h>
#else
#error !!! Unsupported operating system!
#endif

#if	defined (_TGT_OS_CYGWIN32) || defined (_TGT_OS_CYGWIN64) ||			\
		defined (_TGT_OS_LINUX32) || defined (_TGT_OS_LINUX64)
#include <unistd.h>
#endif


/*	*************************************************************************
 *											  CONSTANTS
 *	************************************************************************* */

#define SRC_LINE_WIDTH_MAX			256		/*	Source Line Maximum Width. */
#define FILES_LEVEL_MAX				16			/*	Maximum Files Level. */

#define EQUATION_SIZE_MAX			160		/*	Equation maximum size. */
#define SYMBOL_SIZE_MAX				64			/*	Symbol Maximum Size. */

/*	Label Maximum Size.
 *	Notes: This value is (only) 6 in some 8080 legacy assemblers.
 *	*/	
#define LABEL_SIZE_MAX				SYMBOL_SIZE_MAX

/*	When "1", accept '_' character in label/name.
 *	*/	
#define ACCEPT_UNDERSCORE_CHAR_IN_LN	1

/*	Define to 1 to make "asm8080" support language extension.
 *	Language extension are:
 *		"C" like operators:
 *			>>, <<, ==, =, <, <=, >, >=, , ~, |, ||, ^, &, &&
 *		Pascal like operators:
 *			=
 *	*/
#define LANG_EXTENSION						1

/*	Temporary debug.
 *	*/	
#define DEBUG_TMP_ACTIVE					0


/*	*************************************************************************
 *												 MACROS
 *	************************************************************************* */

/*	Define Path Separator accordingly to the operating system.
 *	---------------------------------------------------------- */
#if	defined (_TGT_OS_CYGWIN32) || defined (_TGT_OS_CYGWIN64) ||			\
		defined (_TGT_OS_LINUX32) || defined (_TGT_OS_LINUX64) ||			\
		defined (_TGT_OS_SOLARIS32) || defined (_TGT_OS_SOLARIS64)
#define PATH_SEPARATOR_CHAR	'/'
#define PATH_SEPARATOR_STR		"/"
#elif defined (_TGT_OS_WIN32) || defined (_TGT_OS_WIN64)
#define PATH_SEPARATOR_CHAR	'\\'
#define PATH_SEPARATOR_STR		"\\"
#else
#error !!! Unsupported operating system!
#endif


/*	*************************************************************************
 *	                                 TYPEDEF
 *	************************************************************************* */

/*	- In Windows, "stdint.h" do not exist and some types found in
 *	  "stdint.h" must be added.
 *	------------------------------------------------------------- */
#if defined (_TGT_OS_WIN32) || defined (_TGT_OS_WIN64)
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int		uint32_t;
#endif

/*	Types of Symbols.
 *	----------------- */	
enum symbol_type_t
{
	SYMBOL_LABEL,
	SYMBOL_NAME,
	SYMBOL_NAME_EQU,
	SYMBOL_NAME_SET
};

/*	Symbol table storage.
 *	--------------------- */
typedef struct Symbol
{
	char	*Symbol_Name;
	int	Symbol_Value;
	enum	symbol_type_t	Symbol_Type;
	int	code_line;
	char	*src_filename;
	void	*next;
} SYMBOL;

/* Binary stack for lister.
 *	------------------------ */
typedef struct Stack
{
	int	word;
	void	*next;
} STACK;

typedef struct Instructions
{
	const char	*Name;
	int			(*fnc) (char *, char *);
} keyword_t;

/*	Targeted binary handling storage.
 *	*/
typedef struct targ
{
	int	addr;			/*	Address. */
	int	pc;			/*	Program Counter. */
	int	pc_org;		/*	Program Counter, value set by "ORG" directive. */
	int	pc_lowest;	/*	Program Counter, Lowest value. */
	int	pc_highest;	/*	Program Counter, Highest value. */
} TARG;


#endif






