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

	int i = 1;
	while(plFGets(lineBuffer, 4095, fileToParse) != NULL){
		plmltoken_t* parsedToken = plMLParse(lineBuffer, mt);

		string_t name;
		int type;
		memptr_t value;

		plMLGetTokenAttrib(parsedToken, &name, PLML_GET_NAME);
		plMLGetTokenAttrib(parsedToken, &type, PLML_GET_TYPE);
		plMLGetTokenAttrib(parsedToken, &value, PLML_GET_VALUE);

		printf("Token %d\n\n", i);
		printf("Name: %s\n", name);
		printf("Type: ");
		switch(type){
			case PLML_TYPE_STRING:
				printf("String\n");
				printf("Value: %s\n\n", (string_t)value);
				break;
			case PLML_TYPE_BOOL:
				printf("Bool\n");
				printf("Value: ");
				if(*((bool*)value))
					printf("True\n\n");
				else
					printf("False\n\n");
				break;
			case PLML_TYPE_INT:
				printf("Integer\n");
				printf("Value: %i\n\n", *((int*)value));
				break;
			case PLML_TYPE_HEADER:
				printf("Header\n\n");
				break;
			case PLML_TYPE_FLOAT:
				printf("Float\n");
				printf("Value: %f\n\n", *((double*)value));
		}

		plMLFreeToken(parsedToken);
		i++;
	}

	plMTStop(mt);
	return 0;
}
