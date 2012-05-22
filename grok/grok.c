// $Id: grok.c,v 1.8 2003/05/21 00:53:46 idcmp Exp $

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

FILE *grokfp;
void *html;
char *currentPos;

extern int maphtml(char *);
extern char *trim(char *);
extern void set_current_value(char *);
extern char *strcasestr(char *,char *);

char *error_msg = NULL, *fetch_url = NULL, *temp_file = NULL;
unsigned long url_expiry = 600;
char *variable[50],*content[50];
int current_var = -1;

#define TRUE 1
#define FALSE (!TRUE)

char *strupper(char *str)
{
    char *cp;
    for(cp=str;*cp;cp++) {
        *cp = toupper(*cp);
    }
    
    return str;
        
}


int fetch_content() {
    char sys[4096];
    struct stat sbuf;
    
    if (fetch_url == NULL || temp_file == NULL) return FALSE;
    
    
    if (url_expiry > 0) {
        // check to ensure it hasn't expired
        if (stat(temp_file,&sbuf) == 0) {
            if (sbuf.st_mtime + url_expiry >= time(NULL)) return TRUE;
        }
    }
    
    sprintf(sys,"wget -U '%s' -q -O '%s' '%s'",
            "Mozilla/5.001 (windows; U; NT4.0; en-us) Gecko/25250101",
            temp_file,
            fetch_url);
    if (system(sys) != 0) return FALSE;
    
    return TRUE;
}

/* Seek to the specified string.  Leave the cursor
 * at the next character after the string ends.
 */
void *seek_to(char *str)
{
    char *cp = strcasestr(currentPos,str);
    if (cp == NULL) {
        fprintf(stderr,"Cannot seek to '%s'\n",str);
        return NULL;
    }
    cp += strlen(str);
    return cp;
}

char *trim(char *str)
{
    char *cp;
    
    for(cp=str;cp[0]==' ';cp++);
    for(;cp[strlen(cp)-1]==' ';) cp[strlen(cp)-1]='\0';
    
    return strdup(cp);
}

char *capture_until(char *str)
{
    char *search,*startPos = currentPos;
    char *cv;
    int i;
    size_t len;
    
    search  = strcasestr(startPos,str);
    if (search == NULL) return NULL;
    
    for(;*startPos == ' ';startPos++);
    len = search - startPos;
        
    cv = calloc(1,len+1);
    strncpy(cv,startPos,len);
    for(;cv[strlen(cv)-1]==' ';) cv[strlen(cv)-1]='\0';
    for(i=0;i != strlen(cv); i++) {
        if (strchr("\t\n\r\b",cv[i])) cv[i]=' ';
    }
    
    set_current_value(cv);
    
    return seek_to(str);
}

void set_current_value(char *str)
{
    content[current_var] = trim(str);
}

void set_current_variable(char *varname)
{
    variable[++current_var] = strdup(varname);
    
}

char *get_variable(char *name) {
    int i;
    
    for(i=0;i < current_var+1;i++) {
        if (!strcmp(name,variable[i])) return content[i];
    }
    
    return NULL;
}


char *expand_string(char *template)
{
    char bigbuf[4096], vbuf[4096], *cp = template;
    int is_var = 0;
    
    bzero(&bigbuf,4096);
    bzero(&vbuf,4096);
    
    for(;*cp != '\0';cp++) {
        char c = cp[0];
        
        if (!is_var && c == '{') {
            is_var = 1;
            continue;
        }
        
        if (is_var && c == '}') {
            char *v;
            v = get_variable(vbuf);
            if (v != NULL) {
                strcat(&bigbuf[strlen(bigbuf)],v);
            } else {
                fprintf(stderr,"No such variable, %s.\n",vbuf);
                return NULL;
            }
            
            is_var = 0;
            bzero(&vbuf,4096);
            continue;
        }
        
        if (is_var) {
            vbuf[strlen(vbuf)] = c;
        } else {
            bigbuf[strlen(bigbuf)] = c;
        }
        
        if (strlen(vbuf) > 4000 || strlen(bigbuf) > 4000) return NULL;
    }
    
    return strdup(bigbuf);
    
}


int generate_output(char *template)
{
    char *cp;
    
    cp = expand_string(template);
    
    if (cp == NULL) return FALSE;
    
    printf("%s\n",cp);
    return TRUE;
}


int mainLoop() 
{
    char str[1024];
    
    do {
        bzero(&str,1024);
        fgets(str,1024,grokfp);
        
        str[strlen(str)-1]= '\0';
        
        switch (str[0]) {
        case 'E':
            error_msg = expand_string(&str[1]);
            break;
        case 'U':
            fetch_url = expand_string(&str[1]);
            break;
        case 'X':
            url_expiry = atol(&str[1]);
            break;
        case 'T':
            temp_file = expand_string(&str[1]);
            break;
        case '*':
            if (!fetch_content() || !maphtml(temp_file)) {
                fprintf(stderr,"Unable to process content.\n");
                return FALSE;
            }
            break;
        case 'S':
            currentPos = seek_to(expand_string(&str[1]));
            if (currentPos == NULL) return FALSE;
            break;
        case 'V':
            set_current_variable(&str[1]);
            break;
        case 'C':
            currentPos = capture_until(&str[1]);
            if (currentPos == NULL) return FALSE;
            break;
        case 'O':
            if (!generate_output(&str[1])) return FALSE;
            break;
        case '#': 
            break;
        default:
            break;
        }
        
    } while (!feof(grokfp));
    
    return TRUE;
}



int maphtml(char *filename)
{
    struct stat sbuf;
    int fd;
    
    if (stat(filename,&sbuf) != 0) return FALSE;
    
    fd = open(filename,O_RDONLY);
    
    if (fd == -1) return FALSE;
    
    html = mmap(0,sbuf.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    
    if (html == NULL) return FALSE;
    
    currentPos = html;
    return TRUE;
    
}


int main(int ac,char **av) {
    
    int i;
    char workspace[4096];
    
    if (ac < 2) {
        fprintf(stderr,"Usage: %s <grok file>\n",av[0]);
        return 1;
    }
    
    grokfp = fopen(av[1],"r");
    
    if (!grokfp) {
        fprintf(stderr,"%s: Can't open \"%s\".\n",av[0],av[1]);
        return 1;
    }
    
    bzero(&workspace,4096);
    
    for(i=2; i < ac ; i++) {
        char vbuf[1024];
        sprintf(vbuf,"arg%d",i-2);
        set_current_variable(vbuf);
        set_current_value(strdup(av[i]));
        
        set_current_variable(strupper(vbuf));
        set_current_value(strupper(strdup(av[i])));
        
        strcat(workspace,av[i]);
        strcat(workspace," ");
    }
    
    if (strlen(workspace) > 0) {
        set_current_variable("args");
        set_current_value(workspace);
    }
    
    if (!mainLoop() && error_msg != NULL) {
        printf("%s\n",error_msg);
    }
    
    return 0;
}

