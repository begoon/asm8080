/*	*************************************************************************
 *	Module Name:	war_code.h
 *	Description:	Assembler Warning Codes.
 *	Copyright(c):	See below...
 *	Author(s):		Claude Sylvain
 *	Created:			23 December 2010
 *	Last modified:	17 May 2013
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

#ifndef _WAR_CODE_H
#define _WAR_CODE_H


/*	*************************************************************************
 *											  CONSTANTS
 *	************************************************************************* */

#define WC_MQ				0		/* Missing Quote. */
#define WC_EDFIIF			1		/* 'END' Directive Found Inside an Include File. */
#define WC_ELOS			2		/* Extra "-l" Option Specified. */
#define WC_EOOS			3		/* Extra "-o" Option Specified. */
#define WC_NIFSATLOP		4		/* No Input File Specified At Time "-l" Option was Processed. */
#define WC_NIFSATOOP		5		/* No Input File Specified At Time "-o" Option was Processed. */
#define WC_OFNHE			6		/*	Output File Name Have an Extension. */
#define WC_LNBFC			7		/*	Label/Name, Bad First Character. */
#define WC_SCNS			8		/*	Special Command Not Supported. */
#define WC_LTL				9		/* Label Too Long. */
#define WC_ILNC			10		/*	Invalid Label/Name Character. */
#define WC_IFNHNE			11		/*	Input File Name Have No Extension. */
#define WC_SAUAL			12		/*	Symbol Already Used As Label. */
#define WC_MPNS			13		/*	Macro Parameters Not Supported. */
#define WC_IDERINB		14		/*	"IF" Directive Expression Result Is Not Boolean. */



#endif





