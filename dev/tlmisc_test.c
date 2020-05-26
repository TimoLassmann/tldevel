#include "tldevel.h"
#include "tlmisc.h"

static int cmd_line_test(void);
int cmd_line_test(void)
{
        const int argc = 10;
        char* const argv[10] = {
                "Low",
                "level",
                "programming",
                "is",
                "good",
                "for",
                "the",
                "soul",
                "- John",
                "Carmack"
        };
        char* cmd = NULL;
        int alloc_len;

        RUN(make_cmd_line(&cmd,argc,argv ));

        RUN(get_dim1(cmd,&alloc_len));
        fprintf(stdout,"%s\n%d characters allocated.\n", cmd,alloc_len);
        gfree(cmd);
        return OK;
ERROR:
        return FAIL;
}

int main(void)
{


        //char testpath[] = "/home/OTTO/mainProject/fileA.txt";
        char* testpath = NULL;
        char* out = NULL;

        MMALLOC(testpath,sizeof(char) * 1024);

        snprintf(testpath,1024,"%s","/home/OTTO/mainProject/fileA.txt");


        RUN(tlfilename(testpath,&out));
        fprintf(stdout,"%s\n%s\n",testpath,out);
        MFREE(out);
        out = NULL;
        RUN(tldirname(testpath,&out));
        fprintf(stdout,"%s\n%s\n",testpath,out);
        MFREE(out);


        snprintf(testpath,1024,"%s","OTTO/mainProject/fileA.txt");


        RUN(tlfilename(testpath,&out));
        fprintf(stdout,"%s\n%s\n",testpath,out);
        MFREE(out);
        out = NULL;
        RUN(tldirname(testpath,&out));
        fprintf(stdout,"%s\n%s\n",testpath,out);
        MFREE(out);


        snprintf(testpath,1024,"%s","/home/OTTO/mainProject/fileAtxt");


        RUN(tlfilename(testpath,&out));
        fprintf(stdout,"%s\n%s\n",testpath,out);
        MFREE(out);
        out = NULL;
        RUN(tldirname(testpath,&out));
        fprintf(stdout,"%s\n%s\n",testpath,out);
        MFREE(out);


        MFREE(testpath);


        cmd_line_test();
        return OK;
ERROR:
        return FAIL;
}
