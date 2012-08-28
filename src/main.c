/*	*************************************************************************
 *	Module Name:	main.c
 *	Description:	Main module for Intel 8080 Assembler.
 *	Copyright(c):	See below...
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	6 January 2012
 *
 * Notes:
 *						- The assembler assumes that the left column is a label,
 *						  the mid column is an opcode, and the right column is an
 *						  equation.
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
#include "err_code.h"		/*	Error Codes. */
#include "war_code.h"		/*	Warning Codes. */
#include "util.h"
#include "asm_dir.h"
#include "opcode.h"
#include "exp_parser.h"
#include "msg.h"
#include "main.h"


/*	*************************************************************************
 *											  CONSTANTS
 *	************************************************************************* */

#define FN_BASE_SIZE					80
#define FN_IN_SIZE					80
#define FN_OUT_SIZE					(FN_BASE_SIZE + 4)


/*	*************************************************************************
 *												 STRUCT
 *	************************************************************************* */

/*	Structure that hold an "-I" Option.
 *	----------------------------------- */
struct option_i_t
{
	char					*path;
	struct option_i_t	*next;
};


/*	*************************************************************************
 *												 CONST
 *	************************************************************************* */

/*	Public const.
 *	************* */

const char	*name_pgm	= "asm8080";		/*	Program Name. */


/*	Local const.
 *	************ */

/*	Program Version.
 *	---------------- */
static const unsigned char	pgm_version_v	= 1;	/*	Version. */
static const unsigned char	pgm_version_sv	= 0;	/*	Sub-Version. */
static const unsigned char	pgm_version_rn	= 5;	/*	Revision Number. */


/*	*************************************************************************
 *	                          FUNCTIONS DECLARATION
 *	************************************************************************* */

/*	Private functions.
 *	****************** */

static void check_new_pc(int count);
static int update_pc(int count);
static void print_symbols_table(void);
static void init(void);
static int process_option_i(char *text);
static int process_option_l(char *text);
static int process_option_o(char *text);
static int check_set_output_fn(void);
static int OpenFiles(void);
static void CloseFiles(void);
static void RewindFiles(void);
static void DumpBin(void);
static void do_asm(void);
static int print_symbols_type(	enum symbol_type_t symbol_type,
	  										int symbol_field_size, int tab_length);
static void PrintList(char *text);
static void display_help(void);
static int src_line_parser(char *text);
static void asm_pass1(void);
static void asm_pass2(void);
static void clean_up(void);
static int process_input_file(char *text);
static int cmd_line_parser(int argc, char *argv[]);


/*	*************************************************************************
 *	                                VARIABLES
 *	************************************************************************* */

/*	Public variables.
 *	***************** */

int	if_true[10];
int	if_nest		= 0;

int	data_size	= 0;

int	b1	= 0;
int	b2	= 0;
int	b3	= 0;
int	b4	= 0;

int asm_pass;			/* Assembler Pass. */

FILE	*list		= NULL;

STACK	*ByteWordStack;
TARG	target;

int	type;

FILE	*in_fp[FILES_LEVEL_MAX];
char	*in_fn[FILES_LEVEL_MAX];		/*	Input File Name. */
int	codeline[FILES_LEVEL_MAX];

FILE	*bin;
FILE	*hex;

int	file_level	= 0;


/* Global storage for assembler */

SYMBOL	*Symbols;

char Image[1024 * 64];

/*	- This "Empty String" is used for initializing string pointers, when
 *	  unable to initialize them using memory allocated dynamically.
 *	*/
char	*empty_string	= "";


/*	Private variables.
 *	****************** */

/*	TODO: Size this string dynamically.
 *	*/
static char	fn_base[FN_BASE_SIZE];

static char	*list_file	= NULL;		/*	Listing File name. */
static char	*bin_file	= NULL;		/*	Binary File name. */
static char	*hex_file	= NULL;		/*	Intel Hexadecimal File name. */

/*	Single linked list that old all "-I" options.
 *	*/	
static struct	option_i_t	option_i	= {NULL, NULL};


/*	*************************************************************************
 *	                           FUNCTIONS DEFINITION
 *	************************************************************************* */


/*	*************************************************************************
 *	Function name:	set_pc
 *	Description:	Set Program Counter.
 *	Author(s):		Claude Sylvain
 *	Created:			24 December 2011
 *	Last modified:
 *
 *	Parameters:		int pc_value:
 *							New Program Counter Value.
 *
 *	Returns:			int:
 *							-1	: Program Counter out of range.
 *							0	: Program counter updated successfully.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int set_pc(int pc_value)
{
	int	rv	= 0;				/*	Return Value. */

	target.addr	= pc_value;
	target.pc	= target.addr & 0xFFFF;

	/*	Check if program counter is valid.
	 *	---------------------------------- */	
	if ((target.addr < 0) || (target.addr > 0xFFFF))
		rv	= -1;

	return (rv);
}


/*	*************************************************************************
 *	Function name:	check_new_pc
 *	Description:	Check the New Program Counter value.
 *	Author(s):		Claude Sylvain
 *	Created:			4 December 2011
 *	Last modified:	24 December 2011
 *
 *	Parameters:		int count:
 *							Count that will be added to the program counter.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void check_new_pc(int count)
{
	int	new_pc	= target.addr + count;	/*	Calculate new PC value. */

	/*	- Check if the new program counter is out of range, and
	 *	  manage messages if necessary.
	 *	------------------------------------------------------- */	 
	if (((new_pc < 0) || (new_pc > 0x10000)) && (asm_pass == 1))
		msg_error_d("Program counter over range!", EC_PCOR, target.pc);
}


/*	*************************************************************************
 *	Function name:	update_pc
 *	Description:	Update Program Counter.
 *	Author(s):		Claude Sylvain
 *	Created:			4 December 2011
 *	Last modified:	30 December 2011
 *
 *	Parameters:		int count:
 *							Count to add to the program counter.
 *
 *	Returns:			int:
 *							-1	: - Can not update program counter, because
 *									 of an out of range error.  Program counter
 *									 is resetted to 0x0000.
 *
 *							0	: Program counter updated successfully.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int update_pc(int count)
{
	int	rv	= 0;				/*	Return Value. */

	/*	- Keep track of the lowest PC value, only if not processing
	 *	  "DS" assembler directive.
	 *	  Notes: This is necessary, since "DS" modify the PC, but
	 *	  do not generate code.
	 *	----------------------------------------------------------- */
	if ((type != LIST_DS) && (target.pc < target.pc_lowest))
		target.pc_lowest	= target.pc;

	target.addr	+= count;						/*	Update Address. */
	target.pc	= target.addr & 0xFFFF;		/*	Update Program Counter. */

	/*	- Keep track of the highest PC value, only if not processing
	 *	  "DS" assembler directive.
	 *	  Notes: This is necessary, since "DS" modify the PC, but
	 *	  do not generate code.
	 *	------------------------------------------------------------ */
	if (type != LIST_DS)
	{
		if (target.addr <= 0x10000)
		{
			/*	- Update Target PC Highest value only if current PC
			 *	  is higher.  This is necessary to handle properly
			 *	  program using multiple "ORG" directives, that are
			 *	  not necessarily in ascendant order.
			 *	--------------------------------------------------- */	  
			if (target.addr > target.pc_highest)
				target.pc_highest	= target.addr;
		}
		/*	- PC is not valid, and will be reseted to 0.  So, do the
		 *	  same with Target PC Highest value.
		 *	-------------------------------------------------------- */
		else
			target.pc_highest	= 0x10000;
	}

	/*	Check if program counter is out of range.
	 *	----------------------------------------- */	 
	if ((target.addr < 0) || (target.addr > 0xFFFF))
		rv	= -1;

	return (rv);
}


