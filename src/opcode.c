/*	*************************************************************************
 *	Module Name:	opcode.c
 *	Description:	Opcode.
 *	Copyright(c):	See below...
 *	Author(s):		Claude Sylvain
 *	Created:			24 December 2010
 *	Last modified:	4 January 2012
 * Notes:
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
#include "exp_parser.h"
#include "main.h"
#include "msg.h"
#include "opcode.h"


/*	*************************************************************************
 *											  CONSTANTS
 *	************************************************************************* */

/*	"parse_reg16bits()" Allowed 16-bit Register bit mask.
 *	----------------------------------------------------- */
#define PR16_AR_BC			0x01
#define PR16_AR_DE			0x02
#define PR16_AR_HL			0x04
#define PR16_AR_SP			0x08
#define PR16_AR_PSW			0x10


/*	*************************************************************************
 *	                          FUNCTIONS DECLARATION
 *	************************************************************************* */

/*	Private functions.
 *	****************** */

static char *parse_reg16bits(char *text, unsigned char ar);
static char *DestReg(char *text);
static char *SourceReg(char *text);

static int proc_add(char *, char *);
static int proc_adi(char *, char *);
static int proc_adc(char *, char *);
static int proc_aci(char *, char *);
static int proc_ana(char *, char *);
static int proc_ani(char *, char *);

static int proc_cmp(char *, char *);
static int proc_cpi(char *, char *);
static int proc_cma(char *, char *);
static int proc_cmc(char *, char *);
static int proc_call(char *, char *);
static int proc_cnz(char *, char *);
static int proc_cz(char *, char *);
static int proc_cnc(char *, char *);
static int proc_cc(char *, char *);
static int proc_cpo(char *, char *);
static int proc_cpe(char *, char *);
static int proc_cp(char *, char *);
static int proc_cm(char *, char *);

static int proc_dcr(char *, char *);
static int proc_dcx(char *, char *);
static int proc_daa(char *, char *);
static int proc_dad(char *, char *);
static int proc_di(char *, char *);

static int proc_ei(char *, char *);

static int proc_hlt(char *, char *);

static int proc_in(char *label, char *equation);
static int proc_inr(char *, char *);
static int proc_inx(char *, char *);

static int proc_jmp(char *, char *);
static int proc_jnz(char *, char *);
static int proc_jz(char *, char *);
static int proc_jnc(char *, char *);
static int proc_jc(char *, char *);
static int proc_jpo(char *, char *);
static int proc_jpe(char *, char *);
static int proc_jp(char *, char *);
static int proc_jm(char *, char *);

static int proc_lxi(char *, char *);
static int proc_lhld(char *, char *);
static int proc_lda(char *, char *);
static int proc_ldax(char *, char *);

static int proc_mov(char *, char *);
static int proc_mvi(char *, char *);

static int proc_nop(char *label, char *equation);

static int proc_ora(char *, char *);
static int proc_ori(char *, char *);
static int proc_out(char *label, char *equation);

static int proc_pchl(char *, char *);
static int proc_pop(char *, char *);
static int proc_push(char *, char *);

static int proc_rlc(char *, char *);
static int proc_rrc(char *, char *);
static int proc_ral(char *, char *);
static int proc_rar(char *, char *);
static int proc_ret(char *, char *);
static int proc_rnz(char *, char *);
static int proc_rz(char *, char *);
static int proc_rnc(char *, char *);
static int proc_rc(char *, char *);
static int proc_rpo(char *, char *);
static int proc_rpe(char *, char *);
static int proc_rp(char *, char *);
static int proc_rm(char *, char *);
static int proc_rst(char *label, char *equation);

static int proc_sta(char *, char *);
static int proc_shld(char *, char *);
static int proc_stax(char *, char *);
static int proc_sub(char *, char *);
static int proc_sui(char *, char *);
static int proc_sbb(char *, char *);
static int proc_sbi(char *, char *);
static int proc_stc(char *, char *);
static int proc_sphl(char *, char *);

static int proc_xchg(char *, char *);
static int proc_xra(char *, char *);
static int proc_xri(char *, char *);
static int proc_xthl(char *, char *);


