#ifndef JSONUTIL__h
#define JSONUTIL__h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "main.h"
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
static cJSON *createModeJson(const Mode *mode);
static cJSON *createMessageJson(const Message *message);
char *generateMessageJson(const Message *message);
void parseModeJSON(cJSON *root, const char *envs[], int size);
#endif