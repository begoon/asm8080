# *****************************************************************************
# Filename:			Makefile.am
# Description:
# Copyright(c):	See below.
# Author(s):		Jay Cotton, Claude Sylvain
# Created:			26 Mars 2011
# Last modified:	27 December 2011
# Notes:
# *****************************************************************************

# *****************************************************************************
# Copyright (c) <2007-2011> <jay.cotton@oracle.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
# *****************************************************************************


#AUTOMAKE_OPTOINS = foreign

# Set C Flags.
# Include the following settings:
#
# Set the Machine type and Operating System type.
#
# 	Possible values for Machine type are:
# 			_TGT_M_X86		32-bit Machine
# 			_TGT_M_X64		64-bit Machine
#
#	 Possible values for Operating System are:
# 			_TGT_OS_SOLARIS32		Solaris 32-bit
# 			_TGT_OS_SOLARIS64		Solaris 64-bit
# 			_TGT_OS_LINUX32		GNU/Linux 32-bit
# 			_TGT_OS_LINUX64		GNU/Linux 64-bit
# 			_TGT_OS_CYGWIN32		Cygwin 32-bit
# 			_TGT_OS_CYGWIN64		Cygwin 64-bit (do not still exist)
# 			_TGT_OS_WIN32			Windows 32-bit
# 			_TGT_OS_WIN64			Windows 64-bit
#
#AM_CFLAGS = -O0 -g -D_TGT_M_X86 -D_TGT_OS_CYGWIN32
AM_CFLAGS = -D_TGT_M_X86 -D_TGT_OS_CYGWIN32

bin_PROGRAMS = asm8080

asm8080_SOURCES = asm_dir.c exp_parser.c msg.c opcode.c util.c main.c 
#asm8080_CFLAGS = -g -O0
#asm8080_LDFLAGS = -g
#asm8080_CPPFLAGS = -g -O0






