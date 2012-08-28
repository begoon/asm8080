#!/bin/sh
# *****************************************************************************
# Filename:			autogen.sh
# Description:
# Copyright(c):
# Author(s):		Jay Cotton, Claude Sylvain
# Created:			26 Mars 2011
# Last modified:	17 April 2011
# *****************************************************************************


echo "Generating configure files... may take a while."


aclocal
libtoolize --automake
automake -a
autoconf


