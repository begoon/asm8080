/*	*************************************************************************
 *	Module Name:	exp_parser.c
 *
 *	Description:	- Expression Parser module.
 *
 *						- Support most of standard 8080 assembler operators.
 *
 *						- Support some "C" like operators, when language
 *						  extension is enable.
 *
 *	Copyright(c):	See below...
 *	Author(s):		Claude Sylvain
 *	Created:			27 December 2010
 *	Last modified:	4 January 2012
 *
 *	Notes:			- This module implement an expression parser using
 *						  DAL (Direct Algebraic Logic) format.
 *						  RPN (Reverse Polish Notation) has been dropped down,
 *						  since January 2011.
 *
 * Ref.:				http://en.wikipedia.org/wiki/Reverse_Polish_notation
 *
 *	- Expressions can be as simple as 1, or complex like
 *	  "xyzzy"+23/7(45*3)+16<<test3
 * 
 *	- Parseing forms that this parser will handle:
 *			(expression) -- (stack) = (expression) [op] (expression)
 * 
 *	- Expression can have operators, strings, numbers and labels.
 * 
 *	- Strings can be single quote, or double quote.
 *	  i.e.  'AB' or "AB".
 *	- Numbers can be base 10, base 16, base 8 or base 2.
 *	  i.e.  124 (124d), 3Eh, 12q or 01001001b.
 * 
 *	- At the end of the process, return (stack).
 * 
 *	- A typical expression that is seen in assembly is:
 *			LXI	H,<exp>
 *	  where <exp> is:
 *			("EV"+STARTOFTABLE)/4
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
#include "main.h"
#include "msg.h"
#include "exp_parser.h"


/*	*************************************************************************
 *											  CONSTANTS
 *	************************************************************************* */

#define STACK_LEVELS			8


/*	*************************************************************************
 *												 STRUCT
 *	************************************************************************* */

struct operator_t
{
	const char	*name;
	int			op;
};


/*	*************************************************************************
 *												 STRUCT
 *	************************************************************************* */

/*	Expression Parser Stack.
 *	------------------------ */
struct ep_stack_t
{
	struct ep_stack_t	*prev;
	int					word[STACK_LEVELS];
	int					level;
};


/*	*************************************************************************
 *												  ENUM
 *	************************************************************************* */

/*	Expression Parser Stack Operators.
 *	Ref.: "8080-8085_assembly_language_programming_1977_intel.pdf", page 2.16
 *	------------------------------------------------------------------------- */	
enum Operators
{
	/*	Operators using a single character.
	 *	----------------------------------- */
	OP_ADD,
  	OP_SUB,
  	OP_MUL,
	OP_DIV,

	/*	Operators using a string.
	 *	------------------------- */
//	OP_NUL,			/*	Test for null (missing) macro parameters. */
	OP_HIGH,
	OP_LOW,
	OP_MOD,
	OP_SHL,
	OP_SHR,
	OP_EQ,
	OP_LT,
	OP_LE,
	OP_GT,
	OP_GE,
	OP_NE,
	OP_NOT,
	OP_AND,
	OP_OR,
	OP_XOR
};


/*	*************************************************************************
 *												 CONST
 *	************************************************************************* */

/*	Private "const".
 *	**************** */

/*	Operators.
 *	Ref.: "8080-8085_assembly_language_programming_1977_intel.pdf", page 2.16
 *	------------------------------------------------------------------------- */	
static const struct operator_t	operator[]	=
{
//	{"NUL",	OP_NUL},		/*	Test for null (missing) macro parameters. */
	{"HIGH",	OP_HIGH},
	{"LOW",	OP_LOW},
	{"MOD",	OP_MOD},
	{"SHL",	OP_SHL},
	{"SHR",	OP_SHR},
	{"EQ",	OP_EQ},
	{"LT",	OP_LT},
	{"LE",	OP_LE},
	{"GT",	OP_GT},
	{"GE",	OP_GE},
	{"NE",	OP_NE},
	{"NOT",	OP_NOT},
	{"AND",	OP_AND},
	{"OR",	OP_OR},
	{"XOR",	OP_XOR},
  	{NULL,	0}
};


/*	*************************************************************************
 *	                          FUNCTIONS DECLARATION
 *	************************************************************************* */

/*	Private functions.
 *	****************** */

static int search_operator(char *text, int *text_bp);
static int pop(void);
static void push(int);
static int add_stack(void);
static int remove_stack(void);
static int dalep(char *text);
static void eval(void);