/*	*************************************************************************
 *	Function name:	get_file_from_path
 *	Description:	Get File name From Path.
 *	Author(s):		Claude Sylvain
 *	Created:			31 December 2010
 *	Last modified:	1 January 2010
 *
 *	Parameters:		char *fn:
 *							- Point to a string that hold file name to which
 *							  path must be applied.
 *
 *						char fn_path:
 *							- Point to a buffer that will receive the file
 *							  name prefixed by a path.
 *
 *						size_t fn_path_size:
 *							Size of "fn_path" buffer.
 *
 *	Returns:			void
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int get_file_from_path(char *fn, char* fn_path, size_t fn_path_size)
{
	static struct option_i_t	*p_option_i_cur;

	int		rv			= -1;

	/*	- If one the the parameter is NULL or 0, we assume that caller
	 *	  want us to initialize.
 	 *	-------------------------------------------------------------- */	 
	if ((fn == NULL) || (fn_path == NULL) || (fn_path_size == 0))
	{
		p_option_i_cur	= &option_i;
		rv					= 0;					/*	Success! */
	}
	/*	Caller want us to add a path to its file name.  So, do it...
	 *	------------------------------------------------------------ */	
	else
	{
		/*	If there is a file name path available, go further more...
		 *	---------------------------------------------------------- */	
		if (p_option_i_cur->next != NULL)
		{
			size_t	fn_len;

			/*	- Calculate the lenght of the string that will be put
			 *	  in the caller buffer.
			 *	*/	 
			fn_len	= strlen(p_option_i_cur->path) + strlen(fn) + 1;

			/*	If caller buffer is enough large, go further more...
			 *	---------------------------------------------------- */	
			if (fn_path_size >= fn_len)
			{
				/*	- Copy file name prefixed by path to the caller
				 *	  buffer.
			 	 *	----------------------------------------------- */	 
				strcpy(fn_path, p_option_i_cur->path);
				strcat(fn_path, fn);

				p_option_i_cur	= p_option_i_cur->next;		/*	Next path. */
				rv					= 0;								/*	Success! */
			}
			/*	Caller buffer too small :-(
			 *	--------------------------- */	
			else
			{
				fprintf(stderr, "*** Error %d: Buffer too small!\n", EC_BTS);
			}
		}
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	print_symbols_type
 *	Description:	Print Symbols having a specific type.
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2011
 *	Last modified:
 *
 *	Parameters:		enum symbol_type_t symbol_type:
 *							Symbol Type.
 *
 *						int symbol_field_size:
 *							Symbols Field Size.
 *
 *						int tab_length:
 *							Tab Length.	
 *
 *	Returns:			int:
 *							Number of symbol printed.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int print_symbols_type(	enum symbol_type_t symbol_type,
	  										int symbol_field_size, int tab_length)
{
	int		symbols_num				= 0;
	char		string_type[16];
	SYMBOL	*local	= Symbols;
	size_t	str_len;
	int		tab_cnt;
	int		i;


	/*	Set Type String.
	 *	---------------- */
	switch (symbol_type)
	{
		case SYMBOL_LABEL:
			strcpy(string_type, "Label");
			break;

		case SYMBOL_NAME:
			strcpy(string_type, "Name");
			break;

		case SYMBOL_NAME_EQU:
			strcpy(string_type, "EQU");
			break;

		case SYMBOL_NAME_SET:
			strcpy(string_type, "SET");
			break;

		default:
			strcpy(string_type, "?");
			break;
	}

	/*	Print symbols of type "symbol_type".
	 *	------------------------------------ */	
	while (local->next != NULL)
	{
		if (local->Symbol_Type == symbol_type)
		{
			symbols_num++;			/*	One more symbols. */

			str_len	= strlen(local->Symbol_Name);

			/*	If there is something to print...
			 *	--------------------------------- */	
			if (str_len > 0)
			{
				/*	Calculate number of tabulation character to print.
				 *	*/
				tab_cnt	= (symbol_field_size - (int) str_len) / tab_length;

				/*	- Add "1" if space to fill is not multiple of
				 *	  tabulation size.  This can be seen directly
				 *	  from "str_len".
				 *	--------------------------------------------- */	 
				if ((str_len % tab_length) != 0)
					tab_cnt++;

				fprintf(list, "%s", local->Symbol_Name);			/*	Name. */

				/*	Space between fields (filled with "tab").
				 *	----------------------------------------- */	
				for (i = 0; i < tab_cnt; i++)
					fprintf(list, "\t");

				fprintf(list, "%s", string_type);
				fprintf(list, "\t");
				fprintf(list, "%05Xh\n", local->Symbol_Value);	/*	Value. */
			}
		}

		local = (SYMBOL *) local->next;		/*	Select next symbols. */
	}
	
	return (symbols_num);
}


/*	*************************************************************************
 *	Function name:	print_symbols_table
 *	Description:	Print Symbols Table.
 *	Author(s):		Claude Sylvain
 *	Created:			31 December 2010
 *	Last modified:	28 December 2011
 *	Parameters:		void
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void print_symbols_table(void)
{
#define pst_TAB_LENGTH				8

	/*	Print symbols table only on second assembly pass.
	 *	------------------------------------------------- */	
	if (asm_pass == 1)
	{
		int		i;
		size_t	str_len;
		int		name_num		= 0;
		int		equ_num		= 0;
		int		set_num		= 0;
		int		label_num	= 0;
		SYMBOL	*local		= Symbols;

		/*	- Notes: Must have a value > 0, in case there is no
		 *	  symbols at all.
	 	 *	*/	 
		int		symbol_name_len_max	= 6;

		int		symbol_field_size;


		/*	Get the length of the longuest symbol name.
		 *	------------------------------------------- */	
		while (local->next != NULL)
		{
			str_len	= strlen(local->Symbol_Name);

			if (str_len > symbol_name_len_max)
				symbol_name_len_max	= str_len;

			local = (SYMBOL *) local->next;		/*	Select next symbols. */
		}

		/*	Calculate the number of "TAB" for the symbols field.
		 *	*/
		symbol_field_size	=	((symbol_name_len_max / pst_TAB_LENGTH) + 1) *
		  							pst_TAB_LENGTH;

		/*	Add additionnal space if minimal space is less than 4 spaces.
		 *	------------------------------------------------------------- */
		if ((symbol_field_size % symbol_name_len_max) <= 4)
			symbol_field_size	+= pst_TAB_LENGTH;


		local	= Symbols;		/*	Sync. */

		/*	Print header.
		 *	************* */

		fprintf(list, "\n\n");
		fprintf(list, "*******************************************************************************\n");
		fprintf(list, "                                 Symbols table\n");
		fprintf(list, "*******************************************************************************\n");
		fprintf(list, "\n");

		fprintf(list, "Names");

		for (i = 0; i < (symbol_field_size / pst_TAB_LENGTH); i++)
			fprintf(list, "\t");

		fprintf(list, "Types\tValues\n");

		fprintf(list, "-----");

		for (i = 0; i < (symbol_field_size / pst_TAB_LENGTH); i++)
			fprintf(list, "\t");

		fprintf(list, "-----\t------\n");

		/*	Print each type of symbols by group.
		 *	------------------------------------ */
		name_num		=
		  	print_symbols_type(SYMBOL_NAME, symbol_field_size, pst_TAB_LENGTH);
		equ_num		=
		  	print_symbols_type(SYMBOL_NAME_EQU, symbol_field_size, pst_TAB_LENGTH);
		set_num		=
		  	print_symbols_type(SYMBOL_NAME_SET, symbol_field_size, pst_TAB_LENGTH);
		label_num	=
		  	print_symbols_type(SYMBOL_LABEL, symbol_field_size, pst_TAB_LENGTH);

		fprintf(list, "\n");

		/*	Print statistics.
		 *	----------------- */
		fprintf(list, "Statistics\n");
		fprintf(list, "----------\n");
		fprintf(list, "\"Name\"\t= %d\n", name_num);
		fprintf(list, "\"EQU\"\t= %d\n", equ_num);
		fprintf(list, "\"SET\"\t= %d\n", set_num);
		fprintf(list, "Labels\t= %d\n", label_num);

		fprintf(list, "\n\n");
	}
}


/*	*************************************************************************
 *	Function name:	OpenFiles
 *	Description:	Open Files.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	17 December 2011
 *	Parameters:		void
 *
 *	Returns:			int:
 *							-1	: Operation failed.
 *							0	: Operation successfull.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int OpenFiles(void)
{
	/* Remove old output files, if any.
	 *	-------------------------------- */
	if (list_file != NULL)	remove(list_file);
	if (bin_file != NULL)	remove(bin_file);
	if (hex_file != NULL)	remove(hex_file);


	/*	Open files.
	 *	*********** */

	/*	- Open input file.
	 *	  If not able to open input file, display an error and abort
	 *	  operation.
	 *	------------------------------------------------------------ */	  
	if ((in_fp[0] = fopen(in_fn[0], "r")) == NULL)
  	{
		fprintf(	stderr,
			  		"*** Error %d: Can't open input file (\"%s\")!\n",
				  	EC_COINF, in_fn[0]);

		return (-1);
	}

	/*	If have to output a listing, create it and check for error.
	 *	----------------------------------------------------------- */	
	if ((list_file != NULL) && (list = fopen(list_file, "w")) == NULL)
  	{
		fprintf(	stderr,
			  		"*** Error %d: Can't open listing file (\"%s\")!\n",
				  	EC_COLF, list_file);

		return (-1);
	}

	/*	Open binary file, and check for error.
	 *	-------------------------------------- */
	if ((bin = fopen(bin_file, "wb")) == NULL)
  	{
		fprintf(	stderr,
			  		"*** Error %d: Can't open binary file (\"%s\")!\n",
				  	EC_COBF, bin_file);

		return (-1);
	}

	/*	Open Intel hexadecimal file, and check for error.
	 *	------------------------------------------------- */
	if ((hex = fopen(hex_file, "w")) == NULL)
  	{
		fprintf(	stderr,
			  		"*** Error %d: Can't open Intel hexadecimal file (\"%s\")!\n",
				  	EC_COHF, hex_file);

		return (-1);
	}

	return (0);
}


/*	*************************************************************************
 *	Function name:	RewindFiles
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2010
 *	Parameters:
 *	Returns:	
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void RewindFiles(void)
{
	fseek(in_fp[0], 0, SEEK_SET);
}


/*	*************************************************************************
 *	Function name:	CloseFiles
 *	Description:	Close Files.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	17 December 2011
 *	Parameters:		void
 *	Returns:			void
 *
 *	Globals:			FILE *list
 *						FILE *bin
 *
 *	Notes:
 *	************************************************************************* */

