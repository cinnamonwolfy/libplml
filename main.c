#include <pl32.h>
#define PLML_FILE 1
#define PLML_INVALID 2

#define PLML_TYPE_STRING 8
#define PLML_TYPE_BOOL 9
#define PLML_TYPE_INT 10
#define PLML_TYPE_HEADER 11

typedef struct pltoken {
	string_t name;
	int type;
	memptr_t value;
	plmt_t* mt;
} pltoken_t;

void plMLError(string_t name, int errNum, plmt_t* mt){
	printf("%s: ", name);

	switch(errNum){
		case 0:
			printf("Success\n");
			return;
		case PLML_FILE:
			printf("Couldn't open file\n");
			break;
		case PLML_INVALID:
			printf("Invalid token\n");
			break;
		default:
			printf("General Fatal Error\n");
			break;
	}

	plMTStop(mt);
	exit(errNum);
}

void plMLFreeToken(pltoken_t* token){
	plMTFree(token->mt, token->name);
	if(token->type != PLML_TYPE_HEADER)
		plMTFree(token->mt, token->value);

	plMTFree(token->mt, token);
}

pltoken_t* plMLParse(string_t string, plmt_t* mt){
	plarray_t* tokenizedStr = plParser(string, mt);
	if(tokenizedStr->size != 3 && tokenizedStr->size != 1)
		plMLError("plMLParse", PLML_INVALID, mt);

	string_t* tokenizedStrArr = tokenizedStr->array;
	pltoken_t* returnToken = plMTAllocE(mt, sizeof(pltoken_t));
	returnToken->mt = mt;

	if(tokenizedStr->size == 3){
		byte_t* basicStringStart = strchr(string, '"');
		byte_t* literalStringStart = strchr(string, '\'');
		byte_t* beginningOfThirdToken  = strchr(string, '=') + 2;

		returnToken->name = plMTAllocE(mt, strlen(tokenizedStrArr[0]) + 1);
		strcpy(returnToken->name, tokenizedStrArr[0]);

		if(basicStringStart != beginningOfThirdToken && literalStringStart != beginningOfThirdToken){
			string_t leftoverStr = NULL;
			int tempInt = strtol(tokenizedStrArr[2], &leftoverStr, 10);
			if(leftoverStr != NULL && *leftoverStr != '\0'){
				returnToken->type = PLML_TYPE_BOOL;
				returnToken->value = plMTAllocE(mt, sizeof(bool));

				if(strcmp(tokenizedStrArr[2], "true") == 0)
					*((bool*)returnToken->value) = true;
				else if(strcmp(tokenizedStrArr[2], "false") == 0)
					*((bool*)returnToken->value) = false;
				else
					plMLError("plMLParse", PLML_INVALID, mt);
			}else{
				returnToken->type = PLML_TYPE_INT;
				returnToken->value = plMTAllocE(mt, sizeof(int));
				memcpy(returnToken->value, &tempInt, sizeof(int));
			}
		}else{
			returnToken->type = PLML_TYPE_STRING;
			returnToken->value = plMTAllocE(mt, strlen(tokenizedStrArr[2]) + 1);
			strcpy(returnToken->value, tokenizedStrArr[2]);
		}
	}else{
		returnToken->type = PLML_TYPE_HEADER;
		returnToken->value = NULL;

		byte_t* headerStart = strchr(tokenizedStrArr[0], '[') + 1;
		byte_t* headerEnd = strchr(tokenizedStrArr[0], ']');
		size_t nameSize = headerEnd - headerStart;

		if(headerStart == NULL || headerEnd == NULL)
			plMLError("plMLParse", PLML_INVALID, mt);

		returnToken->name = plMTAllocE(mt, nameSize + 1);
		memcpy(returnToken->name, headerStart, nameSize);
	}

	plMTFreeArray(tokenizedStr, true);
	return returnToken;
}

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
