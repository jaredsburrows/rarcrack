RarCrack!

This program licensed under GPL 2.

If you forget your password for compressed archive (rar, 7z, zip), this program is the solution.
This program uses bruteforce algorithm to find correct password. You can specify wich characters will be used in password generations.
Warning: Please don't use this program for any illegal things!

Changelog:
--------------------------------------------------
Version 0.2 [this is the latest version]
    * Added multiple cracking threads support
    * Added 7z, zip support
    * Archive file type auto-detection

Version 0.1
    * Inital version (rar cracking working)
--------------------------------------------------

Software requirements:
    * > glibc 2.4
    * any POSIX compatible operating system [sorry Window$ isn't]
    * pthreads
    * libxml2
    * and finally: 7zip, unrar, unzip

Building and installing:

Everything is very easy:
---------------------------------------------------
tar -xjf rarcrack-VERSION.tar.bz2
cd rarcrack-VERSION
#you need gcc or any C compiler (edit Makefile CC=YOUR_C_COMPILER)
make
#you must be root in next step:
make install
---------------------------------------------------


Using RarCrack:
---------------------------------------------------
rarcrack your_encrypted_archive.ext [--threads thread_num] [--type rar|zip|7z]
---------------------------------------------------
Everything in [] are optional, rarcrack default crack two threads and autodetect the archive type. If the detection wrong you can specify the correct file type with the type parameter. RarCrack currently crack maximum in 12 threads.

After the cracking started RarCrack will print the current status of cracking and save it's to a status file. If you want more specific password character set, you need to run RarCrack to create the XML status file (3 sec).
---------------------------------------------------
<?xml version="1.0" encoding="UTF-8"?>
<rarcrack>
 <abc>0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ</abc>
 <current>uU</current>
 <good_password></good_password>
</rarcrack>
---------------------------------------------------
This is a sample XML file, and you see there is a character set. If you want, you can modify this file and when you start RarCrack again the program will be use new variables.
Warning: Take care when you changing this file, make sure the current password don't have characters outside the abc[character set]!