/*	*************************************************************************
 *												 CONST
 *	************************************************************************* */

/*	Public "const".
 *	*************** */

/*	Opcodes.
 *	-------- */	
const keyword_t	OpCodes[] =
{
	{"MOV", proc_mov},			{"MVI", proc_mvi},	{"LXI", proc_lxi},
	{"LDA", proc_lda},			{"STA", proc_sta},	{"LHLD", proc_lhld},
	{"SHLD", proc_shld},			{"LDAX", proc_ldax},	{"STAX", proc_stax},
	{"XCHG", proc_xchg},			{"ADD", proc_add},	{"ADI", proc_adi},
	{"ADC", proc_adc},			{"ACI", proc_aci},	{"SUB", proc_sub},
	{"SUI", proc_sui},			{"SBB", proc_sbb},	{"SBI", proc_sbi},
	{"INR", proc_inr},			{"DCR", proc_dcr},	{"INX", proc_inx},
	{"DCX", proc_dcx},			{"DAD", proc_dad},	{"DAA", proc_daa},
	{"ANA", proc_ana},			{"ANI", proc_ani},	{"ORA", proc_ora},
	{"ORI", proc_ori},			{"XRA", proc_xra},	{"XRI", proc_xri},
	{"CMP", proc_cmp},			{"CPI", proc_cpi},	{"RLC", proc_rlc},
	{"RRC", proc_rrc},			{"RAL", proc_ral},	{"RAR", proc_rar},
	{"CMA", proc_cma},			{"CMC", proc_cmc},	{"STC", proc_stc},
	{"JMP", proc_jmp},			{"JNZ", proc_jnz},	{"JZ", proc_jz},
	{"JNC", proc_jnc},			{"JC", proc_jc},		{"JPO", proc_jpo},
	{"JPE", proc_jpe},			{"JP", proc_jp},		{"JM", proc_jm},
	{"CALL", proc_call},			{"CNZ", proc_cnz},	{"CZ", proc_cz},
	{"CNC", proc_cnc},			{"CC", proc_cc},		{"CPO", proc_cpo},
	{"CPE", proc_cpe},			{"CP", proc_cp},		{"CM", proc_cm},
	{"RET", proc_ret},			{"RNZ", proc_rnz},	{"RZ", proc_rz},
	{"RNC", proc_rnc},			{"RC", proc_rc},		{"RPO", proc_rpo},
	{"RPE", proc_rpe},			{"RP", proc_rp},		{"RM", proc_rm},
	{"RST", proc_rst},			{"PCHL", proc_pchl},	{"PUSH", proc_push},
	{"POP", proc_pop},			{"XTHL", proc_xthl},	{"SPHL", proc_sphl},
	{"IN", proc_in},				{"OUT", proc_out},	{"EI", proc_ei},
	{"DI", proc_di},				{"HLT", proc_hlt},	{"NOP", proc_nop},
	{0, NULL}
};


/*	*************************************************************************
 *	                           FUNCTIONS DEFINITION
 *	************************************************************************* */