/*	*************************************************************************
 *	                                VARIABLES
 *	************************************************************************* */

/*	Private variables.
 *	****************** */

/*	- Expression Parser Stack.
 *	- Notes: This is the static base stack.  All other stacks
 *	  are dynamically created as needed.
 *	*/
static struct ep_stack_t	ep_stack;

static struct ep_stack_t	*p_ep_stack	= &ep_stack;


/*	*************************************************************************
 *	                           FUNCTIONS DEFINITION
 *	************************************************************************* */


/*	*************************************************************************
 *	Function name:	search_operator
 *	Description:	Search for an Operator.
 *	Author(s):		Claude Sylvain
 *	Created:			28 December 2010
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *text:
 *							Point to text that possibly hold an operator.
 *
 *						int *text_bp:
 *							Point to a variable that receive the number of
 *							text character globed, and have to be bypassed
 *							by the caller.
 *
 *	Returns:			int:
 *							-1		: No operator found.
 *							> 0	: Operator value.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int search_operator(char *text, int *text_bp)
{
	int	rv;
	char	*p_text;
	int	i				= 0;

	struct operator_t	*p_operator	= (struct operator_t *) operator;


	*text_bp	= 0;

	p_text	= (char *) malloc(strlen(text) + 1);

	if (p_text == NULL)
	{
		fprintf(stderr, "*** Error %d: Memory allocation error!\n", EC_MAE);

		return (-1);
	}

	/*	Bypass space.
	 *	------------- */	
	while ((isspace((int) *text) != 0) && (*text != '\0'))
	{
		(*text_bp)++;
		text++;
	}


	/*	Get an uppercase working copy of "text".
	 *	**************************************** */
		
	while (*text != '\0')
	{
		if (isalnum((int) *text) == 0)
			break;

			(*text_bp)++;
			p_text[i++]	= toupper((int) *(text++));
	}

	p_text[i]	= '\0';

	/*	Search the operator.
	 *	-------------------- */	
	while (p_operator->name != NULL)
	{
		if (strcmp(p_text, p_operator->name) == 0)
			break;

		p_operator++;
	}

	if (p_operator->name == NULL)
	{
		*text_bp	= 0;
		rv			= -1;
	}
	else
		rv	= p_operator->op;

	free(p_text);		/*	Free allocated memory. */

	return (rv);
}


/*	*************************************************************************
 *	Function name:	extract_byte
 *	Description:	Extract Byte.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2011
 *
 *	Parameters:		char *text:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int extract_byte(char *text)
{
	int	accum	= 0;
	int	inc	= 0;
	int	c;


	/*	Get numerical base increment, if possible.
	 *	------------------------------------------ */	
	if (isdigit((int) *text))
		inc	= util_get_number_base_inc(text);

	/*	Get the job done only if no error.
 	 *	---------------------------------- */	 
	if (inc != -1)
	{
		while (*text)
		{
			switch (*text)
			{
				case '+':
					text++;

					/*	Get numerical base increment, if possible.
					 *	------------------------------------------ */	
					if (isdigit((int) *text))
						inc	= util_get_number_base_inc(text);

					if (inc == -1)
					{
						msg_error("Bad data encoding!", EC_BDE);
						return (accum);
					}

					break;

				case '-':
					text++;

					/*	Get numerical base increment, if possible.
					 *	------------------------------------------ */	
					if (isdigit((int) *text))
						inc	= util_get_number_base_inc(text);

					if (inc == -1)
					{
						msg_error("Bad data encoding!", EC_BDE);
						return (accum);
					}

					break;

				case '\'':
					text++;
					accum = *text;
					return (accum);

				case '\0':
				case '\n':
				case '\t':
				case '\"':
					return (accum);

				case ' ':
					if (accum)
						return (accum);

					text++;

					/*	Get numerical base increment, if possible.
					 *	------------------------------------------ */	
					if (isdigit((int) *text))
						inc	= util_get_number_base_inc(text);

					if (inc == -1)
					{
						msg_error("Bad data encoding!", EC_BDE);
						return (accum);
					}

					break;

				case ',':
					return (accum);

				default:
					break;
			}

			accum *= inc;
			c		= toupper((int) *text);

			/*	Check/Convert/Accumulate, depending of the numerical base.
			 *	---------------------------------------------------------- */	
			switch (inc)
			{
				/*	Binary base.
				 *	------------ */	
				case 2:
					if ((c >= '0') && (c <= '1'))
					{
						accum += *(text++) - '0';
					}
					else
					{
						msg_error_c("Bad binary digit!", EC_BBD, *text);

						return (accum);	/* Punt. */
					}

					break;

				/*	Octal base.
				 *	----------- */	
				case 8:
					/*	Convert/Accumulate.
					 *	------------------- */	
					if ((c >= '0') && (c <= '7'))
					{
						accum += *(text++) - '0';
					}
					else
					{
						msg_error_c("Bad octal digit!", EC_BOC, *text);

						return (accum);	/* Punt. */
					}

					break;

				/*	Decimal base.
				 *	------------- */	
				case 10:
					/*	Convert/Accumulate.
					 *	------------------- */	
					if (isdigit(c) != 0)
					{
						accum += *(text++) - '0';
					}
					else
					{
						msg_error_c("Bad decimal digit!", EC_BDD, *text);

						return (accum);	/* Punt. */
					}

					break;

				/*	Hexadecimal base.
				 *	----------------- */	
				case 16:
					/*	Convert/Accumulate.
					 *	------------------- */	
					if (isdigit(c) != 0)
					{
						accum += *(text++) - '0';
					}
					else if ((c >= 'A') && (c <= 'F'))
					{
						accum += (toupper((int) *(text++)) - 'A') + 10;
					}
					else
					{
						msg_error_c("Bad hexadecimal digit!", EC_BHD, *text);

						return (accum);	/* Punt. */
					}

					break;

				default:
					msg_error_s("Bad data!", EC_BD, text);
					return (0);
			}
		}
	}
	else
	{
		msg_error("Bad data encoding!", EC_BDE);
	}

	return (accum);
}