static void CloseFiles(void)
{
	/*	Close files, if necessary.
	 *	-------------------------- */
	if (in_fp[0] != NULL)	fclose(in_fp[0]);		/*	Source file. */
	if (list != NULL)			fclose(list);			/*	Listing file. */
	if (bin != NULL)			fclose(bin);			/*	Binary file. */
	if (hex != NULL)			fclose(hex);			/*	Intel Hexadecimal file. */
}


/*	*************************************************************************
 *	Function name:	src_line_parser
 *	Description:	Break down a source line.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	29 December 2011
 *
 *	Parameters:		char *text:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *
 *	Notes:			- We assume that "text" always contain something, and
 *						  do not check for empty line.
 *	************************************************************************* */

static int src_line_parser(char *text)
{
	char	keyword[16];
	char	keyword_uc[sizeof (keyword)];	/*	OpCode in Upper Case. */
	int	i					= 0;
	int	msg_displayed	= 0;
	char	equation[80];
	char	label[LABEL_SIZE_MAX];
	int	status	= LIST_ONLY;

	/*	If this is a comment, don't do anything.
	 *	---------------------------------------- */	
	if (text[0] == ';')
	{
		/*	TODO: Why "type" and "status" are not the same ???
		 *	*/

		/* Process comment statement in source stream.
		 * */
		type		= COMMENT;
	}
	/*	- Some 8080 assemblers seems to support special commands beginning
	 *	  with '$' character.
	 *	  For the moment, just ignore lines that contain such special
	 *	  command.
	 *	------------------------------------------------------------------ */
	else if (text[0] == '$')
	{
		if (inside_macro == 0)
			msg_error_s("Special command not supported!", WC_SCNS, text);

		/* Process comment statement in source stream.
		 * */
		type		= COMMENT;
	}
	/*	- Process label/name field, opcode field, operand field, and
	 *	  comment field.
	 * ------------------------------------------------------------ */
  	else
  	{
		keyword_t   *p_keyword;

		memset(label, 0, sizeof (label));
		memset(keyword, 0, sizeof (keyword));
		memset(keyword_uc, 0, sizeof (keyword_uc));
		memset(equation, 0, sizeof (equation));


		/*	Grab the label/name, if any.
		 *	**************************** */

		/*	If there is a label/name, process it.
		 * ------------------------------------- */
		if (isspace((int) *text) == 0)
		{
			/*	Check for non valid first label/name character.
			 *	----------------------------------------------- */
			if (	(isdigit((int) *text) != 0) ||
			  		((isalpha((int) *text) == 0) &&
					((*text != '?') && (*text != '@') && (*text != '&') &&
					(*text != '%')))
				)
			{
				msg_warning_c(	"Bad first character on label/name!", WC_LNBFC,
					  				*text);

				/*	- Bypass all character(s) that can not be used as first
				 *	  label/name character.
				 *	*/
				while (	(isdigit((int) *text) != 0) ||
					  		((isalpha((int) *text) == 0) &&
							((*text != '?') && (*text != '@') && (*text != '&') &&
							(*text != '%')))
						)
				{
					text++;
				}
			}

			/*	TODO: Is this standard Intel assembler code?
			 *	-------------------------------------------- */	
			if (*text == '&')	label[i++] = *(text++);
			if (*text == '%')	label[i++] = *(text++);

			/*	- First label/name character can be '?' or '@'
			 *	  special character.
			 *	---------------------------------------------- */
			if ((*text == '?') || (*text == '@'))
				label[i++] = *(text++);

			/*	Grab remaining of label/name characters.
			 *	---------------------------------------- */	
			while (1)
			{
				if (islabelchar((int) *text) != 0)
				{
					if (i < (sizeof (label) - 3))
					{
						label[i]	= *(text++);
						i++;
					}
					else
					{
						text++;

						/*	- Display/Print error message, if not already done,
						 *	  and necessary.
						 *	--------------------------------------------------- */
						if (!msg_displayed && (asm_pass == 1))
						{
							msg_displayed	= 1;	/*	No more message. */

							msg_warning_s("Label too long!", WC_LTL, label);
						}
					}
				}
				else
				{
					/*	If this is the end of label.
					 *	---------------------------- */
					if (	(isspace((int) *text) != 0) || (*text == '\0') ||
						  	(*text == ':'))
					{
						/*	- TODO: Add the possibility to make "asm8080" check
						 *	  for the presence of ':' at end of labels.
						 *	- Notes: "Name" do not need to end with ':'.
						 *	*/

						/*	Destroy ':', if necessary.
						 *	-------------------------- */
						if (*text == ':')
							text++;

						break;
					}
					/*	This is not a valid label/name character.
					 *	Bypass the invalid character.
					 *	----------------------------------------- */
					else
					{
						msg_warning_c(	"Invalid label/name character!", WC_ILNC,
							  				*text);

						text++;
					}
				}
			}
		}

		/*	Bypass space character(s).
		 *	-------------------------- */
		while (isspace((int) *text) != 0)
			text++;

		/*	If nothing else than the label/name on the line...
		 *	-------------------------------------------------- */	
		if ((*text == '\0') || (*text == ';'))
		{
			/*	- If code section is activated and there is no macro
			 *	  definition processed, process label.
			 *	- Notes: There is no assembler directive, neither mnemonic
			 *	  on the line.  So, label processing will not be
		 	 *	  done by external function.  We must do label
			 *	  processing here.
			 *	---------------------------------------------------------- */
			if ((util_is_cs_enable() != 0) && (inside_macro == 0))
				process_label(label);

			/*	TODO: Why "type" and "status" are not the same ???
			 *	*/

			/* Process comment statement in source stream.
			 * */
			type		= COMMENT;

			return (LIST_ONLY);
		}


		/*	Grab the keyword (assembler directive or opcode).
		 *	************************************************* */

		i					= 0;
		msg_displayed	= 0;

		while ((isalnum((int) *text)) || (*text == '_'))
		{
			if (i < (sizeof (keyword) - 1))
			{
				keyword[i]		= *text;
				keyword_uc[i]	= toupper((int) *text);
				text++;
				i++;
			}
			else
			{
				text++;

				/*	- Display/Print error message, if not already done,
				 *	  and necessary.
				 *	--------------------------------------------------- */
				if (!msg_displayed && (asm_pass == 1))
				{
					msg_displayed	= 1;	/*	No more message. */

					msg_error_s("Keyword too long!", EC_KTL, keyword);
				}
			}
		}

		/*	Bypass delimitors, if necessary.
		 *	-------------------------------- */
		while (isspace((int) *text))
			text++;

		/*	Copy third field to equation buffer.
		 *	************************************	*/

		if ((*text != '\0') && (*text != ';'))
		{
			i					= 0;
			msg_displayed	= 0;

			while ((iscntrl((int) *text) == 0) && (*text != ';'))
			{
				if (i < (sizeof (equation) - 1))
				{
					equation[i]	= *(text++);
					i++;
				}
				else
				{
					text++;

					/*	- Display/Print error message, if not already done,
					 *	  and necessary.
					 *	--------------------------------------------------- */
					if (!msg_displayed && (asm_pass == 1))
					{
						msg_displayed	= 1;	/*	No more message. */

						msg_error_s("equation too long!", EC_ETL, equation);
					}
				}
			}
		}

		/*	- Lookup for assembler directives, and call associated
		 *	  function if necessary.
		 *	****************************************************** */

		p_keyword	= (keyword_t *) asm_dir;

		while (p_keyword->Name)
		{
			/*	If keyword is found, call the associated function.
			 *	-------------------------------------------------- */	
			if (strcmp(p_keyword->Name, keyword_uc) == 0)
		  	{
				if (inside_macro == 0)
				{
					status	= p_keyword->fnc(label, equation);
					type		= status;

					if (status == PROCESSED_END)
						return (status);
				}
				else if (strcmp(keyword_uc, "ENDM") == 0)
				{
					status	= p_keyword->fnc(label, equation);
					type		= status;
				}
				else
					type		= LIST_ONLY;

				break;
			}
		  	else
			{
				p_keyword++;
			}
		}

		/*	If assembler directive was found and processed, exit.
		 *	----------------------------------------------------- */	
		if (p_keyword->Name != NULL)
			return (status);

		/*	- If code section is not active or inside macro definition
		 *	  processing, do no search for opcodes and macros.
		 *	  Just list.
		 *	---------------------------------------------------------- */
		if ((util_is_cs_enable() == 0) || (inside_macro == 1))
		{
			type		= LIST_ONLY;
			status	= LIST_ONLY;
			return (status);
		}


		/*	- Lookup for opcodes, and call associated
		 *	  function if necessary.
		 *	***************************************** */

		p_keyword	= (keyword_t *) OpCodes;

		while (p_keyword->Name)
		{
			/*	If keyword is found, call the associated function.
			 *	-------------------------------------------------- */	
			if (!strcmp(p_keyword->Name, keyword_uc))
			{
				status	= p_keyword->fnc(label, equation);
				type		= status;
				break;
			}
			else
			{
				p_keyword++;
			}
		}

		/*	If opcode was found and processed, exit.
		 *	---------------------------------------- */	
		if (p_keyword->Name != NULL)
			return (status);


		/*	Try with a macro.
		 *	***************** */

		/*	Remember actual File Level.
		 *	This will serve us later to see if macro was found or not.
		 *	*/
		i	= file_level;

		/*	Increment File Level, and check for overflow error.
		 *	--------------------------------------------------- */
		if (++file_level <= FILES_LEVEL_MAX)
		{
			int	file_openned	= 1;
			char	*fn_macro;

			/*	Open include file.
			 *	****************** */	

			fn_macro	= (char *) malloc(strlen(keyword_uc) + 3);

			if (fn_macro != NULL)
			{
				strcpy(fn_macro, keyword_uc);
				strcat(fn_macro, ".m");

				if ((in_fp[file_level] = fopen(fn_macro, "r")) == NULL)
				{
					--file_level;				/*	Restore. */
					file_openned	= 0;
				}

				/*	Open include file, and check for error.
				 *	--------------------------------------- */
				if (file_openned)
				{
					/*	Allocate memory for the input file name.
					 *	*/	
					in_fn[file_level]	= (char *) malloc(strlen(fn_macro) + 1);

					/*	Check for memory allocation error.
					 *	--------------------------------- */	
					if (in_fn[file_level] != NULL)
					{
						strcpy(in_fn[file_level], fn_macro);	/*	Save input file name. */

#if 0
						/*	- Check if macro have paramaters.
						 *	- If macro have parameters, warn user that macro
						 *	  parameters are not supported.
						 *	------------------------------------------------ */
						if (strlen(equation) > 0)
						{
							msg_warning_s(	"Macro parameters are not supported!",
								  				WC_MPNS, equation);
						}
#endif
					}
					else
					{
						fclose(in_fp[file_level]);		/*	Close file. */
						--file_level;						/*	Abort "include". */
						msg_error("Memory allocation error!", EC_MAE);
					}
				}

				free(fn_macro);
			}
			else
			{
				--file_level;			/*	Abort "include". */

				msg_error("Memory allocation error!", EC_MAE);
			}
		}
		else
		{
			--file_level;				/*	Abort "include". */

			msg_error("Include overflow!", EC_IOF);
		}

		/*	- If code section is activated, process label.
		 *	- Notes: Label was not processed by either assembler
		 *	  directive processing function or assembler opcode
		 *	  processing function.  So, we have to explicitly call
		 *	  label processing.
		 *	------------------------------------------------------ */
		if (util_is_cs_enable() != 0)
			process_label(label);

		/*	If trying as a macro fails...
		 *	----------------------------- */
		if (file_level == i)
		{
			type		= COMMENT;
			status	= COMMENT;

			msg_error_s("Can't find keyword", EC_CNFK, keyword);
		}
	}

	return (status);
}