/*	*************************************************************************
 *	Function name:	parse_reg16bits
 *	Description:	Parse 16-bit Register.
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2011
 *
 *	Parameters:		char *text:
 *							...
 *
 *						unsigned char ar:
 *							Allowed 16-bit Registers (See "PR16_AR_x" defines).
 *
 *	Returns:			char *:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static char *parse_reg16bits(char *text, unsigned char ar)
{
#define PARSE_REG16BITS_BUF_SIZE		4

	char	buf[PARSE_REG16BITS_BUF_SIZE];
	char	buf_ori[PARSE_REG16BITS_BUF_SIZE];	/*	Buffer for original text. */

	int	i						= 0;
	int	text_len				= 0;
	char	*p_text				= text;
	int	reg_not_allowed	= 0;


	/*	Get register pair name.
	 *	----------------------- */
	while (	(*p_text != '\0') && (*p_text != ',') &&
		  		(isspace((int) *p_text)) == 0)
	{
		text_len++;
		p_text++;
	}

	/*	Check for name validity.
	 *	------------------------ */	
	if ((text_len <= 0) || (text_len > (sizeof (buf) - 1)))
	{
		msg_error_s("Bad 16-bit register!", EC_B16BR, text);
		return (text);
	}

	/*	- Make a uppercase working copy of string.
	 *	- Notes: Not necessary to check for buffer overflow, since
	 *	  string length check have already been done.
	 *	---------------------------------------------------------- */	
	while (	(*text != '\0') && (*text != ',') &&
		  		(isspace((int) *text)) == 0)
	{
		buf_ori[i]	= *text;
		buf[i]		= toupper((int) *text);
		i++;
		text++;
	}

	buf_ori[i]	= '\0';
	buf[i]		= '\0';

	if ((strcmp("BC", buf) == 0) || (*buf == 'B'))
  	{
		if ((ar & PR16_AR_BC) != 0)
			return (text);
		else
			reg_not_allowed	= 1;
	}

	if ((strcmp("DE", buf) == 0) || (*buf == 'D'))
  	{
		if ((ar & PR16_AR_DE) != 0)
		{
			b1	+= 0x10;
			return (text);
		}
		else
			reg_not_allowed	= 1;
	}

	if ((strcmp("HL", buf) == 0) || (*buf == 'H'))
  	{
		if ((ar & PR16_AR_HL) != 0)
		{
			b1	+= 0x20;
			return (text);
		}
		else
			reg_not_allowed	= 1;
	}

	if (strcmp("SP", buf) == 0)
  	{
		if ((ar & PR16_AR_SP) != 0)
		{
			b1	+= 0x30;
			return (text);
		}
		else
			reg_not_allowed	= 1;
	}

	if (strcmp("PSW", buf) == 0)
  	{
		if ((ar & PR16_AR_PSW) != 0)
		{
			b1	+= 0x30;
			return (text);
		}
		else
			reg_not_allowed	= 1;
	}

#if 0
	/*	Print/Display error on pass #2 only.
	 *	------------------------------------ */
	if (asm_pass == 1)
	{
		/*	If register not allowed...
		 *	-------------------------- */
		if (reg_not_allowed)
		{
			msg_error_s("Register not allowed!", EC_RNA, buf_ori);
		}
		/*	Bad register.
		 *	------------- */
		else
		{
			msg_error_s("Bad 16-bit register!", EC_B16BR, buf_ori);
		}
	}
#endif

	/*	- Print error message if register is not allowed
	 *	  or bad.
	 *	------------------------------------------------ */
	if (reg_not_allowed)
		msg_error_s("Register not allowed!", EC_RNA, buf_ori);
	else
		msg_error_s("Bad 16-bit register!", EC_B16BR, buf_ori);

	return (text);
}


/*	*************************************************************************
 *	Function name:	DestReg
 *	Description:	Parse Destination Register.
 *	Author(s):		Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2011
 *
 *	Parameters:		char *text:
 *							...
 *
 *	Returns:			char *:
 *							...
 *
 *	Globals:			None.
 *	Notes:
 *	************************************************************************* */

static char *DestReg(char *text)
{
	int	c	= toupper((int) *text);

	switch (c)
  	{
		case 'A':
			b1 += 0x7 << 3;
			break;

		case 'B':
			b1 += 0x0 << 3;
			break;

		case 'C':
			b1 += 0x1 << 3;
			break;

		case 'D':
			b1 += 0x2 << 3;
			break;

		case 'E':
			b1 += 0x3 << 3;
			break;

		case 'H':
			b1 += 0x4 << 3;
			break;

		case 'L':
			b1 += 0x5 << 3;
			break;

		case 'M':
			b1 += 0x6 << 3;
			break;

		default:
			msg_error_c("Bad destination register!", EC_BDR, *text);
			break;
	}

	text++;

	return (text);
}


/*	*************************************************************************
 *	Function name:	SourceReg
 *	Description:	Parse Source Register.
 *	Author(s):		Claude Sylvain
 *	Created:			2007
 *	Last modified:	27 December 2011
 *
 *	Parameters:		char *text:
 *							...
 *
 *	Returns:			char *:
 *							...
 *
 *	Globals:			None.
 *	Notes:
 *	************************************************************************* */

