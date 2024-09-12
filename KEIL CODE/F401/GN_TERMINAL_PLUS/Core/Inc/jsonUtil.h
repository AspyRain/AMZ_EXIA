#ifndef JSONUTIL__h
#define JSONUTIL__h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
typedef struct {
    int id;
    char *colors; // Pointer to dynamically allocated string
} Mode;

typedef struct {
    int modeType;
    Mode mode;
    Mode gnMode;
} Message;
static int parseMode(cJSON *modeObj, Mode *mode) ;
int parseMessage(const char *jsonStr, Message *message);
#endif