/*	*************************************************************************
 *	Function name:	eval
 *	Description:	Evaluate an expression.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2011
 *	Parameters:		void
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void eval(void)
{
	/*	Binary operations.
	 *	------------------ */
	if (p_ep_stack->level == 3)
	{
		int	b	= pop();
		int	op	= pop();
		int	a	= pop();

		switch (op)
		{
			case OP_ADD:
				push(a + b);
				break;

			case OP_SUB:
				push(a - b);
				break;

			case OP_MUL:
				push(a * b);
				break;

			case OP_DIV:
				push(a / b);
				break;

			case OP_SHL:
				push(a << b);
				break;

			case OP_SHR:
				push(a >> b);
				break;

			case OP_MOD:
				push(a % b);
				break;

			case OP_EQ:
				push(a == b);
				break;

			/*	Lower Than.
			 *	----------- */
			case OP_LT:
				push(a < b);
				break;

			/*	Lower of Equal.
			 *	--------------- */
			case OP_LE:
				push(a <= b);
				break;

			/*	Greater Than.
			 *	------------- */
			case OP_GT:
				push(a > b);
				break;

			/*	Greater or Equal.
			 *	----------------- */
			case OP_GE:
				push(a >= b);
				break;

			/*	Not Equal.
			 *	---------- */
			case OP_NE:
				push(a != b);
				break;

			case OP_AND:
				push(a & b);
				break;

			case OP_OR:
				push(a | b);
				break;

			case OP_XOR:
				push(a ^ b);
				break;

			default:
				break;
		}
	}
	/*	Unary operations.
	 *	----------------- */
	else if (p_ep_stack->level == 2)
	{
		int	a	= pop();
		int	op	= pop();


		switch (op)
		{
			case OP_HIGH:
				push((a >> 8) & 0xFF);
				break;

			case OP_LOW:
				push(a & 0xFF);
				break;

			case OP_ADD:
				push(a);
				break;

			case OP_SUB:
				push(-a);
				break;

			case OP_NOT:
				push(~a);
				break;

			default:
				break;
		}
	}
}


/*	*************************************************************************
 *	Function name:	add_stack
 *	Description:	Add Stack .
 *	Author(s):		Claude Sylvain
 *	Created:			2 January 2011
 *	Last modified:	27 December 2011
 *	Parameters:		void
 *
 *	Returns:			int:
 *							-1	: Operation failed.
 *							0	: Operation successfull.
 *
 *	Globals:
 *	Notes:
 *	Warning:
 *	************************************************************************* */