static char *SourceReg(char *text)
{
	int	c;

	/*	Bypass space if necessary.
	 *	-------------------------- */
	while (isspace((int) *text))
		text++;

	c	= toupper((int) *text);

	switch (c)
  	{
		case 'A':
			b1 += 0x7;
			break;

		case 'B':
			b1 += 0x0;
			break;

		case 'C':
			b1 += 0x1;
			break;

		case 'D':
			b1 += 0x2;
			break;

		case 'E':
			b1 += 0x3;
			break;

		case 'H':
			b1 += 0x4;
			break;

		case 'L':
			b1 += 0x5;
			break;

		case 'M':
			b1 += 0x6;
			break;

		default:
			msg_error_c("Bad source register!", EC_BSR, *text);
			break;
	}

	text++;

	return (text);
}


/*	*************************************************************************
 *	Function name:	proc_nop
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int proc_nop(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	data_size	= 1;
	b1				= 0;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_in
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int proc_in(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xDB;

	tmp		= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_out
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int proc_out(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xD3;

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rst
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

int proc_rst(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC7;

	tmp	= exp_parser(equation);

	check_oor(tmp, 0x07);		/*	Check Operand Over Range. */

	tmp			= tmp & 0x7;
	b1				|= tmp << 3;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_mov
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_mov(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1 = 0x040;

	/* proc dest reg */
	equation = DestReg(equation);

	/* proc source reg */
	equation = AdvancePastSpace(AdvancePast(equation, ','));
	equation = SourceReg(equation);

	data_size = 1;
	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_mvi
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_mvi(char *label, char *equation)
{
	int		tmp;

	process_label(label);				/*	Process Label. */

	b1			= 0x06;

	/* Process destination register.
	 *	Notes: This can modify "b1".
	 *	*/
	equation = DestReg(equation);

	equation = AdvancePast(equation, ',');
	tmp		= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_lxi
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	24 December 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_lxi(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0x01;

	/* Process destination register pair.
	 *	Notes: This can modify "b1".
	 *	*/
	equation = parse_reg16bits(
		equation, PR16_AR_BC | PR16_AR_DE | PR16_AR_HL | PR16_AR_SP);

	equation = AdvancePast(equation, ',');
	tmp		= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_lda
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_lda(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0x3A;

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_sta
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_sta(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0x32;

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_lhld
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_lhld(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0x2A;

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_shld
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_shld(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0x22;

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_ldax
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	15 January 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_ldax(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1	= 0x0A;

	/* Process destination pair.
	 * */
	parse_reg16bits(equation, PR16_AR_BC | PR16_AR_DE);

	data_size	= 1;		/*	Now the value. */

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_stax
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	15 January 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_stax(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1	= 0x02;

	/* Process destination pair.
	 * */
	parse_reg16bits(equation, PR16_AR_BC | PR16_AR_DE);

	data_size = 1;		/* Now the value. */

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_xchg
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_xchg(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xEB;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_add
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_add(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x80;
	SourceReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_adi
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_adi(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b1				= 0xC6;
	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_adc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_adc(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x88;
	SourceReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_aci
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_aci(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b1 			= 0xCE;
	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_sub
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_sub(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x90;
	SourceReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_sui
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_sui(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b1				= 0xD6;
	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_sbb
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_sbb(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x98;
	SourceReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_sbi
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_sbi(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b1				= 0xDE;
	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_inr
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_inr(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x04;
	DestReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_dcr
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_dcr(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x05;
	DestReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_inx
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	15 January 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_inx(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1	= 0x03;

	/* Process destination pair.
	 * */
	parse_reg16bits(equation, PR16_AR_BC | PR16_AR_DE | PR16_AR_HL | PR16_AR_SP);

	data_size	= 1;			/* Now the value. */

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_dcx
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	15 January 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_dcx(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1	= 0x0B;

	/* Process destination pair.
	 * */
	parse_reg16bits(equation, PR16_AR_BC | PR16_AR_DE | PR16_AR_HL | PR16_AR_SP);

	data_size	= 1;			/* Now the value. */

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_dad
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	15 January 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_dad(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1	= 0x09;

	/* Process destination pair.
	 * */
	parse_reg16bits(equation, PR16_AR_BC | PR16_AR_DE | PR16_AR_HL | PR16_AR_SP);

	data_size	= 1;			/* Now the value. */

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_daa
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_daa(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x27;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_ana
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_ana(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xA0;
	SourceReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_ani
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_ani(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b1				= 0xE6;
	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_ora
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_ora(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xB0;
	SourceReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_ori
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_ori(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b1				= 0xF6;
	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_xra
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_xra(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xA8;
	SourceReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_xri
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_xri(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b1				= 0xEE;
	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cmp
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cmp(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xB8;
	SourceReg(equation);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cpi
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cpi(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFF);		/*	Check Operand Over Range. */

	b1				= 0xFE;
	b2				= tmp & 0xFF;
	data_size	= 2;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rlc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rlc(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x07;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rrc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rrc(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x0F;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_ral
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_ral(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x17;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rar
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rar(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x1F;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cma
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cma(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x2F;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cmc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cmc(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x3F;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_stc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_stc(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x37;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_jmp
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_jmp(char *label, char *equation)
{
	int	tmp;

	process_label(label);		/*	Process Label. */

	b1				= 0xC3;

	tmp = exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_jnz
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_jnz(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC2;

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_jz
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_jz(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC2 + (1 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_jnc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_jnc(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC2 + (2 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_jc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_jc(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC2 + (3 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_jpo
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_jpo(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC2 + (4 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_jpe
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_jpe(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC2 + (5 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_jp
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_jp(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC2 + (6 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_jm
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_jm(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC2 + (7 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_call
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_call(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xCD;

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cnz
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cnz(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC4;

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cz
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cz(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC4 + (1 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cnc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cnc(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC4 + (2 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cc(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC4 + (3 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cpo
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cpo(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC4 + (4 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cpe
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cpe(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC4 + (5 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cp
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cp(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC4 + (6 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_cm
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	26 November 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_cm(char *label, char *equation)
{
	int		tmp;

	process_label(label);		/*	Process Label. */

	b1 = 0xC4 + (7 << 3);

	tmp	= exp_parser(equation);

	check_oor(tmp, 0xFFFF);		/*	Check Operand Over Range. */

	b2				= tmp & 0xFF;
	b3				= (tmp >> 8) & 0xFF;
	data_size	= 3;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_ret
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_ret(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC9;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rnz
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rnz(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC0;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rz
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rz(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC0 + (1 << 3);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rnc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rnc(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC0 + (2 << 3);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rc
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rc(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC0 + (3 << 3);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rpo
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rpo(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC0 + (4 << 3);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rpe
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rpe(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC0 + (5 << 3);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rp
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rp(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC0 + (6 << 3);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_rm
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_rm(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC0 + (7 << 3);
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_pchl
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_pchl(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xE9;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_push
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	15 January 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_push(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC5;
	data_size	= 1;

	parse_reg16bits(
		equation, PR16_AR_BC | PR16_AR_DE | PR16_AR_HL | PR16_AR_PSW);

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_pop
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	15 January 2011
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_pop(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xC1;
	data_size	= 1;

	parse_reg16bits(
		equation, PR16_AR_BC | PR16_AR_DE | PR16_AR_HL | PR16_AR_PSW);

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_xthl
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_xthl(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xE3;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_sphl
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_sphl(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xF9;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_ei
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_ei(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xFB;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_di
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_di(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0xF3;
	data_size	= 1;

	return (TEXT);
}


/*	*************************************************************************
 *	Function name:	proc_hlt
 *	Description:
 *	Author(s):		Jay Cotton, Claude Sylvain
 *	Created:			2007
 *	Last modified:	28 December 2010
 *
 *	Parameters:		char *label:
 *							...
 *
 *						char *equation:
 *							...
 *
 *	Returns:			int:
 *							...
 *
 *	Globals:
 *	Notes:
 *	************************************************************************* */

static int proc_hlt(char *label, char *equation)
{
	process_label(label);		/*	Process Label. */

	b1				= 0x76;
	data_size	= 1;

	return (TEXT);
}



