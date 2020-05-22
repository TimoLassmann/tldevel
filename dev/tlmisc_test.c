#include "tldevel.h"
#include "tlmisc.h"

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
        return OK;
ERROR:
        return FAIL;
}