/*	*************************************************************************
 *	Function name:	PrintList
 *
 *	Description:	- Output list format.
 *						  [addr] [bn] [bn] [bn] [b4] lab^topcode^taddr^tcomment
 *
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	6 January 2012
 *
 *	Parameters:		char *text:
 *							...
 *
 *	Returns:			void
 *
 *	Globals:			int target.pc
 *						int data_size
 *
 *	Notes:
 *	************************************************************************* */

static void PrintList(char *text)
{
	STACK	*LStack	= ByteWordStack;
	int	space		= 0;


	/*	Don't print list if not in assembler pass 1.
	 *	*/
	if (asm_pass != 1)	return;

	switch (type)
	{
		case COMMENT:
			if (list != NULL)
				fprintf(list, "\t\t\t%s\n", text);

			break;

		case TEXT:
			switch (data_size)
			{
				case 1:
					check_new_pc(data_size);		/*	Check the new PC value. */

					if (list != NULL)
					{
						fprintf(	list, "%6d %04X %02X\t\t%s\n", codeline[file_level],
							  		target.pc, b1, text);
					}

					break;

				case 2:
					check_new_pc(data_size);		/*	Check the new PC value. */

					if (list != NULL)
					{
						fprintf(	list, "%6d %04X %02X %02X\t%s\n",
									codeline[file_level], target.pc, b1, b2, text);
					}

					break;

				case 3:
					check_new_pc(data_size);		/*	Check the new PC value. */

					if (list != NULL)
					{
						fprintf(	list, "%6d %04X %02X %02X %02X\t%s\n",
									codeline[file_level], target.pc, b1, b2, b3, text);
					}

					break;

				case 4:
					check_new_pc(data_size);		/*	Check the new PC value. */

					if (list != NULL)
					{
						fprintf(	list, "%6d %04X %02X %02X %02X %02X\t%s\n",
									codeline[file_level], target.pc, b1, b2, b3, b4,
								  	text);
					}

					break;

				/* 0
				 * - */
				default:
					if (list != NULL)
						fprintf(list, "            %02X %02X\t%s\n", b2, b1, text);

					break;
			}
			break;

		case LIST_ONLY:
		case PROCESSED_END:
			if (list != NULL)
				fprintf(list, "\t\t\t%s\n", text);

			break;

		/*	List "DS" (Data Storage).
		 *	------------------------- */	
		case LIST_DS:
			/*	Negative value is not accepted, on "DS" assembler directive.
			 *	Print an error in case such situation happen.
			 *	Update "PC" only if "data_size" is positive.
			 *	------------------------------------------------------------ */	
			if (data_size < 0)
			{
				msg_error("Negative value on \"DS\"!", EC_NVDS);
			}
			else
				check_new_pc(data_size);		/*	Check the new PC value. */

			if (list != NULL)
			{
				fprintf(	list, "%6d %04X\t\t%s\n", codeline[file_level],
					  		target.pc, text);
			}

			break;

		case LIST_BYTES:
		case LIST_WORDS:
		case LIST_STRINGS:
			data_size	= 0;

			/* - Set "data_size" by counting space that will be
			 *   taken by the list elements.
			 *	------------------------------------------------ */
			while (LStack->next)
			{
				if ((type == LIST_BYTES) || (type == LIST_STRINGS))
					data_size++;
				/*	Assuming this is a "WORD".
				 *	-------------------------- */
				else
					data_size	+= 2;

				LStack = (STACK *) LStack->next;
			}

			check_new_pc(data_size);		/*	Check the new PC value. */

			/*	Restore stack pointer.
			 *	*/
			LStack	= ByteWordStack;

			if (list != NULL)
			{
				fprintf(	list, "%6d %04X\t\t%s\n", codeline[file_level],
					  		target.pc, text);

				fprintf(list, "            ");
			}

			/*	Process all elements in the linked list.
			 *	---------------------------------------- */
			while (LStack->next)
			{
				if ((type == LIST_BYTES) || (type == LIST_STRINGS))
				{
					if (list != NULL)
						fprintf(list, "%02X ", (LStack->word & 0xFF));

					space += 3;
				}
				/*	Assuming this is a "WORD".
				 *	-------------------------- */
				else
				{
					/*	- If listing file exist, list word using Little Endian
					 *	  format.
					 *	------------------------------------------------------ */	
					if (list != NULL)
					{
						fprintf(list, "%02X", LStack->word & 0xFF);
						fprintf(list, "%02X ", LStack->word >> 8);
					}

					space				+= 5;
				}

				LStack = (STACK *) LStack->next;

				/*	Change line, if necessary.
				 *	-------------------------- */
				if (space >= (4 * 3))
				{
					if (list != NULL)
						fprintf(list, "\n            ");

					space = 0;
				}
			}

			if (list != NULL)
				fprintf(list, "\n");

			break;

		default:
			break;
	}

	/*	Remove after debug.
	 *	------------------- */
	if (list != NULL)
		fflush(list);
}


/*	*************************************************************************
 *	Function name:	ProcessDumpBin
 *	Description:	Process Dump of Binary code.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	17 December 2011
 *	Parameters:		void
 *	Returns:			void
 *	Globals:
 *
 *	Notes:
 *						- This function is called on end of assembler pass #2,
 *						  and dump all significant target bytes to the
 *						  binary file.
 *	************************************************************************* */

void ProcessDumpBin(void)
{
	/*	If there is something to write...
	 *	--------------------------------- */
	if ((target.pc_highest > 0) && (asm_pass == 1))
	{
		/*	Write binary.
		 *	*/
		fwrite(	&Image[target.pc_lowest],
			  		target.pc_highest - target.pc_lowest, 1, bin);
	}
}


/*	*************************************************************************
 *	Function name:	ProcessDumpHex
 *	Description:	Process Dump of Intel Hexadecimal code.a
 *	Author(s):		Claude Sylvain
 *	Created:			17 December 2011
 *	Last modified:	24 December 2011
 *
 *	Parameters:		char end_of_asm:
 *							0	: This is not the End Of Assembly process.
 *							1	: This is the End Of Assembly process.
 *
 *	Returns:			void
 *	Globals:
 *
 *	Notes:
 * 					- This function is called every time an "ORG"
 * 					  directive is found, and at end of the
 * 					  assembly pass #2.
 *	************************************************************************* */

