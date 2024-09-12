#include "jsonUtil.h"
#include "string.h"
#include <rtthread.h>
#include <stdlib.h> // For malloc and free

// Helper function to parse the mode from JSON
static int parseMode(cJSON *modeObj, Mode *mode) {
    if (modeObj == NULL) {
        return 0; // Failed to parse
    }

    cJSON *idObj = cJSON_GetObjectItem(modeObj, "id");
    cJSON *colorsObj = cJSON_GetObjectItem(modeObj, "colors");

    if (idObj != NULL && colorsObj != NULL && idObj->type == cJSON_Number && colorsObj->type == cJSON_String) {
        mode->id = idObj->valueint;

        // Allocate memory for the color string
        size_t len = strlen(colorsObj->valuestring) + 1; // +1 for null terminator
        mode->colors = (char *)malloc(len);
        if (mode->colors == NULL) {
            return 0; // Failed to allocate memory
        }
        strcpy(mode->colors, colorsObj->valuestring);
        return 1; // Success
    }

    return 0; // Failed to parse
}

// Function to parse the message from JSON
int parseMessage(const char *jsonStr, Message *message) {
    cJSON *json = cJSON_Parse(jsonStr);

    if (json == NULL) {
        return 0; // Failed to parse JSON
    }

    cJSON *modeTypeObj = cJSON_GetObjectItem(json, "modeType");
    cJSON *modeObj = cJSON_GetObjectItem(json, "mode");
    cJSON *gnModeObj = cJSON_GetObjectItem(json, "gnMode");

    if (modeTypeObj == NULL || modeObj == NULL || gnModeObj == NULL ||
        modeTypeObj->type != cJSON_Number ||
        modeObj->type != cJSON_Object ||
        gnModeObj->type != cJSON_Object) {
        cJSON_Delete(json);
        return 0; // Invalid JSON format
    }

    message->modeType = modeTypeObj->valueint;

    if (!parseMode(modeObj, &message->mode) || !parseMode(gnModeObj, &message->gnMode)) {
        cJSON_Delete(json);
        return 0; // Failed to parse one of the modes
    }

    cJSON_Delete(json); // Free the JSON object
    return 1; // Success
}