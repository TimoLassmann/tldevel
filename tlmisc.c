#include <sys/stat.h>

#include <string.h>

#define TLMISC_IMPORT
#include "tlmisc.h"
#include "tldevel.h"

int my_file_exists(const char* name)
{
        struct stat buf;
        int ret,local_ret;
        ret = 0;
        local_ret= stat ( name, &buf );
        /* File found */
        if ( local_ret == 0 )
        {
                ret++;
        }
        return ret;
}

/* I don't like that both libgen and string have functions to work with
   directory / filenames. The functions below copy the input path and alloc
   a new character array to store the output. Needs to be MFREE'd...
 */
int tlfilename(char* path, char** out)
{
        char* tmp = NULL;
        int len;
        int i;
        int c;
        len = 0;

        len = (int) strlen(path);
        MMALLOC(tmp, sizeof(char) * (len+1));
        c = 0;
        for(i = 0;i < len;i++){
                tmp[c] = path[i];
                c++;
                if(path[i] == '/'){
                        c = 0;
                }

        }
        tmp[c] = 0;
        if(c == 0){
                ERROR_MSG("No filename found in: %s", path);
        }
        *out = tmp;
        return OK;
ERROR:
        if(tmp){
                MFREE(tmp);
        }
        return FAIL;
}

int tldirname(char* path, char** out)
{
        char* tmp = NULL;
        int len;
        int i;
        int c;
        int e;
        len = 0;

        len = (int) strlen(path);
        MMALLOC(tmp, sizeof(char) * (len+1));
        c = 0;
        e = 0;
        for(i = 0;i < len;i++){

                tmp[c] = path[i];
                if(path[i] == '/'){
                        e = c;
                }
                c++;
        }
        tmp[e] = 0;
        if(e == 0){
                ERROR_MSG("No dirname found in: %s", path);
        }
        *out = tmp;
        return OK;
ERROR:
        if(tmp){
                MFREE(tmp);
        }
        return FAIL;
}

int make_cmd_line(char** command, const int argc,char* const argv[])
{
        char* cmd = NULL;
        int i,j,c;

        RUN(galloc(&cmd,16384));

        c = 0;
        for(i =0 ; i < argc;i++){
                for(j = 0; j < (int) strlen(argv[i]);j++){
                        if(c == 16384-1){
                                break;
                        }
                        cmd[c] = argv[i][j];
                        c++;

                }
                if(c == 16384-1){
                        break;
                }
                cmd[c] = ' ';
                c++;


        }
        cmd[c] = 0;
        *command = cmd;
        return OK;
ERROR:
        if(cmd){
                gfree(cmd);
        }
        return FAIL;
}