void ProcessDumpHex(char end_of_asm)
{
	int	addr_end;

	/*	Process only on assembler pass #2.
	 *	*/
	if (asm_pass != 1)	return;

	/*	Set Highest Address accordingly to "target.addr".
	 *	------------------------------------------------- */	
	if (target.addr <= 0x10000)
		addr_end	= target.addr;
	else
		addr_end	= target.pc;

	/*	- Process Intel hexadecimal object file only if there is something
	 *	  to process.
	 *	------------------------------------------------------------------ */
	if (addr_end > target.pc_org)
	{
		int		addr			= target.pc_org;
		int		i;
		int		byte_count;
		uint8_t	checksum;
		char		string[5];

		/*	Process target bytes.
		 *	--------------------- */
		while (addr < addr_end)
		{
			/*	Set bytes count.
			 *	---------------- */
			if ((addr_end - addr) >= 16)
				byte_count	= 16;
			else
				byte_count	= addr_end - addr;

			checksum	= 0x00;							/*	Reset checksum. */

			fprintf(hex, ":");						/*	Start code. */

			/*	Process byte count.
			 *	------------------- */
			checksum	+= (uint8_t) byte_count;
			byte_to_hex((uint8_t) byte_count, string);
			fprintf(hex, "%s", string);

			/*	Process address.
			 *	---------------- */
			checksum	+= (uint8_t) (addr >> 8);		/*	Address MSB part. */
			checksum	+= (uint8_t) (addr & 0xFF);	/*	Address LSB part. */
			word_to_hex((uint16_t) addr, string);
			fprintf(hex, "%s", string);

			fprintf(hex, "00");							/*	Record type. */

			/*	Process "Data" field.
			 *	--------------------- */
			for (i = 0; i < byte_count; ++i)
			{
				checksum	+= Image[(addr) + i];

				byte_to_hex(Image[addr + i], string);
				fprintf(hex, "%s", string);
			}

			/*	Process checksum field.
			 *	----------------------- */
			checksum	= ~checksum + 1;			/*	Two's complement. */
			byte_to_hex(checksum, string);
			fprintf(hex, "%s\n", string);

			addr	+= byte_count;					/*	Update byte location. */
		}
	}

	/*	- If this is the end of assembly process, put the End Of File
	 *	  record in the hex file.
	 *	------------------------------------------------------------- */	  
	if (end_of_asm != 0)
		fprintf(hex, ":00000001FF\n");
}


/*	*************************************************************************
 *	Function name:	DumpBin
 *	Description:	Dump Binary.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	6 January 2012
 *	Parameters:		void
 *	Returns:			void
 *
 *	Globals:			int type
 *						int data_size
 *						int target.pc
 *
 *	Notes:
 *	************************************************************************* */

static void DumpBin(void)
{
	STACK	*DLStack;
	STACK	*LStack	= ByteWordStack;


	switch (type)
	{
		case TEXT:
			switch (data_size)
			{
				case 1:
					Image[target.pc] = b1;
					update_pc(1);
					break;

				case 2:
					Image[target.pc] = b1;
					update_pc(1);
					Image[target.pc] = b2;
					update_pc(1);
					break;

				case 3:
					Image[target.pc] = b1;
					update_pc(1);
					Image[target.pc] = b2;
					update_pc(1);
					Image[target.pc] = b3;
					update_pc(1);
					break;

				case 4:
					Image[target.pc] = b1;
					update_pc(1);
					Image[target.pc] = b2;
					update_pc(1);
					Image[target.pc] = b3;
					update_pc(1);
					Image[target.pc] = b4;
					update_pc(1);
					break;

				default:
					break;	
			}

			break;

		case LIST_DS:
		{	
			int	i;

			/*	Fill space.
			 *	Notes: 0x00 is also known as "NOP" opcode.
			 *	------------------------------------------ */
			for (i = 0; i < data_size; i++)
			{
				Image[target.pc] = 0x00;
				update_pc(1);
			}

			break;
		}

		case LIST_BYTES:
		case LIST_WORDS:
		case LIST_STRINGS:
			/* Don't count last byte/word.
			 *	--------------------------- */
			while (LStack->next)
			{
				if ((type == LIST_BYTES) || (type == LIST_STRINGS))
				{
					Image[target.pc]	= LStack->word & 0xFF;
					update_pc(1);
				}
				/*	Assuming "LIST_WORDS".
				 *	Store it in memory using Little Endian format.	
				 *	---------------------------------------------- */
				else
				{
					Image[target.pc]	= (char) (LStack->word & 0xFF);
					update_pc(1);
					Image[target.pc]	= (char) (LStack->word >> 8);
					update_pc(1);
				}

				LStack = (STACK *) LStack->next;
			}

			/*	Always keep the stack root.
			 *	--------------------------- */
			LStack	= ByteWordStack;
			DLStack	= LStack = (STACK *) LStack->next;

			if (LStack)
			{
				do
				{
					LStack	= (STACK *) LStack->next;
					free(DLStack);
					DLStack	= LStack;
				} while (LStack);

				LStack			= ByteWordStack;
				LStack->next	= NULL;
			}
			break;

		default:
			break;	
	}
}


/*	*************************************************************************
 *	Function name:	do_asm
 *	Description:	Assemble source file.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2011
 *	Parameters:		void
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void do_asm(void)
{
	char		*p_text;
	char		*p_text_1;
	int		EmitBin;
	int		eol_found;			/*	End Of Line Found. */
	size_t	str_len;


	/*	- Allocated memory for source line buffer, and check for
	 *	  memory allocation error.
	 *	-------------------------------------------------------- */	  
	if ((p_text = (char *) malloc(SRC_LINE_WIDTH_MAX)) == NULL)
	{
		fprintf(stderr, "*** Error %d: Can't allocate memory!\n", EC_CAM);

		return;
	}

	/*	Assemble source file.
	 *	--------------------- */
	while (1)
	{
		EmitBin	= LIST_ONLY;
		type 		= LIST_ONLY;
		codeline[file_level]++;

		/*	- Get a source file line.
		 *	- If not able to get a line...
		 *	- Notes: "fgets()" add a '\0' after the last character,
		 *	  in the buffer.
	 	 *	------------------------------------------------------- */	 
		if (fgets(p_text, SRC_LINE_WIDTH_MAX, in_fp[file_level]) == NULL)
		{
			/*	If source file is an included file, ...
			 *	--------------------------------------- */		  
			if (file_level > 0)
			{
				/*	Close input file handle.
				 *	------------------------ */	
				fclose(in_fp[file_level]);
				in_fp[file_level]	= NULL;

				/*	Free memory allocated for the input file name.
				 *	---------------------------------------------- */	
				free(in_fn[file_level]);
				in_fn[file_level]	= NULL;

				file_level--;
				continue;			/*	Restart assembly process at lower level. */
			}
			/*	- We are at level 0 (main source file), and there is no
			 *	  other file to process, process binary output, if necessary
			 *	  and exit.
			 *	- Notes: Main source file will be close later; elsewhere.
		 	 *	----------------------------------------------------------- */	 
			else
			{
				ProcessDumpBin();
				ProcessDumpHex(1);

				/*	Print symbols table, if necessary.
				 *	---------------------------------- */	
				if (list != NULL)
					print_symbols_table();

				break;
			}
		}

		/*	If there is an active macro, save line to the macro file.
		 *	--------------------------------------------------------- */
		if (fp_macro != NULL)
			fputs(p_text, fp_macro);

		/*	Check if we was able to grab all the source line.
		 *	************************************************* */

		p_text_1		= p_text;
		eol_found	= 0;

		/*	Search for End Of Line.
		 *	Notes: Some times a line can terminate with EOF!	
		 *	------------------------------------------------ */
		while (*p_text_1 != '\0')
		{
			if (*(p_text_1++) == '\n')
				eol_found	= 1;
		}

		/*	- If No end of line was found and not all file was read,
		 *	  this tell us that line is too long :-(
		 *	- Notes: Some times a line can terminate with EOF!	
		 *	-------------------------------------------------------- */
		if (!eol_found && (feof(in_fp[file_level]) == 0))
		{
			int	c;

			/*	Bypass remaining of source line.
			 *	Notes: This is necessary to get rid of "codeline[]" phasing error.
			 *	------------------------------------------------------------------ */
			do
				c	= fgetc(in_fp[file_level]);
			while ((c != '\n') && (c != EOF));

			msg_error("Line too long!", EC_SLTL);
		}

		/*	Destroy New Line character, if necessary.
		 *	----------------------------------------- */
		if (eol_found)			
			p_text[strlen(p_text) - 1] = '\0';

		str_len	= strlen(p_text);		/*	Get the length of the source line. */

		/*	- If source line contain something, process it.
		 *	- Notes: Do not parse line that only contain
		 *	  Ctrl+Z ASCII control code, because this is
		 *	  the end of file mark for CP/M text file; and
		 *	  that such files comming from the old age can
		 *	  be processed by "asm8080".
		 *	----------------------------------------------- */
		if ((str_len >= 1) && ((str_len > 2) || ((*p_text != 0x1A))))
		{
			EmitBin = src_line_parser(p_text);

			if (util_is_cs_enable() == 0)
				type = LIST_ONLY;
		}

		/*	- When "END" directive was processed, don't print list
		 *	  immediatly.  This will be done later...
	 	 *	------------------------------------------------------ */	 
		if (EmitBin != PROCESSED_END)
			PrintList(p_text);

		if (util_is_cs_enable() == 1)
			DumpBin();

		/*	If an "END" directive was executed...
		 *	------------------------------------- */	
		if (EmitBin == PROCESSED_END)
		{
			/*	If the "END" directive was found inside an include file...
			 *	---------------------------------------------------------- */
			if ((file_level > 0) && (asm_pass == 1))
			{
				msg_warning(	"\"END\" directive found inside an include file!",
					  				WC_EDFIIF);
			}

			PrintList(p_text);
			ProcessDumpBin();
			ProcessDumpHex(1);

			/*	Print symbols table, if necessary.
			 *	---------------------------------- */	
			if (list != NULL)
				print_symbols_table();

			/*	If the "END" directive was found inside an include file...
			 *	---------------------------------------------------------- */
			if (file_level > 0)
			{
				int	i;

				/*	- Close all openned included files, and free associated
				 *	  resources.
				 *	------------------------------------------------------- */	
				for (i = file_level; i > 0; i--)
				{
					/*	Close input file handle.
					 *	------------------------ */
					fclose(in_fp[i]);
					in_fp[i]	= NULL;

					/*	Free memory allocated for the input file name.
					 *	---------------------------------------------- */
					free(in_fn[i]);
					in_fn[i]	= NULL;
				}

				file_level	= 0;		/*	We are now at level 0. */
			}

			break;						/*	Terminate assembly process. */
		}

		data_size	=
	  	b1				=
	  	b2				=
	  	b3				=
	  	b4				= 0;
	}

	free(p_text);		/*	Free allocated memory. */
}


