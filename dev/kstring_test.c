


#include "tldevel.h"
#include "kstring.h"



int main(int argc, char *argv[])
{
        kstring_t s = { 0, 0, NULL };
        char* free_string;
        char hello[] = "hello";
        LOG_MSG("Playing with kstring library... ");


        int l = kputs(hello,&s);



        kputc(' ',&s);

        kputs(hello,&s);
        fprintf(stdout,"%s\n", ks_str(&s));
        /*static inline int kputsn(const char *p, int l, kstring_t *s)

          static inline int kputs(const char *p, kstring_t *s)
          static inline int kputc(int c, kstring_t *s)


          static inline char *ks_str(kstring_t *s)
          {
          return s->s;
          }

          static inline size_t ks_len(kstring_t *s)
        */
        kstring_t s2 = {0,0,NULL};

        kputsn(s.s,s.l,&s2);

        fprintf(stdout,"%s\n", ks_str(&s2));
        free_string = ks_release(&s);
        MFREE(s2.s);
        MFREE(free_string);
        return EXIT_SUCCESS;
}
