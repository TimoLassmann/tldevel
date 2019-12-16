
#include "tldevel.h"
#include "tlseqio.h"



int main(int argc, char *argv[])
{
        struct file_handler* f = NULL;
        if(argc > 1){
                RUN(get_io_handler(&f, argv[1]));
                echo_file(f);

                free_io_handler(f);
        }

        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;
}
