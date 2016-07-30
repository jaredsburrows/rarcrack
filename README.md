Rarcrack [![Build Status](https://travis-ci.org/jaredsburrows/rarcrack.svg?branch=master)](https://travis-ci.org/jaredsburrows/rarcrack)
========

Rarcrack - port for Mac OSX (works on Linux and Mac OSX)

 - Fixed warnings + errors
   - Linking errors
   - Segfaults

Originally source code by Copyright (C) 2007 David Zoltan Kedves - kedazo@gmail.com

Download source from Sourceforge.net here: http://sourceforge.net/projects/rarcrack/files/rarcrack-0.2/


####Compile

    make

####Usage

	$ ./rarcrack 
	RarCrack! 0.2 by David Zoltan Kedves (kedazo@gmail.com)

	USAGE: rarcrack encrypted_archive.ext [--threads NUM] [--type rar|zip|7z]
	       For more information please run "rarcrack --help"

####Example

   ./rarcrack --threads 6 --type zip test.zip
