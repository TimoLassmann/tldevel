#include "tldevel.h"

#include "tlrng.h"

#define TLALPHABET_IMPORT
#include "tlalphabet.h"


struct alphabet{
        uint8_t to_internal[256];
        /* int8_t to_external[32]; */
        struct rng_state* rng_state;
        uint8_t type;
        uint8_t L;
        uint8_t r_type;
        uint8_t r_options;
};



static int create_default_protein(struct alphabet* a);
static int create_nonambiguous_protein(struct alphabet* a);
static int create_default_DNA(struct alphabet* a);
static int create_nonambiguous_DNA(struct alphabet* a);
static int create_reduced_protein(struct alphabet* a);

static int clean_and_set_to_extern(struct alphabet* a);
static int merge_codes(struct alphabet*a,const int X, const int Y);

int create_alphabet(struct alphabet** alphabet, struct rng_state* rng,int type)
{
        struct alphabet* a = NULL;
        int i;


        MMALLOC(a, sizeof(struct alphabet));
        a->L = 0;

        a->r_type = UINT8_MAX;
        a->r_options = 0;
        a->rng_state = NULL;
        if(rng != NULL){
                RUNP(a->rng_state = init_rng_from_rng(rng));
        }else{
                RUNP(a->rng_state = init_rng(0));
        }
        for(i = 0; i < 256;i++){
                a->to_internal[i] = UINT8_MAX;

        }

        /* for(i = 0; i < 32;i++){ */
        /*         a->to_external[i] = -1; */
        /* } */
        a->type = type;

        switch (type) {
        case TLALPHABET_DEFAULT_PROTEIN : {
                RUN(create_default_protein(a));
                break;
        }
        case TLALPHABET_DEFAULT_DNA : {
                RUN(create_default_DNA(a));
                break;
        }
        case TLALPHABET_REDUCED_PROTEIN : {
                RUN(create_reduced_protein(a));
                break;
        }
        case TLALPHABET_NOAMBIGIOUS_PROTEIN:{
                RUN(create_nonambiguous_protein(a));
                break;
        }
        case TLALPHABET_NOAMBIGUOUS_DNA:{
                RUN(create_nonambiguous_DNA(a));
                break;
        }
        default:
                ERROR_MSG("Alphabet type %d not recognised", type);
                break;
        }

        RUN(clean_and_set_to_extern(a));


        *alphabet = a;
        return OK;
ERROR:
        if(a){
                MFREE(a);
        }
        return FAIL;
}

void free_alphabet(struct alphabet* a)
{
        if(a){
                free_rng(a->rng_state);
                MFREE(a);
        }
}

uint8_t tlalphabet_get_code(const struct alphabet* a,const char c)
{
        uint8_t o;
        o = a->to_internal[(int)c];
        if(o == a->r_type){
                o = tl_random_int(a->rng_state, a->r_options);
        }
        return o;
}

int convert_to_internal(struct alphabet* a, uint8_t* seq, int len)
{
        uint8_t* t = NULL;
        int i;
        t = a->to_internal;
        for(i = 0;i < len;i++){
                seq[i] = t[seq[i]];
                if(seq[i] == a->r_type){
                        seq[i] = tl_random_int(a->rng_state, a->r_options);
                }
        }
        return OK;
}

/* int convert_to_external(struct alphabet* a, uint8_t* seq, int len) */
/* { */
/*         int8_t* t = NULL; */
/*         int i; */
/*         t = a->to_external; */

/*         for(i = 0;i < len;i++){ */
/*                 seq[i] = t[seq[i]]; */
/*         } */
/*         return OK; */
/* } */