/*	*************************************************************************
 *	Function name:		display_help
 *	Description:		Display Help.
 *	Author(s):			Jay Cotton, Claude Sylvain
 *	Created:				2007
 *	Last modified:		17 December 2011
 *	Parameters:			void
 *	Returns:				void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void display_help(void)
{
	printf("Usage: %s <source file> [<options>]\n", name_pgm);
	printf("Options:\n");
	printf("  -h           : Display Help.\n");
	printf("  -I<dir>      : Add directory to the include file search path.\n");
	printf("  -l<filename> : Generate listing file.\n");
	printf("  -o<filename> : Define output files (optionnal).\n");
	printf("  -v           : Display version.\n");
}


/*	*************************************************************************
 *	Function name:		display_version
 *	Description:		Display Version.
 *	Author(s):			Claude Sylvain
 *	Created:				3 January 2011
 *	Last modified:
 *	Parameters:			void
 *	Returns:				void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void display_version(void)
{
	printf(	"%s version %d.%d.%d\n", name_pgm, pgm_version_v,
		  		pgm_version_sv, pgm_version_rn);
}


/*	*************************************************************************
 *	Function name:	asm_pass1
 *	Description:	Assembler Pass #1.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	24 December 2011
 *	Parameters:		void
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void asm_pass1(void)
{
	int	i;

	for (i = 0; i < FILES_LEVEL_MAX; i++)
		codeline[i]	= 0;

	target.addr			= 0;
	target.pc			= 0x0000;
	target.pc_org		= 0x0000;
	target.pc_lowest	= 0xFFFF;
	target.pc_highest	= 0;
	type					= LIST_ONLY;
	asm_pass 			= 0;

	memset(Image, 0, sizeof (Image));
	do_asm();
}


/*	*************************************************************************
 *	Function name:	asm_pass2
 *	Description:	Assembler Pass #2.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	24 December 2011
 *	Parameters:		void
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void asm_pass2(void)
{
	int	i;

	for (i = 0; i < FILES_LEVEL_MAX; i++)
		codeline[i]	= 0;

	target.addr			= 0;
	target.pc			= 0x0000;
	target.pc_org		= 0x0000;
	target.pc_lowest	= 0xFFFF;
	target.pc_highest	= 0;
	type					= LIST_ONLY;
	asm_pass				= 1;

	memset(Image, 0, sizeof (Image));
	RewindFiles();
	do_asm();
}


/*	*************************************************************************
 *	Function name:	init
 *	Description:	Initialize module.
 *	Author(s):		Claude Sylvain
 *	Created:			28 December 2010
 *	Last modified:	31 December 2010
 *	Parameters:		void
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void init(void)
{
	int	i;

	/*	Init. "in_fn" array.
	 *	-------------------- */	
	for (i = 0; i < (sizeof (in_fn) / sizeof (char *)); i++)
		in_fn[i] = NULL;
}


/*	*************************************************************************
 *	Function name:	process_option_i
 *	Description:	Process Option "-I".
 *	Author(s):		Claude Sylvain
 *	Created:			31 December 2010
 *	Last modified:	27 March 2011
 *
 *	Parameters:		char *text:
 *							Pointer to text that hold "-I" option.
 *
 *	Returns:			int:
 *							-1	: Operation failed.
 *							0	: Operation successfull.
 *
 *	Globals:
 *
 *	Notes:			- If a path is specified, path delimitor is not mandatory
 *						  at end of path.
 *	************************************************************************* */

