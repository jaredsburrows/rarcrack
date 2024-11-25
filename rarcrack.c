// Standard headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

// libxml2 headers
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/threads.h>

// Default char list
static const char default_ABC[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

// File extensions
// "" is the end of the list
static const char *const TYPE[] = { "rar", "7z", "zip", "" };

// File Types
static const char *const MIME[] = { "application/x-rar;", "application/x-7z-compressed;", "application/zip;", "" };

// Max password length
#define PWD_LEN 100

typedef void (*CMD_exec_t)(const char password[PWD_LEN + 1], const char *filename);
static void CMD_exec_unrar_7z(const char *prog, const char password[PWD_LEN + 1], const char *filename) {
    char parg[2 + PWD_LEN + 1];
    strcpy(parg, "-p");
    strcpy(parg + 2, password);
    execlp(prog, prog, "t", "-y", parg, filename, (char *)NULL);
}
static void CMD_exec_unrar(const char password[PWD_LEN + 1], const char *filename) {
    CMD_exec_unrar_7z("unrar", password, filename);
}
static void CMD_exec_7z(const char password[PWD_LEN + 1], const char *filename) {
    CMD_exec_unrar_7z("7z", password, filename);
}
static void CMD_exec_unzip(const char password[PWD_LEN + 1], const char *filename) {
    execlp("unzip", "unzip", "-P", password, "-t", filename, (char *)NULL);
}

// Commnds for each file type
static const CMD_exec_t CMD[] = { CMD_exec_unrar, CMD_exec_7z, CMD_exec_unzip, NULL };

char *getfirstpassword();
void crack_start(unsigned int threads);

const char* ABC = default_ABC;
int ABCLEN;

char password[PWD_LEN+1] = {'\0','\0'}; //this contains the actual password
char password_good[PWD_LEN+1] = {'\0', '\0'};  //this changed only once, when we found the good passord
unsigned int curr_len = 1; //current password length
long counter = 0;	//this couning probed passwords
xmlMutexPtr pwdMutex;	//mutex for password char array
const char *filename;	//the archive file name
char *statname;	//status xml file name filename + ".xml"
xmlDocPtr status;
int finished = 0;
xmlMutexPtr finishedMutex;
CMD_exec_t finalcmd; //this depending on arhive file type, it's a command to test file with password

char *getfirstpassword() {
    static char ret[2];
    ret[0] = ABC[0];
    ret[1] = '\0';
    return (char*) &ret;
}

void savestatus() {
    xmlNodePtr root = NULL;
    xmlNodePtr node = NULL;
    xmlChar* tmp = NULL;
    if ((strlen(statname) > 0) && status) {
        root = xmlDocGetRootElement(status);
        if (root) {
            xmlMutexLock(finishedMutex);
            for (node = root->children; node; node = node->next) {
                if (xmlStrcmp(node->name, (const xmlChar*)"current") == 0) {
                    xmlMutexLock(pwdMutex);
                    tmp = xmlEncodeEntitiesReentrant(status, (const xmlChar*) &password);
                    xmlMutexUnlock(pwdMutex);

                    if (node->children) {
                        if (password[0] == '\0') {
                            xmlNodeSetContent(node->children, (const xmlChar*)getfirstpassword());
                        } else {
                            xmlNodeSetContent(node->children, tmp);
                        }
                    }

                    xmlFree(tmp);
                } else if ((finished == 1) && (xmlStrcmp(node->name, (const xmlChar*)"good_password") == 0)) {
                    tmp =  xmlEncodeEntitiesReentrant(status, (const xmlChar*) &password_good);

                    if (node->children) {
                        xmlNodeSetContent(node->children, tmp);
                    }

                    xmlFree(tmp);
                }
            }
            xmlMutexUnlock(finishedMutex);
        }
        xmlSaveFormatFileEnc(statname, status, "UTF-8", 1);
    }
}

int abcnumb(char a) {
    int i = 0;
    for (i = 0; i < ABCLEN; i++) {
        if (ABC[i] == a) {
            return i;
        }
    }

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
    if (totest) {
        fclose(totest);
        status = xmlParseFile(statname);
    }

    if (status) {
        root = xmlDocGetRootElement(status);
    } else {
        status = xmlNewDoc(NULL);
    }

    if (root) {
        parserctxt = xmlNewParserCtxt();
        for (node = root->children; node; node = node->next) {
            if (xmlStrcmp(node->name, (const xmlChar*)"abc") == 0) {
                if (node->children && (strlen((const char*)node->children->content) > 0)) {
                    ABC = (char *)xmlStringDecodeEntities(parserctxt, (const xmlChar*)node->children->content, XML_SUBSTITUTE_BOTH, 0, 0, 0);
                } else {
                    ret = 1;
                }
            } else if (xmlStrcmp(node->name, (const xmlChar*)"current") == 0) {
                if (node->children && (strlen((const char*)node->children->content) > 0)) {
                    tmp = (char *)xmlStringDecodeEntities(parserctxt, (const xmlChar*)node->children->content, XML_SUBSTITUTE_BOTH, 0, 0, 0);
                    strcpy(password,tmp);
                    curr_len = strlen(password);
                    printf("INFO: Resuming cracking from password: '%s'\n",password);
                    xmlFree(tmp);
                } else {
                    ret = 1;
                }
            } else if (xmlStrcmp(node->name, (const xmlChar*)"good_password") == 0) {
                if (node->children && (strlen((const char*)node->children->content) > 0)) {
                    tmp = (char *)xmlStringDecodeEntities(parserctxt, node->children->content, XML_SUBSTITUTE_BOTH,0,0,0);
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
        root = xmlNewNode(NULL, (const xmlChar*)"rarcrack");
        xmlDocSetRootElement(status, root);
        node = xmlNewTextChild(root, NULL, (const xmlChar*)"abc", (const xmlChar*)ABC);
        node = xmlNewTextChild(root, NULL, (const xmlChar*)"current", (const xmlChar*)getfirstpassword());
        node = xmlNewTextChild(root, NULL, (const xmlChar*)"good_password", (const xmlChar*)"");
        savestatus();
    }

    return ret;
}

void nextpass2(char *p, unsigned int n) {
    int i;
    if (p[n] == ABC[ABCLEN-1]) {
        p[n] = ABC[0];

        if (n > 0) {
            nextpass2(p, n-1);
        } else {
            for (i=curr_len; i>=0; i--) {
                p[i+1]=p[i];
            }

            p[0]=ABC[0];
            p[++curr_len]='\0';
        }
    } else {
        p[n] = ABC[abcnumb(p[n])+1];
    }
}

void nextpass(char ok[static PWD_LEN+1]) {
    //IMPORTANT: the returned string must be freed
    xmlMutexLock(pwdMutex);
    strcpy(ok, password);
    nextpass2((char*) &password, curr_len - 1);
    xmlMutexUnlock(pwdMutex);
}

void *crack_thread() {
    char current[PWD_LEN+1];
    char *ret = NULL;
    size_t retlen = 0;
    FILE *Pipe;
    int fds[2];
    while (1) {
        nextpass(current);
        (void) -pipe2(fds, O_CLOEXEC);
        if (!vfork()) {
            dup2(fds[1], 1);
            dup2(fds[1], 2);
            finalcmd(current, filename);
            _exit(127);
        }
        close(fds[1]);
        Pipe = fdopen(fds[0], "re");
        while (getline(&ret, &retlen, Pipe) != -1) {
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

        fclose(Pipe);

        xmlMutexLock(finishedMutex);
        counter++;

        if (finished != 0) {
            xmlMutexUnlock(finishedMutex);
            break;
        }

        xmlMutexUnlock(finishedMutex);
    }
    free(ret);
    return 0;
}

void crack_start(unsigned int threads) {
    pthread_t *th = calloc(threads, sizeof(*th));
    unsigned int i;

    signal(SIGCHLD, SIG_IGN);

    for (i = 0; i < threads; i++) {
        (void) pthread_create(&th[i], NULL, crack_thread, NULL);
    }

    const short status_sleep = 3;
    while(1) {
        sleep(status_sleep);
        xmlMutexLock(finishedMutex);
        int pwds = counter / status_sleep;
        counter = 0;

        if (finished != 0) {
            break;
        }

        xmlMutexUnlock(finishedMutex);
        xmlMutexLock(pwdMutex);
        printf("Probing: '%s' [%d pwds/sec]\n", password, pwds);
        xmlMutexUnlock(pwdMutex);
        savestatus();   //FIXME: this is wrong, when probing current password(s) is(are) not finished yet, and the program is exiting
    }

    for (i = 0; i < threads; i++) {
        (void) pthread_join(th[i], NULL);
    }
}

void init(int argc, char **argv) {
    int i, j;
    int help = 0;
    int threads = 1;
    int archive_type = -1;
    FILE* totest;
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
                printf("                    will be run (default: 1)\n\n");
                printf("Info:    This program supports only RAR, ZIP and 7Z encrypted archives.\n");
                printf("         RarCrack! usually detects the archive type.\n\n");
                help = 1;
                break;
            } else if (strcmp(argv[i],"--threads") == 0) {
                if ((i + 1) < argc) {
                    sscanf(argv[++i], "%d", &threads);
                    if (threads < 1) threads = 1;
                } else {
                    printf("ERROR: missing parameter for option: --threads!\n");
                    help = 1;
                }
            } else if (strcmp(argv[i],"--type") == 0) {
                if ((i + 1) < argc) {
                    const char * tp = argv[++i];
                    for (j = 0; strcmp(TYPE[j], "") != 0; j++) {
                        if (strcmp(TYPE[j], tp) == 0) {
                            finalcmd = CMD[j];
                            archive_type = j;
                            break;
                        }
                    }

                    if (archive_type < 0) {
                        printf("WARNING: invalid parameter --type %s!\n", argv[i]);
                        finalcmd = NULL;
                    }
                } else {
                    printf("ERROR: missing parameter for option: --type!\n");
                    help = 1;
                }
            } else {
                filename = argv[i];
            }
        }
    }

    if (help == 1) {
        return;
    }

    if (asprintf(&statname,"%s.xml",filename) == -1) {
        perror("ERROR");
        return;
    }
    if (!freopen(filename, "r", stdin)) {
        printf("ERROR: The specified file (%s) is not exists or \n", filename);
        printf("       you don't have a right permissions!\n");
        return;
    }

    if (!finalcmd) {
        //when we specify the file type, the programm will skip the test
        char mime[50];
        totest = popen("file -i -b -","r");
        if (fscanf(totest,"%49s",mime) != 1) {
            mime[0] = '\0';
        }
        pclose(totest);

        for (i = 0; strcmp(MIME[i],"") != 0; i++) {
            if (strcmp(MIME[i],mime) == 0) {
                finalcmd = CMD[i];
                archive_type = i;
                break;
            }
        }

        if (archive_type > -1 && archive_type < 3) {
            printf("INFO: detected file type: %s\n", TYPE[archive_type]);
        }
    } else {
        if (archive_type > -1 && archive_type < 3) {
            printf("INFO: the specified archive type: %s\n", TYPE[archive_type]);
        }
    }

    if (!finalcmd) {
        printf("ERROR: Couldn't detect archive type\n");
        return;
    }

    printf("INFO: cracking %s, status file: %s\n", filename, statname);

    if (loadstatus() == 1) {
        printf("ERROR: The status file (%s) is corrupted!\n", statname);
        return;
    }

    ABCLEN = strlen(ABC);

    if (password[0] == '\0') {
        password[0] = ABC[0];
    }

    crack_start(threads);
}

int main(int argc, char **argv) {
    // Print author
    printf("RarCrack! 0.2 by David Zoltan Kedves (kedazo@gmail.com)\n\n");

    init(argc,argv);
}
