/*	*************************************************************************
 *	Module Name:	util.c
 *	Description:	Utilities.
 *	Copyright(c):	See below...
 *	Author(s):		Claude Sylvain
 *	Created:			23 December 2010
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
#include "err_code.h"
#include "main.h"
#include "msg.h"
#include "util.h"


/*	*************************************************************************
 *	                           FUNCTIONS DEFINITION
 *	************************************************************************* */


/*	*************************************************************************
 *	Function name:	islabelchar
 *	Description:	Tell if character can be a label character.
 *	Author(s):		Claude Sylvain
 *	Created:			8 January 2011
 *	Last modified:	25 December 2011
 *
 *	Parameters:		int c:
 *							The character to test.
 *
 *	Returns:			int:
 *							0		: Not a label character.
 *							!= 0	: Is a label character.
 *
 *	Globals:
 *
 *	Notes:			- Do not check for label/name first character, that
 *						  can be '?' or '@' special character.
 *
 *						- '_' character is not accepted as a valid character in
 *						  legacy 8080 assemblers.
 *	************************************************************************* */

int islabelchar(int c)
{
/*	If Accepting Underscore Character in Label/Name.
 *	------------------------------------------------ */	
#if ACCEPT_UNDERSCORE_CHAR_IN_LN
	return ((isalnum(c) != 0) || (c == '_'));
#else
	return (isalnum(c) != 0);
#endif
}


/*	*************************************************************************
 *	Function name:	check_evor
 *
 *	Description:	- Check for Expression Value Over Range, and display/print
 *						  an error message if an over range is detected.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			4 December 2011
 *	Last modified:	27 December 2011
 *
 *	Parameters:		int value:
 *							Expression Value to check.
 *
 *						int limit:
 *							Expression value Limit.
 *
 *	Returns:			int:
 *							-1	: Over Range.
 *							0	: Good Range.
 *
 *	Globals:			None.
 *	Notes:
 *	************************************************************************* */

