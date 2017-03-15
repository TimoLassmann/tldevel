
#include <stdlib.h>
#include <stdio.h>



int main(int argc, char * argv[])
{
	char line[1000];
	FILE* f_ptr = NULL;
		
	
	
	if(argc ==1){
		fprintf(stderr,"Too few arguments.\n");
		goto ERROR;
	}
	
	f_ptr = fopen(argv[1],"r");
	
	if(f_ptr == NULL ){
		fprintf(stderr,"Failed to open file:%s.\n",argv[1]);
		goto ERROR;
	}


	int i;
	
	fprintf(stdout,"#include \"tldevel.h\" \n\n");
	//fprintf(stdout,"#ifndef keepseq_build_config_header \n");
	
	//fprintf(stdout,"#define keepseq_build_config_header\n");
	
	fprintf(stdout,"char build_config[] = \"");
	while(fgets(line, 1000, f_ptr)){
		i = 0;
		while (i < 1000) {
			if(line[i] == '\n'){
				fprintf(stdout,"\\n");
				break;
			}
			fprintf(stdout,"%c",line[i]);
			i++;
		}
	}
	fclose(f_ptr);
	fprintf(stdout,"\";\n\n");
	
	//fprintf(stdout,"#endif\n");

	return EXIT_SUCCESS;
ERROR:
	return EXIT_FAILURE;
}
