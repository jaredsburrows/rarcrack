/*
 *   Copyright (C) 2007 by David Zoltan Kedves
 *   kedazo@gmail.com
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "rarcrack.h"

char* ABC = (char*) &default_ABC;
int ABCLEN;

char password[PWD_LEN+1] = {'\0','\0'}; //this contains the actual password
char password_good[PWD_LEN+1] = {'\0', '\0'};  //this changed only once, when we found the good passord
unsigned int curr_len = 1; //current password length
long counter = 0;	//this couning probed passwords
xmlMutexPtr pwdMutex;	//mutex for password char array
char filename[255];	//the archive file name
char statname[259];	//status xml file name filename + ".xml"
xmlDocPtr status;
int finished = 0;
xmlMutexPtr finishedMutex;
char finalcmd[300] = {'\0', '\0'}; //this depending on arhive file type, it's a command to test file with password

char * getfirstpassword() {
	static char ret[2];
	ret[0] = ABC[0];
	ret[1] = '\0';
	return (char*) &ret;
}

inline void savestatus() {
	xmlNodePtr root = NULL;
	xmlNodePtr node = NULL;
	xmlChar* tmp = NULL;
	if ((strlen(statname) > 0) && (status != NULL)) {
		root = xmlDocGetRootElement(status);
		if (root) {
			xmlMutexLock(finishedMutex);
			for (node = root->children; node; node = node->next) {
				if (xmlStrcmp(node->name, "current") == 0) {
					xmlMutexLock(pwdMutex);
					tmp = xmlEncodeEntitiesReentrant(status, (const xmlChar*) &password);
					xmlMutexUnlock(pwdMutex);
					if (node->children) {
					    if (password[0] == '\0')
						xmlNodeSetContent(node->children, getfirstpassword());
					    else
						xmlNodeSetContent(node->children, tmp);
					    }
					xmlFree(tmp);
				} else if ((finished == 1) && (xmlStrcmp(node->name,"good_password") == 0)) {
					tmp =  xmlEncodeEntitiesReentrant(status, (const xmlChar*) &password_good);
					if (node->children)
						xmlNodeSetContent(node->children, tmp);
					xmlFree(tmp);
				}
			}
			xmlMutexUnlock(finishedMutex);
		}
		xmlSaveFormatFileEnc(statname, status, "UTF-8", 1);
	}
	return;
}

inline int abcnumb(char a) {
  int i;
  for (i = 0; i<ABCLEN; i++) 
    if (ABC[i] == a) return i;
  return 0;
}

int loadstatus() {
	xmlNodePtr root = NULL;
	xmlNodePtr node = NULL;
	xmlParserCtxtPtr parserctxt;
	int ret = 0;
	char* tmp;
	FILE* totest;
	totest = fopen(statname, "r");
	if (totest != NULL) {
		fclose(totest);
		status = xmlParseFile(statname);
	}
	if (status != NULL)
		root = xmlDocGetRootElement(status);
	else
		status = xmlNewDoc(NULL);
	if (root != NULL) {
		parserctxt = xmlNewParserCtxt();
		for (node = root->children; node; node = node->next) {
			if (xmlStrcmp(node->name, "abc") == 0) {
				if (node->children && (strlen(node->children->content) > 0)) {
					ABC = xmlStringDecodeEntities(parserctxt, 
						node->children->content,XML_SUBSTITUTE_BOTH,0,0,0);
				} else
					ret = 1;
			} else if (xmlStrcmp(node->name, "current") == 0) {
				if (node->children && (strlen(node->children->content) > 0)) {
					tmp = xmlStringDecodeEntities(parserctxt, 
						node->children->content,XML_SUBSTITUTE_BOTH,0,0,0);
					strcpy(password,tmp);
					curr_len = strlen(password);
					printf("INFO: Resuming cracking from password: '%s'\n",password);
					xmlFree(tmp);
				} else
					ret = 1;
			} else if (xmlStrcmp(node->name, "good_password") == 0) {
				if (node->children && (strlen(node->children->content) > 0)) {
					tmp = xmlStringDecodeEntities(parserctxt, 
						node->children->content,XML_SUBSTITUTE_BOTH,0,0,0);
					strcpy(password,tmp);
					curr_len = strlen(password);
					xmlMutexLock(finishedMutex);
					finished = 1;
					xmlMutexUnlock(finishedMutex);
					strcpy((char*) &password_good, (char*) &password);
					printf("GOOD: This archive was succesfully cracked\n");
					printf("      The good password is: '%s'\n", password);
					xmlFree(tmp);
					ret = 1;
				}
			}
		}
		xmlFreeParserCtxt(parserctxt);
	} else {
		root = xmlNewNode(NULL, "rarcrack");
		xmlDocSetRootElement(status, root);
		node = xmlNewTextChild(root, NULL, "abc", ABC);
		node = xmlNewTextChild(root, NULL, "current", getfirstpassword()); 
		node = xmlNewTextChild(root, NULL, "good_password", "");
		savestatus();
	}
	return ret;
}

void nextpass2(char* p, unsigned int n) {
   int i;
   if (p[n] == ABC[ABCLEN-1]) {
	   p[n] = ABC[0];
	   if (n>0)
	   	nextpass2(p, n-1);
	   else {
		for (i=curr_len; i>=0; i--)
			p[i+1]=p[i];
		p[0]=ABC[0];
		p[++curr_len]='\0';
	   }
	  	
   } else 
	   p[n] = ABC[abcnumb(p[n])+1];
   return;
}

inline char* nextpass() {	//IMPORTANT: the returned string must be freed
   char *ok = malloc(sizeof(char)*(PWD_LEN+1));
   xmlMutexLock(pwdMutex);
   strcpy(ok, password);
   nextpass2((char*) &password, curr_len - 1);
   xmlMutexUnlock(pwdMutex);
   return ok;
}

void * status_thread() {
	int pwds;
	const short status_sleep = 3;
	while(1) {
		sleep(status_sleep);
		xmlMutexLock(finishedMutex);
		pwds = counter / status_sleep;
		counter = 0;
		if (finished != 0)
			break;
		xmlMutexUnlock(finishedMutex);
		xmlMutexLock(pwdMutex);
		printf("Probing: '%s' [%d pwds/sec]\n", password, pwds);
		xmlMutexUnlock(pwdMutex);
		savestatus();	//FIXME: this is wrong, when probing current password(s) is(are) not finished yet, and the program is exiting
	}
}

void * crack_thread() {
	char * current;
	char ret[200];
	char cmd[400];
	FILE * Pipe;
	while (1) {
		current = nextpass();
		sprintf((char*)&cmd, finalcmd, current, filename);
		Pipe = popen(cmd, "r");
		while (!feof(Pipe)) {
		   fgets((char*)&ret, 200, Pipe);
		   if (strcasestr(ret, "ok") != NULL) {
			strcpy(password_good, current);
			xmlMutexLock(finishedMutex);
			finished = 1;
			printf("GOOD: password cracked: '%s'\n", current);
			xmlMutexUnlock(finishedMutex);
			savestatus();
			break;
		   }
		}
		pclose(Pipe);
		xmlMutexLock(finishedMutex);
		counter++;
		if (finished != 0) {
			xmlMutexUnlock(finishedMutex);
			break;
		}
		xmlMutexUnlock(finishedMutex);
		free(current);
	}
}

void crack_start(unsigned int threads) {
	pthread_t th[13];
	unsigned int i;
	for (i = 0; i < threads; i++) {
		(void) pthread_create(&th[i], NULL, crack_thread, NULL);
	}
	(void) pthread_create(&th[12], NULL, status_thread, NULL);
	for (i = 0; i < threads; i++) {
		(void) pthread_join(th[i], NULL);
	}
	(void) pthread_join(th[12], NULL);
	return;
}

void init(int argc, char **argv) {
	unsigned int i, j;
	int help = 0;
	int threads = 2;
	int archive_type = -1;
	FILE* totest;
	char test[300];
	xmlInitThreads();
	pwdMutex = xmlNewMutex();
	finishedMutex = xmlNewMutex();
	if (argc == 1) {
		printf("USAGE: rarcrack encrypted_archive.ext [--threads NUM] [--type rar|zip|7z]\n");
		printf("       For more information please run \"rarcrack --help\"\n");
		help = 1;
	} else {
		for (i = 1; i < argc; i++) {
			if (strcmp(argv[i],"--help") == 0) {
				printf("Usage:   rarcrack encrypted_archive.ext [--threads NUM] [--type rar|zip|7z]\n\n");
				printf("Options: --help: show this screen.\n");
				printf("         --type: you can specify the archive program, this needed when\n");
				printf("                 the program couldn't detect the proper file type\n");
				printf("         --threads: you can specify how many threads\n");
				printf("                    will be run, maximum 12 (default: 2)\n\n");
				printf("Info:    This program supports only RAR, ZIP and 7Z encrypted archives.\n");
				printf("         RarCrack! usually detects the archive type.\n\n");
				help = 1;
				break;	
			} else if (strcmp(argv[i],"--threads") == 0) {
				if ((i + 1) < argc) {
					sscanf(argv[++i], "%d", &threads);
					if (threads < 1) threads = 1;
					if (threads > 12) {
						printf("INFO: number of threads adjusted to 12\n");
						threads = 12;
					}
				} else {
					printf("ERROR: missing parameter for option: --threads!\n");
					help = 1;
				}
			} else if (strcmp(argv[i],"--type") == 0) {
				if ((i + 1) < argc) {
					sscanf(argv[++i], "%s", &test);
					for (j = 0; strcmp(TYPE[j], "") != 0; j++) {
						if (strcmp(TYPE[j], test) == 0) {
							strcpy(finalcmd, CMD[j]);
							archive_type = j;
							break;
						}
					}
					if (archive_type < 0) {
						printf("WARNING: invalid parameter --type %s!\n", argv[i]);
						finalcmd[0] = '\0';
					}
				} else {
					printf("ERROR: missing parameter for option: --type!\n");
					help = 1;
				}
			} else {
				strcpy((char*)&filename, argv[i]);
			}
		}
	}
	if (help == 1)
		return;
	sprintf((char*)&statname,"%s.xml",(char*)&filename);
	totest = fopen(filename,"r");
	if (totest == NULL) {
		printf("ERROR: The specified file (%s) is not exists or \n", filename);
		printf("       you don't have a right permissions!\n");
		return;
	} else
		fclose(totest);
	if (finalcmd[0] == '\0') { //when we specify the file type, the programm will skip the test
		sprintf((char*)&test, CMD_DETECT, filename);
		totest = popen(test,"r");
		fscanf(totest,"%s",(char*)&test);
		pclose(totest);
		for (i = 0; strcmp(MIME[i],"") != 0; i++) {
			if (strcmp(MIME[i],test) == 0) {
				strcpy(finalcmd,CMD[i]);
				archive_type = i;
				break;
			}
		}
		printf("INFO: detected file type: %s\n", TYPE[archive_type]);
	} else
		printf("INFO: the specified archive type: %s\n", TYPE[archive_type]);
	if (finalcmd[0] == '\0') {
		printf("ERROR: Couldn't detect archive type\n");
		return;
	} /*else
		printf("DEBUG: the unpack command is: '%s'\n", finalcmd);*/
	printf("INFO: cracking %s, status file: %s\n", filename, statname);
	if (loadstatus() == 1) {
		printf("ERROR: The status file (%s) is corrupted!\n", statname);
		return;
	}
	ABCLEN = strlen(ABC);
	if (password[0] == '\0')
		password[0] = ABC[0];
	crack_start(threads);
	return;
}

int main(int argc, char *argv[]) {
  printf("RarCrack! 0.2 by David Zoltan Kedves (kedazo@gmail.com)\n\n");
  init(argc,argv);
  if (ABC != (char*) &default_ABC)
	  xmlFree(ABC);
  if (status)
	  xmlFreeDoc(status);
  xmlFreeMutex(pwdMutex); 
  xmlFreeMutex(finishedMutex);
  return EXIT_SUCCESS;
}

