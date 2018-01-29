#include<stdio.h>
#include<stdlib.h>
#include<string.h>


void makeClass(float inputClass[], int *inputClassSize,int *inputSize, float decClass[], char decValue[][5], int *decClassSize, float thld[], FILE* fp) {

    char * line = NULL;
    size_t len = 0;
    ssize_t read;    
    char *pch = NULL;
    float *currentArray = NULL;
    int *currentInc = NULL;
    int classRead = 1;
    int initValue = 1;
    float init = 0.3;
    int i;
    
    if (fp == NULL)
           exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1) {
        if(line[0] == '~') {
            if(strcmp(line, "~CL\n")) {
                classRead = 0;
            } else {
                classRead = 1;
            }
        }
        if(!classRead) 
            continue;
        if(read > 1) {
            line[read - 1] = '\0';
        }        
        
        pch = strtok (line,"|");
        if(!strcmp(pch, "D")) {
            currentArray = decClass;
            currentInc = decClassSize;
            initValue = 0;
        } else {
            currentArray = inputClass;
            currentInc = inputClassSize;
            initValue = 1;
            (*inputSize)++;
            
        }
        
        while (pch != NULL){            
            pch = strtok (NULL, "|");
            if(pch) {
                //printf ("%s\n",pch);
                if(initValue) 
                    currentArray[*currentInc] = init;
                else {
                    currentArray[*currentInc] = (*currentInc);
                    strcpy(decValue[*currentInc], pch);
                } 
                (*currentInc)++;
            }               
        }
    }
    if (line)
        free(line);
    float thrhld = (float)1/(*decClassSize);
    for(i = 0; i<(*decClassSize)-1; i++) {
        thld[i] = thrhld*(i+1);
    }
    for(i = 0; i<(*decClassSize); i++) {
        if(i == 0) {
            decClass[i] = 0;
        } else if(i == (*decClassSize) - 1) {
            decClass[i] = 1;
        } else {
            decClass[i] = (thld[i-1] + thld[i])/2;
        }        
    }
}

float getClass(float inputClass[], float input[], int size){

    float weight = 0;
    int i;
    for(i = 0; i<size; i++) {
        weight += (inputClass[i] * (float)input[i]);
    }
    return weight;
}

int needAdjust(float weight, float dec[], float thrhld[], int size) {

    int i;
    for(i = 0; i<size; i++) {
        if(dec[i]) {     
            break;
        }
    }
    if(i == 0) {
        return weight < thrhld[0];
    } else if(i == size - 1) {
        return weight >= thrhld[i -1];
    } else {
        return (weight >= thrhld[i-1] && weight < thrhld[i]);
    }
}

void adjustWeight(float inputClass[], float input[], int inputClassSize, int classSize, float class, int decSize, float dec[], float thrhld[]) {
    float lr = 0.3;
    float err = 0.0;
    float actual = getClass(inputClass, input, inputClassSize);
    int needA = needAdjust(actual, dec, thrhld, decSize);
    while(!needA) {		
		err = class - actual;
		float meanError = err / classSize;
		int i;
	   	for(i=0;i<inputClassSize; i++) {
		    if(input[i]) {
		       inputClass[i] += meanError;
		    }    
		}
		actual = getClass(inputClass, input, inputClassSize);
		needA = needAdjust(actual, dec, thrhld, decSize);
	}
}



void learn(float inputClass[], int inputClassSize, int inputSize, float decClass[],int decClassSize, float thld[], FILE* fp) {
    char * line = NULL;
    size_t len = 0;
    ssize_t read;    
    char *pch = NULL;
    float *currentArray = NULL;
    int i = 0, j;
    int trainingRead = 1;
    float input[100];
    float dec[10];
    int flag = 1;
    //float threshhold = 0.5; //need to be calculated latter on
    float weight = 0;
    
    if (fp == NULL)
           exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1) {
        if(line[0] == '~') {
            if(strcmp(line, "~TR\n")) {
                trainingRead = 0;
            } else {
                trainingRead = 1;
            }
            continue;
        } else if(line[0] == '!') {
            continue;
        }
        if(!trainingRead) 
            continue;
        if(read > 1) {
            line[read - 1] = '\0';
        }        
        currentArray = input;
        i = 0;
        flag = 1;
        
        pch = strtok (line,"|");        
        
        while (pch != NULL){                            
            currentArray[i] = atof(pch);
            i++;
            if(i == inputClassSize && flag) {
                i = 0;
                flag = 0;
                currentArray = dec;
            }
            pch = strtok (NULL, "|");             
        }       
        weight = getClass(inputClass, input, inputClassSize);
        float weight2 = getClass(decClass, dec, decClassSize);
       // if(!needAdjust(weight, dec, thld,decClassSize)) {
            adjustWeight(inputClass, input, inputClassSize, inputSize, weight2, decClassSize, dec, thld);
       // } 
    }
    if (line)
        free(line);
}

int getI(float weight, float thld[], int size) {
    int i;
    for(i = 0; i<size;i++) {
        if(weight < thld[i]) {
            return i;
        }
    }
    return i;
}

void test(float inputClass[], int inputSize, float decClass[], char decValue[][5], int decSize,float thhld[], FILE* fp) {
    char * line = NULL;
    size_t len = 0;
    ssize_t read;    
    char *pch = NULL;    
    int i = 0, j;
    int testRead = 1;
    float input[100];     
    float weight = 0;
    
    if (fp == NULL)
           exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1) {
        if(line[0] == '~') {
            if(strcmp(line, "~TE\n")) {
                testRead = 0;
            } else {
                testRead = 1;
            }
            continue;
        } else if(line[0] == '!') {
            continue;
        }
        if(!testRead) 
            continue;
        if(read > 1) {
            line[read - 1] = '\0';
        }        
        i = 0;
        pch = strtok (line,"|");  
        while (pch != NULL){  
            input[i] = atof(pch);
            i++;
            if(i == inputSize) {
                break;
            }
            pch = strtok (NULL, "|");             
        }
                
        weight = getClass(inputClass, input, inputSize);   
        printf("%f ", weight);        
        i = getI(weight, thhld, decSize - 1);
        printf("%s\n", decValue[i]);
        
    }
    if (line)
        free(line);
}


void main(int argc, char* argv[]) {
    float inputClass[100];
    float decClass[10], threshhold[10];
    char decValue[10][5];
    int inputClassSize = 0, inputSize = -1;
    int decSize = 0, i;
    FILE* inFile;
    char fileName[50];
    if(argc <= 1) {
        printf("usage: ./out inputFileName\n");
        exit(1);
    }
    strcpy(fileName, argv[1]);
    inFile = fopen(fileName, "r");  
    makeClass(inputClass, &inputClassSize, &inputSize, decClass, decValue, &decSize,threshhold, inFile);  
    /*
    *for(i=0; i<inputClassSize; i++){
    *    printf("%f|", inputClass[i]);
    *}
    *printf("\n");  
    */
    fclose(inFile);
    inFile = fopen(fileName, "r");     
    learn(inputClass, inputClassSize, inputSize, decClass, decSize,threshhold, inFile);
    /*
    *for(i=0; i<inputClassSize; i++){
    *    printf("%f|", inputClass[i]);
    *}
    *printf("\n");  
    */
    fclose(inFile);
    inFile = fopen(fileName, "r");     
    test(inputClass, inputClassSize, decClass, decValue, decSize,threshhold, inFile);
    
    
}
    
