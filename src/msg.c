/*	*************************************************************************
 *	Module Name:	msg.c
 *	Description:	Manage Messages.
 *	Copyright(c):	See below...
 *	Author(s):		Claude Sylvain
 *	Created:			1 January 2012
 *	Last modified:	4 January 2012
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
 *	                              INCLUDE FILES
 *	************************************************************************* */

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

#include "project.h"
#include "main.h"
#include "msg.h"


/*	*************************************************************************
 *												 CONST
 *	************************************************************************* */

/*	Private "const".
 *	**************** */	

static const char	*str_err	= "Error";
static const char	*str_war	= "Warning";


/*	*************************************************************************
 *									  FUNCTIONS DECLARATION
 *	************************************************************************* */

/*	Private functions.
 *	****************** */

static void print_msg(const char *str_type, char *msg, int code);
static void print_msg_c(const char *str_type, char *msg, int code, char c);
static void print_msg_s(const char *str_type, char *msg, int code, char *s);
static void print_msg_d(const char *str_type, char *msg, int code, int d);


/*	*************************************************************************
 *	                           FUNCTIONS DEFINITION
 *	************************************************************************* */


/*	*************************************************************************
 *	Function name:	print_msg
 *	Description:	Print Message, without additionnal information.
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		const char *str_type:
 *							String that hold message type.
 *
 *						char *msg:
 *							Message to display.
 *
 *						int code:
 *							Error or warning Code.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void print_msg(const char *str_type, char *msg, int code)
{
	/*	Print message only on second assembly pass.
	 *	*/	
	if (asm_pass != 1)	return;

	if (list != NULL)
	{
		fprintf(	list,
						"*** %s %d in \"%s\": %s\n", str_type,
					code, in_fn[file_level], msg);
	}

	fprintf(	stderr,
				"*** %s %d in \"%s\" @%d: %s\n", str_type,
				code, in_fn[file_level], codeline[file_level],
				msg);
	exit(1);
}


/*	*************************************************************************
 *	Function name:	print_msg_c
 *
 *	Description:	- Print Error Message, with additionnal information using
 *						  a character.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		const char *str_type:
 *							String that hold message type.
 *
 *						char *msg:
 *							Message to display.
 *
 *						int code:
 *							Error or warning Code.
 *
 *						char c:
 *							Character to display.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void print_msg_c(const char *str_type, char *msg, int code, char c)
{
	/*	Print message only on second assembly pass.
	 *	*/	
	if (asm_pass != 1)	return;

	if (list != NULL)
	{
		fprintf(	list,
					"*** %s %d in \"%s\": %s (\"%c\")\n", str_type,
					code, in_fn[file_level], msg, c);
	}

	fprintf(	stderr,
				"*** %s %d in \"%s\" @%d: %s (\"%c\")\n", str_type,
				code, in_fn[file_level], codeline[file_level], msg, c);
	exit(1);
}


/*	*************************************************************************
 *	Function name:	print_msg_s
 *
 *	Description:	- Print Error Message, with additionnal information using
 *						  a string.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		const char *str_type:
 *							String that hold message type.
 *
 *						char *msg:
 *							Message to display.
 *
 *						int code:
 *							Error or warning Code.
 *
 *						char *s:
 *							String to display.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void print_msg_s(const char *str_type, char *msg, int code, char *s)
{
	/*	Print message only on second assembly pass.
	 *	*/	
	if (asm_pass != 1)	return;

	if (list != NULL)
	{
		fprintf(	list,
					"*** %s %d in \"%s\": %s (\"%s\")\n", str_type,
					code, in_fn[file_level], msg, s);
	}

	fprintf(	stderr,
				"*** %s %d in \"%s\" @%d: %s (\"%s\")\n", str_type,
				code, in_fn[file_level], codeline[file_level], msg, s);
	exit(1);
}


/*	*************************************************************************
 *	Function name:	print_msg_d
 *
 *	Description:	- Print Error Message, with additionnal information using
 *						  an integer displayed in Decimal.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		const char *str_type:
 *							String that hold message type.
 *
 *						char *msg:
 *							Message to display.
 *
 *						int code:
 *							Error or warning Code.
 *
 *						int d:
 *							Integer to display.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void print_msg_d(const char *str_type, char *msg, int code, int d)
{
	/*	Print message only on second assembly pass.
	 *	*/	
	if (asm_pass != 1)	return;

	if (list != NULL)
	{
		fprintf(	list,
					"*** %s %d in \"%s\": %s (%d)\n", str_type,
					code, in_fn[file_level], msg, d);
	}

	fprintf(	stderr,
				"*** %s %d in \"%s\" @%d: %s (%d)\n", str_type,
				code, in_fn[file_level], codeline[file_level], msg, d);
	exit(1);
}


/*	*************************************************************************
 *	Function name:	msg_error
 *	Description:	Print Error Message, without additionnal information.
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		char *msg:
 *							Message to display.
 *
 *						int err_code:
 *							Error Code.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void msg_error(char *msg, int err_code)
{
	print_msg(str_err, msg, err_code);
}


/*	*************************************************************************
 *	Function name:	msg_error_c
 *
 *	Description:	- Print Error Message, with additionnal information using
 *						  a character.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		char *msg:
 *							Message to display.
 *
 *						int err_code:
 *							Error Code.
 *
 *						char c:
 *							Character to display.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void msg_error_c(char *msg, int err_code, char c)
{
	print_msg_c(str_err, msg, err_code, c);
}


/*	*************************************************************************
 *	Function name:	msg_error_s
 *
 *	Description:	- Print Error Message, with additionnal information using
 *						  a string.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		char *msg:
 *							Message to display.
 *
 *						int err_code:
 *							Error Code.
 *
 *						char *s:
 *							String to display.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void msg_error_s(char *msg, int err_code, char *s)
{
	print_msg_s(str_err, msg, err_code, s);
}


/*	*************************************************************************
 *	Function name:	msg_error_d
 *
 *	Description:	- Print Error Message, with additionnal information using
 *						  an integer displayed as Decimal.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		char *msg:
 *							Message to display.
 *
 *						int err_code:
 *							Error Code.
 *
 *						int d:
 *							Integer to display.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void msg_error_d(char *msg, int err_code, int d)
{
	print_msg_d(str_err, msg, err_code, d);
}


/*	*************************************************************************
 *	Function name:	msg_warning
 *	Description:	Print Warning Message, without additionnal information.
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		char *msg:
 *							Message to display.
 *
 *						int war_code:
 *							Warning Code.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void msg_warning(char *msg, int war_code)
{
	print_msg(str_war, msg, war_code);
}


/*	*************************************************************************
 *	Function name:	msg_warning_c
 *
 *	Description:	- Print Warning Message, with additionnal information using
 *						  a character.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		char *msg:
 *							Message to display.
 *
 *						int war_code:
 *							Warning Code.
 *
 *						char c:
 *							Character to display.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void msg_warning_c(char *msg, int war_code, char c)
{
	print_msg_c(str_war, msg, war_code, c);
}


/*	*************************************************************************
 *	Function name:	msg_warning_s
 *
 *	Description:	- Print Warning Message, with additionnal information using
 *						  a string.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		char *msg:
 *							Message to display.
 *
 *						int war_code:
 *							Warning Code.
 *
 *						char *s:
 *							String to display.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void msg_warning_s(char *msg, int war_code, char *s)
{
	print_msg_s(str_war, msg, war_code, s);
}






