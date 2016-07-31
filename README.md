# Rarcrack [![Build Status](https://travis-ci.org/jaredsburrows/rarcrack.svg?branch=master)](https://travis-ci.org/jaredsburrows/rarcrack)

Rarcrack - port for Mac OSX (works on Linux and Mac OSX)

 - Fixed warnings + errors
   - Linking errors
   - Segfaults

Originally source code by Copyright (C) 2007 David Zoltan Kedves - kedazo@gmail.com

Download source from Sourceforge.net here: http://sourceforge.net/projects/rarcrack/files/rarcrack-0.2/


## Compile

    make

## Usage

	$ ./rarcrack 
	RarCrack! 0.2 by David Zoltan Kedves (kedazo@gmail.com)

	USAGE: rarcrack encrypted_archive.ext [--threads NUM] [--type rar|zip|7z]
	       For more information please run "rarcrack --help"

## Example

 **Rar**

	$ ./rarcrack --threads 2 --type rar test.rar 
	RarCrack! 0.2 by David Zoltan Kedves (kedazo@gmail.com)
	
	INFO: the specified archive type: rar
	INFO: cracking test.rar, status file: test.rar.xml
	Probing: '5h' [129 pwds/sec]
	Probing: 'bm' [125 pwds/sec]
	Probing: 'hu' [126 pwds/sec]
	Probing: 'nA' [126 pwds/sec]
	Probing: 'tJ' [127 pwds/sec]
	Probing: 'zT' [127 pwds/sec]
	Probing: 'FV' [124 pwds/sec]
	Probing: 'LV' [124 pwds/sec]
	Probing: 'RN' [121 pwds/sec]
	Probing: 'XS' [125 pwds/sec]
	Probing: '03I' [120 pwds/sec]
	Probing: '09u' [119 pwds/sec]
	Probing: '0fe' [118 pwds/sec]
	Probing: '0l4' [120 pwds/sec]
	Probing: '0qM' [118 pwds/sec]
	Probing: '0wq' [116 pwds/sec]
	Probing: '0Ci' [121 pwds/sec]
	Probing: '0I4' [119 pwds/sec]
	Probing: '0NG' [116 pwds/sec]
	Probing: '0Tx' [121 pwds/sec]
	Probing: '0Zj' [119 pwds/sec]
	GOOD: password cracked: '100'

 **Zip**

	$ ./rarcrack --threads 2 --type zip test.zip
	RarCrack! 0.2 by David Zoltan Kedves (kedazo@gmail.com)
	
	INFO: the specified archive type: zip
	INFO: cracking test.zip, status file: test.zip.xml
	Probing: 'An' [771 pwds/sec]
	Probing: '0bC' [769 pwds/sec]
	Probing: '0N6' [774 pwds/sec]
	GOOD: password cracked: '100'
	
 **7z**
 
	$ ./rarcrack --threads 2 --type 7z test.7z 
	RarCrack! 0.2 by David Zoltan Kedves (kedazo@gmail.com)
	
	INFO: the specified archive type: 7z
	INFO: cracking test.7z, status file: test.7z.xml
	Probing: '33' [83 pwds/sec]
	Probing: '6T' [79 pwds/sec]
	Probing: 'aP' [81 pwds/sec]
	Probing: 'eL' [81 pwds/sec]
	Probing: 'iE' [80 pwds/sec]
	Probing: 'mu' [79 pwds/sec]
	Probing: 'qk' [79 pwds/sec]
	Probing: 'ua' [79 pwds/sec]
	Probing: 'y0' [79 pwds/sec]
	Probing: 'BT' [80 pwds/sec]
	Probing: 'FH' [78 pwds/sec]
	Probing: 'Jr' [77 pwds/sec]
	Probing: 'N3' [74 pwds/sec]
	Probing: 'QR' [78 pwds/sec]
	Probing: 'UG' [79 pwds/sec]
	Probing: 'Ys' [78 pwds/sec]
	Probing: '024' [74 pwds/sec]
	Probing: '05i' [66 pwds/sec]
	Probing: '08L' [71 pwds/sec]
	Probing: '0ce' [71 pwds/sec]
	Probing: '0fz' [69 pwds/sec]
	Probing: '0j4' [72 pwds/sec]
	Probing: '0mw' [71 pwds/sec]
	Probing: '0q0' [72 pwds/sec]
	Probing: '0ts' [71 pwds/sec]
	Probing: '0wM' [68 pwds/sec]
	Probing: '0A8' [69 pwds/sec]
	Probing: '0DC' [72 pwds/sec]
	Probing: '0H7' [72 pwds/sec]
	Probing: '0KB' [72 pwds/sec]
	Probing: '0O6' [72 pwds/sec]
	Probing: '0RD' [73 pwds/sec]
	Probing: '0V9' [72 pwds/sec]
	Probing: '0YD' [72 pwds/sec]
	GOOD: password cracked: '100'
