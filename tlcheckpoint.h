#ifndef TLCHECKPOINT_H
#define TLCHECKPOINT_H


#ifdef TLCHECKPOINT_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif


typedef struct checkpoint checkpoint;


EXTERN struct checkpoint* init_checkpoint(char* base_name,char* target_dir);
EXTERN int set_checkpoint_file(struct checkpoint* chk,char* function,char* location,char* cmd);
EXTERN int test_for_checkpoint_file(struct checkpoint* chk,char* function,char* location, char* cmd);
EXTERN void free_checkpoint(struct checkpoint* chk);


#define DECLARE_CHK(n,dir) struct checkpoint* chk_##n = NULL;  RUNP( chk_##n =  init_checkpoint(TOSTRING(n),dir));

#define RUN_CHECKPOINT(n,EXP,CMD) do {                                  \
                if(test_for_checkpoint_file(chk_##n,TOSTRING(EXP),AT,CMD) ==0 ){ \
                        RUN(EXP);                                       \
                        RUN(set_checkpoint_file(chk_##n,TOSTRING(EXP),AT,CMD)); \
                }else{                                                  \
                        log_message("Skipping over: %s (%s)",TOSTRING(EXP),AT); \
                }                                                       \
                chk_##n->test_num += 1;                                 \
        }while (0)




#undef TLCHECKPOINT_IMPORT
#undef EXTERN

#endif