static int add_stack(void)
{
	int	rv	= -1;

	/*	Create a new stack.
	 *	*/	
	struct ep_stack_t	*p_ep_stack_new =
		(struct ep_stack_t *) malloc(sizeof (struct ep_stack_t));

	/*	If stack creation was successfull...
	 *	------------------------------------ */	
	if (p_ep_stack_new != NULL)
	{
		/*	Init. previous stack link in the newly created stack.
		 *	*/
		p_ep_stack_new->prev	= p_ep_stack;

		/*	Make the newly created stack the current stack.
		 *	*/
		p_ep_stack				= p_ep_stack_new;

		p_ep_stack->level		= 0;		/*	Init. level. */
		rv							= 0;		/*	Success! */
	}
	else
	{
		msg_error("Memory Allocation Error!", EC_MAE);
	}

	return (rv);
}


/*	*************************************************************************
 *	Function name:	remove_stack
 *	Description:	Remove Stack .
 *	Author(s):		Claude Sylvain
 *	Created:			2 January 2011
 *	Last modified:	27 December 2011
 *	Parameters:		void
 *
 *	Returns:			int:
 *							-1	: Operation failed.
 *							0	: Operation successfull.
 *
 *	Globals:
 *	Notes:
 *	Warning:
 *	************************************************************************* */

static int remove_stack(void)
{
	int	rv	= -1;

	struct ep_stack_t	*p_ep_stack_prev;

	/*	Remove only if not at stack base level.
	 *	--------------------------------------- */	
	if (p_ep_stack != &ep_stack)
	{
		p_ep_stack_prev	= p_ep_stack->prev;	/*	Memoryse previous stack address. */
		free(p_ep_stack);								/*	Free current stack. */

		/*	Set current stack address to the previous stack address.
		 *	*/
		p_ep_stack			= p_ep_stack_prev;

		rv	= 0;		/*	Success! */
	}
	else
		msg_error("Expression parser stack remove underflow!", EC_EPSRUF);

	return (rv);
}


/*	*************************************************************************
 *	Function name:	dalep
 *	Description:	DAL (Direct Algebraic Logic) Expression Parser.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	29 December 2011
 *
 *	Parameters:		char *text:
 *							- Point to a string that hold expression to parse
 *							  and evaluate.
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	Warning:			Recursive parseing.
 *	************************************************************************* */