int check_evor(int value, int limit)
{
	int	rv	= 0;

//	if ((value > limit) && (asm_pass == 1))
	if (value > limit)
	{
		msg_error_d("Expression value over range!", EC_EVOR, value);
		rv	= -1;
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	check_oor
 *
 *	Description:	- Check for Operand Over Range, and display/print
 *						  an error message if an over range is detected.
 *
 *	Author(s):		Claude Sylvain
 *	Created:			24 December 2010
 *	Last modified:	27 December 2011
 *
 *	Parameters:		int value:
 *							Value to check.
 *
 *						int limit:
 *							Operand Limit.
 *
 *	Returns:			int:
 *							-1	: Over Range.
 *							0	: Good Range.
 *
 *	Globals:			None.
 *	Notes:
 *	************************************************************************* */

int check_oor(int value, int limit)
{
	int	rv	= 0;

//	if ((value > limit) && (asm_pass == 1))
	if (value > limit)
	{
		msg_error_d("Operand over range!", EC_OOR, value);
		rv	= -1;
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	FindLabel
 *	Description:	Find a Label (actually a symbol).
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2011
 *
 *	Parameters:		char *text:
 *							"text" that possibly contain label.
 *
 *	Returns:			SYMBOL *:
 *							Pointer to the structure that hold symbol.
 *
 *	Globals:
 *	Notes:			Linked list, linear search, string compare real dumb.
 *	************************************************************************* */

SYMBOL *FindLabel(char *text)
{
	char		tmp[SYMBOL_SIZE_MAX];

	SYMBOL	*Local	= Symbols;
	int		i			= 0;


	/*	TODO: Is this standard Intel assembler code?
	 *	-------------------------------------------- */	
	if (*text == '&')	tmp[i++] = *text++;
	if (*text == '%')	tmp[i++] = *text++;

	/*	- First label/name character can be '?' or '@'
	 *	  special character.
	 *	---------------------------------------------- */
	if ((*text == '?') || (*text == '@'))
		tmp[i++] = *(text++);

	while (islabelchar((int) *text) != 0)
	{
		tmp[i] = *text++;

		/*	Check for buffer overflow.
		 *	-------------------------- */	
		if (++i >= sizeof (tmp))
		{
			--i;
			msg_error("\"FindLabel\" buffer overflow!", EC_FLBOF);
			break;
		}
	}

	tmp[i] = '\0';					/*	String delimitor. */

	while (Local->next)
  	{
		if (!strcmp(Local->Symbol_Name, tmp))
			return (Local);

		Local = (SYMBOL *) Local->next;
	}

	return (NULL);
}


/*	*************************************************************************
 *	Function name:	AddLabel
 *	Description:	Add a Label to the label's linked list.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2011
 *
 *	Parameters:		char *label:
 *							String that contain Label to Add.
 *
 *	Returns:			int:
 *							-1	: Can not add label because it already exist.
 *							0	: Label/name added successfully.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int AddLabel(char *label)
{
	int		rv			= 0;
	SYMBOL	*Local	= Symbols;
	int		phantom	= 0;


	if (*label == '&')
		label++;

	if (*label == '%')
	{
		label++;
		phantom++;
	}

	/*	If label/name already exist...
	 *	------------------------------ */
	if (FindLabel(label) != NULL)
	{
		if (phantom == 0)
		{
			if (list != NULL)
			{
				/*	- Notes: Since this error is printed on assembler pass #1 only,
				 *	  it is printed at beginning of the listing file.
				 *	  So, we have to tell where is the error, by adding the
				 *	  line number to the print out.
				 *	*/
				fprintf(	list,
					  		"*** Error %d in \"%s\" @%d: Duplicate Label (%s)!\n",
						  	EC_DL, in_fn[file_level], codeline[file_level], label);
			}

			fprintf(	stderr,
				  		"*** Error %d in \"%s\" @%d: Duplicate Label (%s)!\n",
					  	EC_DL, in_fn[file_level], codeline[file_level], label);
		}

		return (-1);
	}


	/* Now add it to the list.
	 *	*********************** */

	/* find end of list.
	 * ----------------- */
	while (Local->next)
		Local = (SYMBOL *) Local->next;


	/*	Store symbol name.
	 *	****************** */	

	/*	Allocate memory.
	 *	*/	
	Local->Symbol_Name = (char *) malloc(strlen(label) + 1);

	/*	Check for memory allocation error.
	 *	If no memory allocation error, store the symbol name.
 	 *	----------------------------------------------------- */	 
	if (Local->Symbol_Name != NULL)
	{
		strcpy(Local->Symbol_Name, label);
	}
	/*	Can not allocate memory :-(
	 *	--------------------------- */	
	else
		msg_error_s("Can't allocate memory!", EC_CAM, label);


	/*	Store source file name in which symbol is located.
	 *	************************************************** */	

	/*	Allocate memory.
	 *	*/	
	Local->src_filename = (char *) malloc(strlen(in_fn[file_level]) + 1);

	/*	Check for memory allocation error.
	 *	If no memory allocation error, store the source file name.
 	 *	---------------------------------------------------------- */	 
	if (Local->src_filename != NULL)
	{
		strcpy(Local->src_filename, in_fn[file_level]);
	}
	/*	Can not allocate memory :-(
	 *	--------------------------- */	
	else
		msg_error_s("Can't allocate memory!", EC_CAM, label);


	/*	Store source file line number on which the symbol is located.
	 *	*/
	Local->code_line		= codeline[file_level];

	/*	- Check for memory allocation error, and display error message
	 *	  if necessary.
 	 *	- Notes: When not able to allocate memory for new symbol, this
	 *	  will make the last symbol in the linked list destroyed next
	 *	  time a symbol will be added :-(
	 *	-------------------------------------------------------------- */
	if ((Local->next = (SYMBOL *) calloc(1, sizeof(SYMBOL))) == NULL)
		msg_error_s("Can't allocate memory!", EC_CAM, label);

	return (rv);
}


/*	*************************************************************************
 *	Function name:	process_label
 *	Description:	Process Label.
 *	Author(s):		Claude Sylvain
 *	Created:			28 December 2010
 *	Last modified:	27 December 2011
 *
 *	Parameters:		char *label:
 *							Point to a string that hold label.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void process_label(char *label)
{
	SYMBOL	*Local;

	/*	If no label, do nothing.
	 *	*/	
	if (*label == '\0')	return;

	/*	If in first assembly pass, add and initialize label.
	 *	---------------------------------------------------- */	
	if (asm_pass == 0)
	{
		/*	Add label/name.
		 *	If able to add it, set its value.
	 	 *	--------------------------------- */	 
		if (AddLabel(label) != -1)
		{
			Local = FindLabel(label);

			if (Local != NULL)
			{
				Local->Symbol_Value	= target.pc;
				Local->Symbol_Type	= SYMBOL_LABEL;
			}
		}
	}
	/*	We assume we are in second assembler pass...
	 * Check for phasing error.	
	 *	In case there is phasing error, synchronize the label.
	 *	------------------------------------------------------ */	
	else
	{
		Local = FindLabel(label);

		if ((Local != NULL) && (Local->Symbol_Value != target.pc))
		{
			msg_error_s("Phasing error!", EC_PE, label);

			/*	Sync label value.
			 *	----------------- */
			Local->Symbol_Value	= target.pc;
			Local->Symbol_Type	= SYMBOL_LABEL;
		}
	}
}


/*	*************************************************************************
 *	Function name:	util_get_number_base_inc
 *	Description:	Get Number Base Increment.
 *	Author(s):		Claude Sylvain
 *	Created:			23 December 2010
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *text:
 *							Text that contain number.
 *
 *	Returns:			int:
 *							Base Increment (2, 8, 10, 16)
 *							or
 *							(-1) if an error as occured.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int util_get_number_base_inc(char *text)
{
	int	rv			= -1;
	int	c_num		= 0;
	int	c;


	/*	Search for the last number character.
	 *	------------------------------------- */	
	while ((*text != '\0') && (isalnum((int) *text) != 0))
	{
		text++;
		c_num++;			/*	Keep track of number of characters. */
	}

	/*	Process only if a number exist.
	 *	------------------------------- */	
	if (c_num > 0)
	{
		--text;								/*	Pos. to last character. */
		c	= toupper((int) *text);		/*	Working copy of last character. */

		/*	- If last character is a decimal digit, this mean that
		 *	  the number is a decimal number with no numerical base
	 	 *	  qualifier.
		 * ------------------------------------------------------- */	 
		if (isdigit(c))
		{
			rv	= 10;
		}
		/*	- This is not a number expressed in the default
		 *	  decimal base.
	 	 *	- The last character must be a numerical base
		 *	  specifier (B, Q, D, H).
		 *	----------------------------------------------- */
		else
		{
			/*	- If hold at least 1 digit for the number,
			 *	  go further more.
			 *	------------------------------------------ */	
			if (c_num > 1)
			{
				/*	- Replace the numerical base specifier by a delimitor.
				 *	- Notes: This is necessary to avoid the caller
				 *	  to have to process that character.
				 *	*/
				*text	= '\0';

				/*	Set increment, depending on the numerical specifier.
				 *	---------------------------------------------------- */	
				switch(c)
				{
					case 'B':	rv	= 2;	break;	/*	Binary base. */
					case 'Q':	rv	= 8;	break;	/*	Octal base. */
					case 'D':	rv	= 10;	break;	/*	Decimal base. */
					case 'H':	rv	= 16;	break;	/*	Hexadecimal base. */
					default:					break;	
				}
			}
		}
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	AdvanceTo
 *	Description:
 *	Author(s):		Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2010
 *
 *	Parameters:		char *text:
 *							...
 *
 * 					char x:
 *							...
 *
 *	Returns:			char *:
 *							...
 *
 *	Globals:
 *
 *	Notes:
 *	- For DB, and DW, there may be an unbounded list of hex, dec, and octal
 *	  bytes/words, and quoted strings.
 *	************************************************************************* */

char *AdvanceTo(char *text, char x)
{
	while (*text != '\0')
  	{
		if (*text != x)
			text++;
		else
			return (text);
	}

	return (text--);
}


/*	*************************************************************************
 *	Function name:	AdvancePast
 *	Description:
 *	Author(s):		Claude Sylvain
 *	Created:			2007
 *	Last modified:	23 December 2010
 *
 *	Parameters:		char *text:
 *							...
 *
 * 					char x:
 *							...
 *
 *	Returns:			char *:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

char *AdvancePast(char *text, char x)
{
	text = AdvanceTo(text, x);
	return (++text);
}


/*	*************************************************************************
 *	Function name:	AdvanceToAscii
 *	Description:
 *	Author(s):		Claude Sylvain
 *	Created:			2007
 *	Last modified:	23 December 2010
 *
 *	Parameters:		char *text:
 *							...
 *
 *	Returns:			char *:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

char *AdvanceToAscii(char *text)
{
	/*	i.e. not a space.
	 *	----------------- */	
	while (isspace((int) *text))
		text++;

	return (text);
}


/*	*************************************************************************
 *	Function name:	AdvanceToAscii
 *	Description:
 *	Author(s):		Claude Sylvain
 *	Created:			2007
 *	Last modified:	23 December 2010
 *
 *	Parameters:		char *text:
 *							...
 *
 *	Returns:			char *:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

char *AdvancePastSpace(char *text)
{
	return (AdvanceToAscii(text));
}


/*	*************************************************************************
 *	Function name:	util_is_cs_enable
 *	Description:	Tell if Code Section is Enabled or not.
 *	Author(s):		Claude Sylvain
 *	Created:			12 February 2011
 *	Last modified:
 *	Parameters:		void
 *
 *	Returns:			int:
 *							0	: Code Section innactive.
 *							1	: Code Section active.
 *
 *	Globals:				int if_true[]
 *							int if_nest
 *
 *	Notes:
 *	************************************************************************* */

int util_is_cs_enable(void)
{
	int	rv	= 1;		/*	Enable (default). */
	int	i;

	/*	- Search for disabled code section.
	 *	- If disabled code section is found, set return value
 	 *	  accordingly, and exit loop.
	 *	- Notes: If "if" level is the base level (0), do
	 *	  nothing, and exit with 1 (code section enable).
	 *	  This is normal, since "if" base level is always
	 *	  1.
	 *	----------------------------------------------------- */	 
	for (i = if_nest; i > 0; i--)
	{
		if (if_true[i] == 0)
		{
			rv	= 0;
			break;
		}
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	byte_to_hex
 *	Description:	Convert a byte (8-bit data) to an Hexadecimal string.
 *	Author(s):		Claude Sylvain
 *	Created:			17 December 2011
 *	Last modified:
 *
 *	Parameters:		uint8_t byte:
 *							8-bit data to convert to hexadecimal.
 *
 *						char *string:
 *							- Point to a string that will be filled with
 *							  the hexadecimal representation of "byte".
 *							- Notes: "string" must have a minimal size
 *							  of 3 bytes (2 digits + string delimitor).
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void byte_to_hex(uint8_t byte, char *string)
{
	int		i;
	uint8_t	nibble;

	/*	Process all nibbles.
	 *	-------------------- */	
	for (i = 1; i >= 0; i--)
	{
		nibble	= byte & 0x0F;		/*	Get the nibble. */

		/*	Convert and store nibble.
		 *	------------------------- */	
		if (nibble >= 10)
			string[i]	= (char) (nibble + ('A' - 10));
		else
			string[i]	= (char) (nibble + '0');

		byte	>>= 4;			/*	Select next nibble. */
	}

	string[2]	= '\0';		/*	Add string delimitor. */
}


/*	*************************************************************************
 *	Function name:	word_to_hex
 *	Description:	Convert a word (16-bit data) to an Hexadecimal string.
 *	Author(s):		Claude Sylvain
 *	Created:			17 December 2011
 *	Last modified:
 *
 *	Parameters:		uint16_t word:
 *							16-bit data to convert to hexadecimal.
 *
 *						char *string:
 *							- Point to a string that will be filled with
 *							  the hexadecimal representation of "word".
 *							- Notes: "string" must have a minimal size
 *							  of 5 bytes (4 digits + string delimitor).
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

void word_to_hex(uint16_t word, char *string)
{
	byte_to_hex((uint8_t) (word >> 8), string);
	byte_to_hex((uint8_t) (word & 0x00FF), &string[2]);
}