int create_default_protein(struct alphabet* a)
{
        char aacode[20] = "ACDEFGHIKLMNPQRSTVWY";

        int code;
        int i;
        code = 0;
        for(i = 0; i < 20;i++){
                //fprintf(stdout,"%c %d CODE: %d\n", aacode[i], (int) aacode[i], code);
                a->to_internal[(int) aacode[i]] = code;

                code++;
        }
        /* ambiguity codes  */
        /* BZX  */

        a->to_internal[(int) 'B'] = code;

        a->to_internal[(int) 'Z'] = code;
        a->to_internal[(int) 'X'] = code;

        /* Some protein sequences contain 'U' - a non-IUPAC code
           I will treat these as an ambiguous aa
           e.g:
           >Q74EN2_GEOSL/108-206
           TRELEALVAKGTEEGGYLLIDSRPAGKYNEAHIPTAVSIPFAELEKNPALLTASKDRLLVFYCGGVTUVLSPKSAGLAKKSGYEKVRVYLDGEPEWKKA

        */
        a->to_internal[(int) 'U'] = code;
        /*
          >sp|Q8TTA5|MTBB1_METAC Dimethylamine methyltransferase MtbB1 OS=Methanosarcina acetivorans (strain ATCC 35395 / DSM 2834 / JCM 12185 / C2A) OX=188937 GN=mtbB1 PE=3 SV=3
          MATEYALRMGDGKRVFLTKEKILEELEAGMANASDLGEIPDLSADEIDKLAEILMMPGKA
          VSVEQGMEVPVTHDIGTIRLDGDQGNSGVGIPSSRLVGCMTHERAFGADTMELGHIDYSF
          KPVKPVVSNECQAMEVCQQNMIIPLFYGAMPNMGLYYTPDGPFENPGDLMKAFKIQEAWE
          SMEHAAEHLTRDTVWVMQKLFASGTDGVNFDTTAAAGDADMYGTLHAIEALRKEFPDMYI
          EAGMAGECVLGMHGNLQYDGVTLAGLWPHQQAPLVAKAGANVFGPVCNTNTSKTSAWNLA
          RAVNFMKAAVQASPIPCHVDMGMGVGGIPMLETPPVDAVTRASKAMVEVAGVDGIOIGVG   <- there is an 'O' here DGI..O..IG
          DPLGMPISHIMASGMTGIRAAGDLVARMQFSKNMRIGEAKEYVAKKLNVDVMDLADEHVM
          RELREELDIGVITSVPGAAKGIAAKMNIEKLLDIKINSCNLFRKQIQ
        */
        a->to_internal[(int) 'O'] = code;
        code++;
        return OK;
}

int create_nonambiguous_protein(struct alphabet* a)
{
        char aacode[20] = "ACDEFGHIKLMNPQRSTVWY";

        int code;
        int i;
        code = 0;
        for(i = 0; i < 20;i++){
                //fprintf(stdout,"%c %d CODE: %d\n", aacode[i], (int) aacode[i], code);
                a->to_internal[(int) aacode[i]] = code;

                code++;
        }
        /* ambiguity codes  */
        /* BZX  */

        a->to_internal[(int) 'B'] = code;

        a->to_internal[(int) 'Z'] = code;
        a->to_internal[(int) 'X'] = code;

        /* Some protein sequences contain 'U' - a non-IUPAC code
           I will treat these as an ambiguous aa
           e.g:
           >Q74EN2_GEOSL/108-206
           TRELEALVAKGTEEGGYLLIDSRPAGKYNEAHIPTAVSIPFAELEKNPALLTASKDRLLVFYCGGVTUVLSPKSAGLAKKSGYEKVRVYLDGEPEWKKA

        */
        a->to_internal[(int) 'U'] = code;
        /*
          >sp|Q8TTA5|MTBB1_METAC Dimethylamine methyltransferase MtbB1 OS=Methanosarcina acetivorans (strain ATCC 35395 / DSM 2834 / JCM 12185 / C2A) OX=188937 GN=mtbB1 PE=3 SV=3
          MATEYALRMGDGKRVFLTKEKILEELEAGMANASDLGEIPDLSADEIDKLAEILMMPGKA
          VSVEQGMEVPVTHDIGTIRLDGDQGNSGVGIPSSRLVGCMTHERAFGADTMELGHIDYSF
          KPVKPVVSNECQAMEVCQQNMIIPLFYGAMPNMGLYYTPDGPFENPGDLMKAFKIQEAWE
          SMEHAAEHLTRDTVWVMQKLFASGTDGVNFDTTAAAGDADMYGTLHAIEALRKEFPDMYI
          EAGMAGECVLGMHGNLQYDGVTLAGLWPHQQAPLVAKAGANVFGPVCNTNTSKTSAWNLA
          RAVNFMKAAVQASPIPCHVDMGMGVGGIPMLETPPVDAVTRASKAMVEVAGVDGIOIGVG   <- there is an 'O' here DGI..O..IG
          DPLGMPISHIMASGMTGIRAAGDLVARMQFSKNMRIGEAKEYVAKKLNVDVMDLADEHVM
          RELREELDIGVITSVPGAAKGIAAKMNIEKLLDIKINSCNLFRKQIQ
        */
        a->to_internal[(int) 'O'] = code;
        a->r_type = code;
        a->r_options = 20;
        code++;
        return OK;
}


