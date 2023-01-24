#include <plml.h>

int main(int argc, string_t argv[]){
	if(argc < 2){
		printf("Error: Not enough arguments\n");
		return 1;
	}

	printf("Parsing PLML...\n\n");

	plmt_t* mt = plMTInit(0);
	plfile_t* fileToParse = plFOpen(argv[1], "r", mt);
	char lineBuffer[4096];

	if(fileToParse == NULL)
		plMLError("plFOpen", PLML_FILE, mt);

	int i = 1;
	while(plFGets(lineBuffer, 4095, fileToParse) != NULL){
		pltoken_t* parsedToken = plMLParse(lineBuffer, mt);

		printf("Token %d\n\n", i);
		printf("Name: %s\n", parsedToken->name);
		printf("Type: ");
		switch(parsedToken->type){
			case PLML_TYPE_STRING:
				printf("String\n");
				printf("Value: %s\n\n", (char*)parsedToken->value);
				break;
			case PLML_TYPE_BOOL:
				printf("Bool\n");
				printf("Value: ");
				if(*((bool*)parsedToken->value))
					printf("True\n\n");
				else
					printf("False\n\n");
				break;
			case PLML_TYPE_INT:
				printf("Integer\n");
				printf("Value: %i\n\n", *((int*)parsedToken->value));
				break;
			case PLML_TYPE_HEADER:
				printf("Header\n\n");
				break;
		}

		plMLFreeToken(parsedToken);
		i++;
	}

	plMTStop(mt);
	return 0;
}
