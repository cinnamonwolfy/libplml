#include <plml.h>

struct plmltoken {
	string_t name;
	int type;
	memptr_t value;
	plmt_t* mt;
};

void plMLError(string_t name, plmlerror_t errVal, plmt_t* mt){
	printf("%s: ", name);

	switch(errVal){
		case PLML_SUCCESS:
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
	exit(errVal);
}

plarray_t* plMLSanitize(string_t string, plmt_t* mt){
	plarray_t* retArr = plParser(string, mt);
	string_t* retStrArr = retArr->array;

	int i = 0;
	while(i < retArr->size){
		if(strchr(retStrArr[i], '#') == retStrArr[i]){
			int j = i;
			while(i < retArr->size){
				plMTFree(mt, retStrArr[i]);
				i++;
			}
			retArr->size = j;
		}
		i++;
	}

	return retArr;
}

plmltoken_t* plMLParse(string_t string, plmt_t* mt){
	plarray_t* tokenizedStr = plMLSanitize(string, mt);
	if(tokenizedStr->size != 3 && tokenizedStr->size != 1)
		plMLError("plMLParse", PLML_INVALID, mt);

	string_t* tokenizedStrArr = tokenizedStr->array;
	plmltoken_t* returnToken = plMTAllocE(mt, sizeof(plmltoken_t));
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

void plMLGetTokenAttrib(plmltoken_t* token, memptr_t ptr, plmlaction_t attribType){
	switch(attribType){
		case PLML_GET_NAME:
			*((string_t*)ptr) = token->name;
			break;
		case PLML_GET_TYPE:
			*((int*)ptr) = token->type;
			break;
		case PLML_GET_VALUE:
			*((memptr_t*)ptr) = token->value;
			break;
	}
}

void plMLFreeToken(plmltoken_t* token){
	plMTFree(token->mt, token->name);
	if(token->type != PLML_TYPE_HEADER)
		plMTFree(token->mt, token->value);

	plMTFree(token->mt, token);
}
