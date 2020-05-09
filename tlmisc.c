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

int make_cmd_line(char** command, const int argc,char* const argv[])
{
        char* cmd = NULL;
        int i,j,c;

        RUN(galloc(cmd,16384));

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