static int dalep(char *text)
{
	int	msg_displayed	= 0;

	/*	Parse all expression.
	 *	--------------------- */
	while (1)
	{
		switch (*text)
		{
			/*	Single character.
			 *	----------------- */
			case '\'':
			{
				text++;
				push(*(text++));

				/*	Check for matching quote.
				 *	------------------------- */
				if (*text == '\'')
					text++;		/*	Bypass quote. */
				else
					msg_warning("Missing quote!", WC_MQ);

				break;
			}

#if LANG_EXTENSION
			/*	- If string is found, alert user that string can not be
			 *	  evaluated here, and bypass that string.
		 	 *	------------------------------------------------------- */	 
			case '\"':
				msg_error_s("String can not be evaluated here!", EC_SCNBE, text);

				text++;		/*	Bypass quote. */

				/*	Bypass string.
				 *	-------------- */	
				while (1)
				{
					if (*text == '"')
					{
						text++;		/*	Bypass quote. */
						break;
					}

					if (*text == '\0')
					{
						msg_error("Missing quote!", EC_MQ);
						return (0);
					}

					text++;
				}

				break;
#endif

			/*	Handle '('.
			 *	----------- */	
			case '(':
			{
				int	p_level	= 0;		/*	Parenthesis Level. */
				int	val;

				text++;			/*	Bypass '(' */
				add_stack();	/*	Add a new Stack. */

				/*	Recursively Evaluate expression inside parenthise(s).
				 *	*/
				val	= dalep(text);

				remove_stack();
				push(val);			/*	Push result into the stack	. */

				/*	Bypass expression(s) in parenthise(s).
				 *	-------------------------------------- */
				while (*text != '\0')
				{
					/*	Record additionnal parenthesis level.
					 *	------------------------------------- */	
					if (*text == '(')
						p_level++;

					/*	Check for matching ')'.
					 *	----------------------- */	
					if (*text == ')')
					{
						if (--p_level < 0)
							break;
					}

					text++;
				}

				/*	If no matching ')" found ...
				 *	---------------------------- */	
				if (*text == '\0')
				{
					msg_error("No matching ')'!", EC_NMEP);
				}
				/*	Bypass matching ')'.
				 *	-------------------- */		
				else
					text++;

				break;
			}

			/*	Handle ')'.
			 *	----------- */	
			case ')':
				eval();									/*	Evaluate partial expression. */
				return (p_ep_stack->word[0]);

			/*	- "!" character tell us that the next character is a delimitor
			 *	  and must be considered as an ordinary character.
			 *	  So, just gobble the '!' character, then goto keyword
			 *	  parser section.
			 *	-------------------------------------------------------------- */
			case '!':
				text++;
				goto	dalep_01;

#if LANG_EXTENSION == 0
			/*	- '&' is a concatenation operator.
			 *	- TODO: To implement.  For the moment, just bypass
			 *	  this character.
			 *	-------------------------------------------------- */
			case '&':
				text++;
				goto	dalep_01;
#endif

#if LANG_EXTENSION
			/*	"C" like "~" operators.
			 *	----------------------- */
			case '~':
				push(OP_NOT);
				text++;
				break;

			/*	"C" like "|" and "||" operators.
			 *	-------------------------------- */
			case '|':
				push(OP_OR);
				text++;

				if (*text == '|')
					text++;

				break;

			/*	"C" like '^' operator.
			 *	---------------------- */
			case '^':
				push(OP_XOR);
				text++;
				break;

			/*	"C" like "==" operator and Pascal like '=' operator.
			 *	---------------------------------------------------- */
			case '=':
			{
				char	nc	= *(text + 1);		/*	Next Character. */

				push(OP_EQ);

				/*	If it is a "C" like "==" operator...
				 *	------------------------------------ */
				if (nc == '=')
					text	+= 2;
				/*	It is a Pascal like '=' operator...
				 *	----------------------------------- */
				else
					text++;

				break;
			}

			/*	"C" like "<<", "<=" and "<" operators.
			 *	-------------------------------------- */
			case '<':
			{
				char	nc	= *(text + 1);		/*	Next Character. */

				/*	If it is a left shift...
				 *	------------------------ */
				if (nc == '<')
				{
					push(OP_SHL);
					text	+= 2;
				}
				/*	If it is a Lower or Equal relationnal operator.
				 *	----------------------------------------------- */
				else if (nc == '=')
				{
					push(OP_LE);
					text	+= 2;
				}
				/*	If it is a Lower Than relationnal operator.
				 *	------------------------------------------- */
				else
				{
					push(OP_LT);
					text++;
				}

				break;
			}

			/*	"C" like ">>", ">=" and ">" operators.
			 *	-------------------------------------- */
			case '>':
			{
				char	nc	= *(text + 1);		/*	Next Character. */

				/*	If it is a right shift...
				 *	------------------------- */
				if (nc == '>')
				{
					push(OP_SHR);
					text	+= 2;
				}
				/*	If it is a Greater or Equal relationnal operator.
				 *	------------------------------------------------- */
				else if (nc == '=')
				{
					push(OP_GE);
					text	+= 2;
				}
				/*	If it is a Greater Than relationnal operator.
				 *	--------------------------------------------- */
				else
				{
					push(OP_GT);
					text++;
				}

				break;
			}

			/*	- This can be an 8080 assembler concatenation operator
			 *	  or one of the "C" like "&" or "&&" operators.
			 *	- Notes: There is a trick to be able to distinguish both kind
			 *	  of operators, and avoid conflics.  Usualy, when '&' or
			 *	  "&&" "C" like operators are used, they are surrounded
			 *	  by spaces; and this is not the case for the '&' 8080
			 *	  assembler operator, since this is a concatenation
			 *	  operator.
			 *	------------------------------------------------------------- */
			case '&':
				/*	- If '&' or "&&" is followed by a Space character, this
				 *	  is probably not a concatenation operator, but
				 *	  a "C" like operator.
				 *	------------------------------------------------------- */	  
				if (	(*(text + 1) == 0x20) ||
					  	((*(text + 1) == '&') && (*(text + 2) == 0x20)))
				{
					push(OP_AND);
					text++;

					if (*text == '&')
						text++;
				}
				/*	- '&' is a concatenation operator.
				 *	- TODO: To implement.  For the moment, just bypass
				 *	  this character.
				 *	-------------------------------------------------- */
				else
				{
					text++;
					goto	dalep_01;
				}

				break;
#endif		/*	LANG_EXTENSION */	

			case '+':
				push(OP_ADD);
				text++;
				break;

			case '-':
				push(OP_SUB);
				text++;
				break;

			case '*':
				push(OP_MUL);
				text++;
				break;

			case '/':
				push(OP_DIV);
				text++;
				break;

			case '$':
				push(target.pc);
				text++;
				break;

			case ' ':
			case '\t':
				text++;
				break;

			case '\0':
			case '\n':
			case ',':
			case ';':
				eval();
				return (p_ep_stack->word[0]);

			/*	Must be an operator, a label or a number.
			 *	----------------------------------------- */
			default:
dalep_01:				
			{
				int	text_bp;
				int	op;

				/*	Search for an operator.
				 *	----------------------- */	
				op	= search_operator(text, &text_bp);

				/*	If an operator was found, process it.
				 *	------------------------------------- */	
				if (op != -1)
				{
					push(op);				/*	Push Operator. */
					text	+=	text_bp;		/*	Bypass operator. */
				}
				/*	No operator found.
				 *	Search for a label.
			 	 *	------------------- */
				else
				{
					SYMBOL	*Local;
					char		label[LABEL_SIZE_MAX];
					int		i			= 0;

					memset(label, 0, sizeof (label));

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
							break;
						}
					}

					/*	Process label or number only if it exist.
					 *	----------------------------------------- */	
					if (i > 0)
					{
						/*	Could be a number.
						 *	------------------ */
						if (isdigit((int) label[0]))
						{
							push(extract_byte(label));
							eval();
						}
						else
						{
							Local = FindLabel(label);

							if (!Local)
							{
								msg_error_s("Label not found!", EC_LNF, label);
								return (0);
							}

							push(Local->Symbol_Value);
							eval();
						}

					}
					/*	Label do not exist!
					 *	------------------- */	
					else
					{
						msg_error("Missing field!", EC_MF);
						return (0);
					}

				}		/*	op != -1 */
			}			/*	default. */

			break;
		}
	}

	return (0);
}