int create_default_DNA(struct alphabet* a)
{


        char dnacode[16] = "ACGTUNRYSWKMBDHV";
        int code;
        int i;
        code = 0;
        for(i = 0; i < 16;i++){
                //fprintf(stdout,"%c %d CODE: %d\n", aacode[i], (int) aacode[i], code);
                a->to_internal[(int) dnacode[i]] = code;

                code++;
        }

        merge_codes(a,'U','T');


        /* R.................A or G */
        /* Y.................C or T */
        /* S.................G or C */
        /* W.................A or T */
        /* K.................G or T */
        /* M.................A or C */
        /* B.................C or G or T */
        /* D.................A or G or T */
        /* H.................A or C or T */
        /* V.................A or C or G */
        merge_codes(a,'N','R');
        merge_codes(a,'N','Y');
        merge_codes(a,'N','S');
        merge_codes(a,'N','W');
        merge_codes(a,'N','K');
        merge_codes(a,'N','M');
        merge_codes(a,'N','B');
        merge_codes(a,'N','D');
        merge_codes(a,'N','H');
        merge_codes(a,'N','V');

        return OK;
}

int create_nonambiguous_DNA(struct alphabet* a)
{

        char dnacode[5] = "ACGTU";

        int code;
        int i;
        code = 0;
        for(i = 0; i < 5;i++){
                //fprintf(stdout,"%c %d CODE: %d\n", aacode[i], (int) aacode[i], code);
                a->to_internal[(int) dnacode[i]] = code;

                code++;
        }

        merge_codes(a,'U','T');

        a->to_internal[(int) 'N'] = code;
        a->to_internal[(int) 'R'] = code;
        a->to_internal[(int) 'Y'] = code;
        a->to_internal[(int) 'S'] = code;
        a->to_internal[(int) 'W'] = code;
        a->to_internal[(int) 'K'] = code;
        a->to_internal[(int) 'M'] = code;
        a->to_internal[(int) 'B'] = code;
        a->to_internal[(int) 'D'] = code;
        a->to_internal[(int) 'H'] = code;
        a->to_internal[(int) 'V'] = code;

        a->r_type = code;
        a->r_options = 4;
        code++;
        return OK;
}

int create_reduced_protein(struct alphabet* a)
{
        char aacode[20] = "ACDEFGHIKLMNPQRSTVWY";

        int code;
        int i;
        code = 0;
        for(i = 0; i < 20;i++){
                a->to_internal[(int) aacode[i]] = code;
                code++;
        }
        /* ambiguity codes  */
        /* BZX  */

        a->to_internal[(int) 'B'] = code;
        code++;

        a->to_internal[(int) 'Z'] = code;
        code++;

        a->to_internal[(int) 'X'] = code;
        code++;
        /* From  Clustering huge protein sequence sets in linear time
Martin Steinegger 1, 2, 3 and Johannes Söding 1 */
        /* The default alphabet with A = 13 merges (L,M), (I,V), (K,R), (E, Q), (A,S,T), (N, D) and (F,Y).*/

        /* reduced codes */
        merge_codes(a,'L','M');
        merge_codes(a,'I','V');

        merge_codes(a,'K','R');
        merge_codes(a,'E','Q');

        merge_codes(a,'A','S');
        merge_codes(a,'A','T');
        merge_codes(a,'S','T');
        merge_codes(a,'N','D');

        merge_codes(a,'F','Y');

        /* merge ambiguity codes */
        merge_codes(a,'B','N');
        merge_codes(a,'B','D');

        merge_codes(a,'Z','E');
        merge_codes(a,'Z','Q');


        return OK;

}

int merge_codes(struct alphabet*a,const int X, const int Y)
{
        int min;

        min = MACRO_MIN(a->to_internal[X],a->to_internal[Y]);

        ASSERT(min != -1, "code not set!");

        a->to_internal[X] = min;
        a->to_internal[Y] = min;
        return OK;
ERROR:
        return FAIL;
}

int clean_and_set_to_extern(struct alphabet* a)
{
        int i;
        uint8_t code = 0;
        uint8_t trans[256];
        for(i = 0; i < 256;i++){
                trans[i] = UINT8_MAX;

        }

        for(i = 64; i < 96;i++){
                if(a->to_internal[i] != UINT8_MAX){
                        trans[a->to_internal[i]] = 1;
                }
        }
        code = 0;
        for(i = 0; i < 32;i++){
                if(trans[i] == 1){
                        trans[i] = code;
                        code++;
                }
        }
        a->r_type = trans[a->r_type];
        a->L = code;
        for(i = 64; i < 96;i++){
                if(a->to_internal[i] != UINT8_MAX){
                        a->to_internal[i] = trans[a->to_internal[i]];//a->to_internal[i]];
                        a->to_internal[i+32] = a->to_internal[i];

                }

        }

        /* for(i = 64;i < 96;i++){ */
        /*         if(a->to_internal[i] != -1){ */
        /*                 a->to_external[a->to_internal[i]] = i; */
        /*         } */
        /* } */

        return OK;
}