static int process_option_i(char *text)
{
	int		rv					= -1;
	int		have_to_add_ps	= 0;		/*	Have To Add Path Seperator (default == NO). */
	size_t	string_len;

	struct option_i_t	*p_option_i	= &option_i;		/*	Base structure. */


	text			+= 2;								/*	Bypass "-I". */
	string_len	= strlen(text);

	/*	- If last character is not a path seperator, remember we
	 *	  have to add it, and set string length 1 byte larger
	 *	  to make space to that added space seperator.
	 *	-------------------------------------------------------- */
	if (text[string_len - 1] != PATH_SEPARATOR_CHAR)
	{
		have_to_add_ps	= 1;
		string_len++;
	}

	/*	Search for the next available "option_i" structure.
	 *	--------------------------------------------------- */
	while (p_option_i->next != NULL)
		p_option_i	= p_option_i->next;

	if ((p_option_i->path = (char *) malloc(string_len + 1)) != NULL)
	{
		strcpy(p_option_i->path, text);		/*	Save path. */

		/*	Add path seperator, if necessary.
		 *	--------------------------------- */
		if (have_to_add_ps != 0)
			strcat(p_option_i->path, PATH_SEPARATOR_STR);

		/*	Allocate memory for the next "option_i" structure.
		 *	*/	
		p_option_i->next	=
		  	(struct option_i_t *) malloc(sizeof (struct option_i_t));

		if (p_option_i->next != NULL)
		{
			/*	Init. newly created structure "nexẗ" member to NULL.
			 *	---------------------------------------------------- */
			p_option_i			= p_option_i->next;
			p_option_i->path	= NULL;
			p_option_i->next	= NULL;

			rv	= 0;
		}
		else
		{
			fprintf(stderr, "*** Error %d: Can't allocate memory!\n", EC_MAE);
			free(p_option_i->path);		/*	Make structure available. */
		}
	}
	else
	{
		fprintf(stderr, "*** Error %d: Can't allocate memory!\n", EC_MAE);
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	process_option_l
 *	Description:	Process Option "-l".
 *	Author(s):		Claude Sylvain
 *	Created:			26 March 2011
 *	Last modified:	17 December 2011
 *
 *	Parameters:		char *text:
 *							Pointer to text that hold "-l" option.
 *
 *	Returns:			int:
 *							-1	: Operation failed.
 *							0	: Operation successfull.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int process_option_l(char *text)
{
	const char	*msg_cam	=	"*** Error %d: Can't allocate memory!\n";

	int		rv				= -1;		/*	Return Value (default == -1). */
	size_t	string_len;


	/*	If an "-l" option already processed, alert user, and exit with error.
	 *	--------------------------------------------------------------------- */	
	if (list_file != NULL)
	{
		fprintf(	stderr,
			  		"*** Warning %d: Extra \"-l\" option specified (\"%s\")!\n",
				  	WC_ELOS, text);
		
		fprintf(	stderr,
			  		"    This option will be ignored!\n");

		return (-1);
	}

	text			+= 2;					/*	Bypass "-l". */
	string_len	= strlen(text);

	/*	- If a listing file name follow "-l" option, take it as the
	 *	  listing file name.
 	 *	----------------------------------------------------------- */
	if (string_len > 0)
	{
		int		i;
		int		dot_pos		= 0;

		/*	Search for the beginning of an extension.
		 *	----------------------------------------- */
		for (i = 0; i < (string_len - 1); i++)
		{
			if (	(text[i] == '.') && (text[i + 1] != '.') &&
					(text[i + 1] != PATH_SEPARATOR_CHAR))
			{
				dot_pos	= i;
			}
		}

		/*	- If a file extension is present, init. listing file name
		 *	  without adding an extension.
		 *	--------------------------------------------------------- */	 
		if (dot_pos > 0)
		{
			/*	Allocate memory for the listing file name.
			 *	*/
			list_file	= (char *) malloc(string_len + 1);

			/*	Built list file name, if possible.
			 *	---------------------------------- */	
			if (list_file != NULL)
			{
				strcpy(list_file, text);
				rv	= 0;		/*	Success! */
			}
			else
			{
				fprintf(stderr, msg_cam, EC_CAM);
			}
		}
		/*	- No file extension is present, init. listing file name,
		 *	  and add an extension to it.
		 *	-------------------------------------------------------- */	 
		else
		{
			/*	Allocate memory for the listing file name.
			 *	*/
			list_file	= (char *) malloc(string_len + 4 + 1);

			/*	Built list file name, if possible.
			 *	---------------------------------- */	
			if (list_file != NULL)
			{
				strcpy(list_file, text);
				strcat(list_file, ".lst");
				rv	= 0;		/*	Success! */
			}
			else
			{
				fprintf(stderr, msg_cam, EC_CAM);
			}
		}
	}
	/*	- No listing file name follow "-l" option, use the input
	 *	  file base name as the listing base file name.
 	 *	-------------------------------------------------------- */
	else
	{
		/*	If input file base name exist, use it as listing base file name.
		 *	---------------------------------------------------------------- */	
		if (strlen(fn_base) > 0)
		{
			/*	Allocate memory for the listing file name.
			 *	*/
			list_file	= (char *) malloc(strlen(fn_base) + 4 + 1);

			/*	Built list file name, if possible.
			 *	---------------------------------- */	
			if (list_file != NULL)
			{
				strcpy(list_file, fn_base);
				strcat(list_file, ".lst");
				rv	= 0;		/*	Success! */
			}
			else
			{
				fprintf(stderr, msg_cam, EC_CAM);
			}
		}
		/*	No input file base name exist, use default listing file name.
		 *	------------------------------------------------------------- */	
		else
		{
			char	*default_lst_file_name	= "a.lst";

			fprintf(	stderr,
				  		"*** Warning %d: No input file specified at time \"-l\" option was processed!\n",
					  	WC_NIFSATLOP);

			fprintf(	stderr,
				  		"    Using default name (\"a.lst\")!\n");

			/*	Allocate memory for the listing file name.
			 *	*/
			list_file	= (char *) malloc(strlen(default_lst_file_name) + 1);

			/*	Built list file name, if possible.
			 *	---------------------------------- */	
			if (list_file != NULL)
			{
				strcpy(list_file, default_lst_file_name);
				rv	= 0;		/*	Success! */
			}
			else
			{
				fprintf(stderr, msg_cam, EC_CAM);
			}
		}
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	process_option_o
 *	Description:	Process Option "-o".
 *	Author(s):		Claude Sylvain
 *	Created:			26 March 2011
 *	Last modified:	17 December 2011
 *
 *	Parameters:		char *text:
 *							Pointer to text that hold "-o" option.
 *
 *	Returns:			int:
 *							-1	: Operation failed.
 *							0	: Operation successfull.
 *
 *	Globals:			char	*bin_file
 *						char	*hex_file
 *
 *	Notes:
 *	************************************************************************* */

static int process_option_o(char *text)
{
	const char	*msg_cam	=	"*** Error %d: Can't allocate memory!\n";

	int		rv				= -1;		/*	Return Value (default == -1). */
	size_t	string_len;


	/*	If an "-o" option already processed, alert user, and exit with error.
	 *	--------------------------------------------------------------------- */	
	if ((bin_file != NULL) || (hex_file != NULL))
	{
		fprintf(	stderr,
			  		"*** Warning %d: Extra \"-o\" option specified (\"%s\")!\n",
				  	WC_EOOS, text);

		fprintf(	stderr,
			  		"    This option will be ignored!\n");

		return (-1);
	}

	text			+= 2;					/*	Bypass "-o". */
	string_len	= strlen(text);

	/*	- If a file name follow "-o" option, take it as the
	 *	  output file name.
 	 *	--------------------------------------------------- */
	if (string_len > 0)
	{
		int		i;
		int		dot_pos		= 0;

		/*	Search for the beginning of an extension.
		 *	----------------------------------------- */
		for (i = 0; i < (string_len - 1); i++)
		{
			if (	(text[i] == '.') && (text[i + 1] != '.') &&
					(text[i + 1] != PATH_SEPARATOR_CHAR))
			{
				dot_pos	= i;
			}
		}

		/*	- If a file name extension is present, hack the file name
		 *	  to remove the file name extension.
		 *	--------------------------------------------------------- */
		if (dot_pos > 0)	
		{
			fprintf(	stderr,
				  		"*** Warning %d: Output file name have an extension (\"%s\")!\n",
					  	WC_OFNHE, text);

			fprintf(	stderr,
				  		"    The extension will be ignored!\n");

			text[dot_pos]	= '\0';
			string_len		= strlen(text);
		}

		/*	Allocate memory for the binary file name.
		 *	*/
		bin_file	= (char *) malloc(string_len + 4 + 1);

		/*	Allocate memory for the Intel hexadecimal file name.
		 *	*/
		hex_file	= (char *) malloc(string_len + 4 + 1);

		/*	Built binary file name, if possible.
		 *	------------------------------------ */	
		if ((bin_file != NULL) && (hex_file != NULL))
		{
			strcpy(bin_file, text);
			strcat(bin_file, ".bin");

			strcpy(hex_file, text);
			strcat(hex_file, ".hex");

			rv	= 0;		/*	Success! */
		}
		else
		{
			fprintf(stderr, msg_cam, EC_CAM);
		}
	}
	/*	- No binary file name follow "-o" option, use the input
	 *	  file base name as the binary base file name and Intel
	 *	  hexadecimal base file name.
 	 *	------------------------------------------------------- */
	else
	{
		/*	- If input file base name exist, use it as binary base file
		 *	  name and Intel hexadecimal base file name.
		 *	----------------------------------------------------------- */	
		if (strlen(fn_base) > 0)
		{
			/*	Allocate memory for the binary file name.
			 *	*/
			bin_file	= (char *) malloc(strlen(fn_base) + 4 + 1);

			/*	Allocate memory for the Intel hexadecimal file name.
			 *	*/
			hex_file	= (char *) malloc(strlen(fn_base) + 4 + 1);

			/*	Built binary and Intel hexadecimal file names, if possible.
			 *	----------------------------------------------------------- */	
			if ((bin_file != NULL) && (hex_file != NULL))
			{
				strcpy(bin_file, fn_base);
				strcat(bin_file, ".bin");

				strcpy(hex_file, fn_base);
				strcat(hex_file, ".hex");

				rv	= 0;		/*	Success! */
			}
			else
			{
				fprintf(stderr, msg_cam, EC_CAM);
			}
		}
		/*	- No input file base name exist, use default binary file
		 *	  name and Intel hexadecimal file name.
		 *	-------------------------------------------------------- */	
		else
		{
			char	*default_bin_file_name	= "a.bin";
			char	*default_hex_file_name	= "a.hex";

			fprintf(	stderr,
				  		"*** Warning %d: No input file specified at time \"-o\" option was processed!\n",
				  		WC_NIFSATOOP);

			fprintf(	stderr,
				  		"    Using default names (\"a.bin, a.hex\")!\n");

			/*	Allocate memory for the binary file name.
			 *	*/
			bin_file	= (char *) malloc(strlen(default_bin_file_name) + 1);

			/*	Allocate memory for the Intel hexadecimal file name.
			 *	*/
			hex_file	= (char *) malloc(strlen(default_hex_file_name) + 1);

			/*	- Built binary file name and Intel hexadecimal
			 *	  file name, if possible.
			 *	---------------------------------------------- */	
			if ((bin_file != NULL) && (hex_file != NULL))
			{
				strcpy(bin_file, default_bin_file_name);
				strcpy(hex_file, default_hex_file_name);

				rv	= 0;		/*	Success! */
			}
			else
			{
				fprintf(stderr, msg_cam, EC_CAM);
			}
		}
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	check_set_output_fn
 *	Description:	Check Output Files Names, and Set them if necessary.
 *	Author(s):		Claude Sylvain
 *	Created:			26 March 2011
 *	Last modified:	17 December 2011
 *	Parameters:		void
 *
 *	Returns:			int:
 *							-1	: Operation failed.
 *							0	: Operation successfull.
 *
 *	Globals:			char	*bin_file
 *
 *	Notes:			An error upon calling this function is a fatal error.
 *	************************************************************************* */

static int check_set_output_fn(void)
{
	int	rv	= 0;		/*	Return Value (default == 0). */

	/*	- If No input file specified, and no "-o" option specified,
	 *	  this is a fatal error, and must be handled carefully
	 *	  by the caller (main())!
	 *	----------------------------------------------------------- */	 
	if (strlen(fn_base) == 0)
	{
		fprintf(	stderr,
					"*** Error %d: No input file and no \"-o\" option specified!\n",
					EC_NIFSNOOS);

		return (-1);
	}

	/*	Set binary file name, if not already set.
	 *	----------------------------------------- */
	if (bin_file == NULL)
	{
		/*	Allocate memory for the binary file name.
		 *	*/
		bin_file	= (char *) malloc(strlen(fn_base) + 4 + 1);

		/*	Built binary file name, if possible.
		 *	------------------------------------ */	
		if (bin_file != NULL)
		{
			strcpy(bin_file, fn_base);
			strcat(bin_file, ".bin");
		}
		else
		{
			fprintf(stderr, "*** Error %d: Can't allocate memory!\n", EC_CAM);
			rv	= -1;
		}
	}

	/*	Set Intel hexadecimal file name, if not already set.
	 *	---------------------------------------------------- */
	if (hex_file == NULL)
	{
		/*	Allocate memory for the Intel hexadecimal file name.
		 *	*/
		hex_file	= (char *) malloc(strlen(fn_base) + 4 + 1);

		/*	Built Intel hexadecimal file name, if possible.
		 *	----------------------------------------------- */	
		if (hex_file != NULL)
		{
			strcpy(hex_file, fn_base);
			strcat(hex_file, ".hex");
		}
		else
		{
			fprintf(stderr, "*** Error %d: Can't allocate memory!\n", EC_CAM);
			rv	= -1;
		}
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	clean_up
 *	Description:	Clean Up before exiting.
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2010
 *	Last modified:	26 December 2011
 *	Parameters:		void
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void clean_up(void)
{
	struct option_i_t	*p_option_i_cur;
	struct option_i_t	*p_option_i_next;

	SYMBOL	*Local	= Symbols;


	/* Free symbols linked list.
	 *	************************* */

	do {
		free(Local->Symbol_Name);
		free(Local->src_filename);
	} while ((Local = (SYMBOL *) Local->next) != NULL);

	free(Symbols);


	free(ByteWordStack);

	/*	- Since "in_fn[0]" is no more used, we do not have to init.
	 *	  it to NULL.
	 *	*/		  
	free(in_fn[0]);


	/*	- Free memory allocated for "-I" option structures.
	 *	- Notes: First structure is static, and only its "path" member
 	 *	  need to be freed.
	 *	************************************************************** */

	p_option_i_next	= option_i.next;	/*	First structure to look for. */

	/*	Notes: "NULL" if no option "-I" found.
	 *	*/	
	free(option_i.path);

	/*	Free memory associated with option "-I" dynamic structures.
	 *	----------------------------------------------------------- */	
	while (p_option_i_next != NULL)
	{
		p_option_i_cur		= p_option_i_next;
		p_option_i_next	= p_option_i_cur->next;

		free(p_option_i_cur->path);
		free(p_option_i_cur);
	}

	/*	Notes: "NULL" if no option "-l" found.
	 *	*/	
	free(list_file);

	free(bin_file);
	free(hex_file);
}


/*	*************************************************************************
 *	Function name:	process_input_file
 *	Description:	Process Input File.
 *	Author(s):		Claude Sylvain
 *	Created:			31 December 2010
 *	Last modified:	27 December 2011
 *
 *	Parameters:		char *text:
 *							Pointer to text that hold input file.
 *
 *	Returns:			int:
 *							-1	: Operation failed.
 *							0	: Operation successfull.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int process_input_file(char *text)
{
	/*	Tell if Input File was already Processed.
	 *	*/
	static int	if_processed	= 0;

	int		i;
	int		dot_pos	= 0;					/*	Dot Position. */
	size_t	ln			= strlen(text);
	size_t	in_fn_len;


	/*	If input file already processed, exit with error.
	 *	------------------------------------------------- */	
	if (if_processed)
	{
		fprintf(	stderr,
			  		"*** Error %d: Extra Input file specified (\"%s\")!\n",
				  	EC_EIFS, text);

		fprintf(	stderr,
			  		"    This file will be ignored!\n");

		return (-1);
	}

	if_processed	= 1;					/*	Will be Processed. */

	/*	Search for the beginning of an extension.
	 *	----------------------------------------- */
	for (i = 0; i < (ln - 1); i++)
	{
		if (	(text[i] == '.') && (text[i + 1] != '.') &&
			  	(text[i + 1] != PATH_SEPARATOR_CHAR))
		{
			dot_pos	= i;
		}
	}

	/*	If an extension was found...
	 *	---------------------------- */	
	if (dot_pos > 0)
	{
		/*	Check for input file name maximum length.
		 *	----------------------------------------- */
		if (dot_pos <= (sizeof (fn_base) - 4))
		{
			in_fn_len	= ln + 1;
			strncpy(fn_base, text, dot_pos);
			fn_base[dot_pos]	= '\0';
		}
		else
		{
			fprintf(	stderr,
				  		"*** Error %d: Input file name too long (\"%s\")!\n",
					  	EC_IFNTL, text);

			return (-1);
		}
	}
	/*	- If input file name have no extension, take that name
	 *	  as base file name.
	 *	------------------------------------------------------ */	  
	else
	{
		fprintf(	stderr,
			  		"*** Warning %d: Input file name have no extension. Will use \".asm\" (\"%s\")!\n",
				  	WC_IFNHNE, text);

		/*	Check for input file name maximum length.
		 *	----------------------------------------- */
		if (ln <= (sizeof (fn_base) - 1))
		{
			in_fn_len	= ln + 4 + 1;
			strcpy(fn_base, text);
		}
		else
		{
			fprintf(	stderr,
				  		"*** Error %d: Input file name too long (\"%s\")!\n",
					  	EC_IFNTL, text);

			return (-1);
		}
	}

	/*	Allocate memory for the input file name.
	 *	*/	
	in_fn[0]	= (char *) malloc(in_fn_len);

	/*	Check for memory allocation error.
	 *	--------------------------------- */	
	if (in_fn[0] == NULL)
	{
		fprintf(stderr, "*** Error %d: Memory allocation error!\n", EC_MAE);
		return (-1);
	}

	/*	- If an extension was found, set input file name as the
	 *	  input file name specified in the command line.
	 *	- Otherwise, set the input file name as the input
	 *	  file name specified in the command line + ".asm"
	 *	  extension.	  
	 *	------------------------------------------------------- */	  
	if (dot_pos > 0)
		strcpy(in_fn[0], text);		/*	File name with extension. */
	else
	{
		strcpy(in_fn[0], text);		/*	File name (only). */
		strcat(in_fn[0], ".asm");	/*	Add default extension. */
	}

	return (0);
}


/*	*************************************************************************
 *	Function name:	cmd_line_parser
 *	Description:	Command Line Parser.
 *	Author(s):		Claude Sylvain
 *	Created:			31 December 2010
 *	Last modified:	1 January 2012
 *
 *	Parameters:		int argv:
 *							...
 *
 *						char *argc[]:
 *							...
 *
 *	Returns:			int:
 *							-1	: Operation failed.
 *							0	: User just one help.  Do not assemble.
 *							1	: Ready to assemble.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int cmd_line_parser(int argc, char *argv[])
{
	int	rv		= -1;
	int	pgm_par_cnt;		/*	Program Parameter Count. */


	/*	Need at least 2 arguments.
	 *	-------------------------- */	
	if (argc > 1)
	{
		pgm_par_cnt		= argc - 1;
		argv++;								/*	Select first program parameter. */

		/*	Process program parameters.
		 *	--------------------------- */	
		while (pgm_par_cnt > 0)
		{
			switch (**argv)
			{
				case '-':
					switch (*(*argv + 1))
					{
						/*	"-I" option.
						 *	------------ */
						case 'I':
							process_option_i(*argv);
							break;

						/*	"-l" option.
						 *	------------ */
						case 'l':
							process_option_l(*argv);
							break;

						/*	"-o" option.
						 *	------------ */
						case 'o':
							process_option_o(*argv);
							break;

						/*	- On "-h" option of unknown option, display
						 *	  help and exit.
						 *	------------------------------------------- */	 
						case 'v':
							display_version();
							rv				= 0;		/*	Just display.  Do not assemble. */
							pgm_par_cnt	= 0;		/*	Force Exit. */
							break;

						/*	- On "-h" option of unknown option, display
						 *	  help and exit.
						 *	------------------------------------------- */	 
						case 'h':
						default:
							display_help();
							rv				= 0;		/*	Just display.  Do not assemble. */
							pgm_par_cnt	= 0;		/*	Force Exit. */
							break;

					}

					break;

				default:
					if (process_input_file(*argv) != 1)
						rv	= 1;

					break;
			}

			pgm_par_cnt--;
			argv++;
		}	
	}
	/*	Just 1 argument.  Need at least 2 arguments.
	 *	Alert user.
	 *	-------------------------------------------- */	
	else
	{
		printf("%s: No input file! Use -h option for help.\n", name_pgm);
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	main
 *	Description:	Main function.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2011
 *
 *	Parameters:		int argv:
 *							...
 *
 *						char *argc[]:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int main(int argc, char *argv[])
{
	init();						/*	Initialize module. */
	if_true[0]		= 1;		/*	"IF" nesting base level (always TRUE). */ 

	Symbols			= (SYMBOL *) calloc(1, sizeof(SYMBOL));
	ByteWordStack	= (STACK *) calloc(1, sizeof(STACK));

	/*	Check for memory allocation error.
	 *	---------------------------------- */	
	if ((Symbols == NULL) || (ByteWordStack == NULL))
	{
		fprintf(stderr, "*** Error %d: Can't allocate memory!\n", EC_CAM);
		clean_up();

		return (-1);
	}

	memset(Image, 0, sizeof (Image));

	/*	Need at least one parameter.
	 *	---------------------------- */
	if (cmd_line_parser(argc, argv) > 0) 
	{
		/*	Check/Set Output File Name; Open Files.
		 *	If there was an error, abort operation.
	 	 *	--------------------------------------- */	 
		if ((check_set_output_fn() == -1) || (OpenFiles() == -1))
		{
			CloseFiles();
			clean_up();
			return (-1);
		}

		asm_pass1();
		asm_pass2();
		CloseFiles();
	}

	asm_dir_cleanup();		/*	"asm_dir" module Cleanup. */
	clean_up();					/*	Clean Up module. */

	return (0);
}