/*	*************************************************************************
 *	Function name:	exp_parser
 *	Description:	Expression Parser main entry point.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2011
 *
 *	Parameters:		char *text:
 *							- Point to a string that hold expression to parse
 *							  and evaluate.
 *
 *	Returns:			int:
 *							Resulting expression Value.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int exp_parser(char *text)
{
	p_ep_stack->level	= 0;

	return (dalep(text));
}


/*	*************************************************************************
 *	Function name:	extract_word
 *	Description:	Extract Word.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2010
 *
 *	Parameters:		char *text:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int extract_word(char *text)
{
	int	tmp;

	while (isspace((int) *text))
		text++;

	if (*text == '%')
		return (0);

	if (*text == '\"')
	{
		text++;
		tmp	= *text << 8;
		text++;
		tmp	|= *text;
	}
	else if (*text == '-')
	{
		text++;
		tmp	= extract_byte(text);

		return (-tmp);
	}
	else if (*text == '+')
	{
		text++;
		tmp	= extract_byte(text);

		return (+tmp);
	}
	else
		tmp	= exp_parser(text);

	return (tmp);
}


/*	*************************************************************************
 *	Function name:	push
 *	Description:	Push a value to the stack.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2011
 *
 *	Parameters:		int value:
 *							Value to push to the stack.
 *
 *	Returns:			void
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static void push(int value)
{
	int	i;

	/*	Update stack words.
	 *	------------------- */	
	for (i = STACK_LEVELS - 1; i > 0; i--)
		p_ep_stack->word[i] = p_ep_stack->word[i -1];

	p_ep_stack->word[0] = value;		/*	Add new value in the stack. */

	/*	Check for stack overflow.
	 *	------------------------- */	
	if (++p_ep_stack->level >= (sizeof (p_ep_stack->word) / sizeof (int)))
	{
		p_ep_stack->level--;
		msg_error("Expression parser stack push overflow!", EC_EPSPOF);
	}
}


/*	*************************************************************************
 *	Function name:	pop
 *	Description:	Pop a value from the stack.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2011
 *	Parameters:		void
 *
 *	Returns:			int:
 *							Value poped from the stack.
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int pop(void)
{
	int	i;
	int	value	= p_ep_stack->word[0];		/*	Get value from the stack. */

	/*	Update stack words.
	 *	------------------- */
	for (i = 0; i < STACK_LEVELS - 1; i++)
		p_ep_stack->word[i] = p_ep_stack->word[i + 1];

	/*	Check for stack underflow.
	 *	-------------------------- */	
	if (--p_ep_stack->level < 0)
	{
		p_ep_stack->level	= 0;
		msg_error("Exression parser stack pop underflow!", EC_EPSPUF);
	}

	return (value);
}






