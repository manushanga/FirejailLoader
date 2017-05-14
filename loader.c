#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
    
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_MATCHES 32

static void loader_main() __attribute__((constructor));

char cmdline[256];
char loader[] = "firejail";
char loaderCmdline[256];
char confFile[256];
char *names[MAX_MATCHES];

#ifdef DEBUG
#define DBG printf
#else
#define DBG
#endif
void remove_trailing_spaces(char *str)
{
    while (!isspace(*str))
    {
        str++;
    }
    
    while (*str != '\0')
    {
        *str = '\0';
        str++;
    }
}
void loader_main()
{
    snprintf(confFile, 255, "%s/.loader.conf", getenv("HOME"));
    DBG("%s\n", confFile);
    struct stat confFileStat;
    
    stat(confFile, &confFileStat);
    
    int confFd = open(confFile, O_RDONLY);
    
    if (confFd == -1)
    {
        return;
    }
    DBG("%s\n", confFile);
    char* conf = (char*) malloc(confFileStat.st_size);
    if (conf == NULL)
    {
        return;
    }
    DBG("%s\n", confFile);
    ssize_t ret  = read(confFd, conf, confFileStat.st_size);
    if (ret == -1)
    {
        return;
    }
    size_t fI = 0;
    int matchId = 0;
    DBG("%s\n", confFile);
    names[matchId] = conf;
    matchId++;
    DBG("%s\n", confFile);
    for (;fI < confFileStat.st_size-1;fI++)
    {
        DBG("%c\n",conf[fI]);
        if (conf[fI] == ',')
        {
            names[matchId] = &conf[fI+1];
            conf[fI] = '\0';
            
            matchId++;
        }
    }
       
    remove_trailing_spaces(names[matchId-1]);
   
   
    int fd = open("/proc/self/cmdline", O_RDONLY);
    read(fd, cmdline, 255);
    cmdline[255]='\0';
    DBG("main %s", cmdline);
    int x;
    
    close(confFd);
    close(fd);
    
    for (x = 0;x<matchId;x++)
    {
        DBG("%s\n",names[x]);
        if (strstr(cmdline, names[x]) != NULL)
        {
            DBG("highjack!\n");
            
            free(conf);
            execlp(loader, cmdline, NULL);
        }
    }
    
}
