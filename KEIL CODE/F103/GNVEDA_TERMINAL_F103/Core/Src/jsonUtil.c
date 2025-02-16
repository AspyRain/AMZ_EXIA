#include "jsonUtil.h"
#include "string.h"
#include <rtthread.h>
#include <stdlib.h> // For malloc and free
// Helper function to parse a Mode from a cJSON object
static int parseMode(cJSON *modeObj, Mode *mode)
{
    cJSON *idObj = cJSON_GetObjectItem(modeObj, "id");
    cJSON *colorsObj = cJSON_GetObjectItem(modeObj, "colors");

    if (idObj == NULL || idObj->type != cJSON_Number ||
        colorsObj == NULL || (colorsObj->type != cJSON_String && colorsObj->type != cJSON_NULL))
    {
        rt_kprintf("Invalid JSON format for mode\n");
        return 0; // Invalid JSON format
    }

    mode->id = idObj->valueint;
    rt_kprintf("Parsed mode id: %d\n", mode->id);

    if (colorsObj->type == cJSON_String)
    {
        mode->colors = strdup(colorsObj->valuestring);
        if (mode->colors == NULL)
        {
            rt_kprintf("Memory allocation failed for colors\n");
            return 0; // Memory allocation failed
        }
        rt_kprintf("Parsed mode colors: %s\n", mode->colors);
    }
    else
    {
        mode->colors = NULL; // Handle null case
        rt_kprintf("Parsed mode colors: null\n");
    }

    return 1; // Success
}

// Function to free the dynamically allocated memory in a Mode
static void freeMode(Mode *mode)
{
    if (mode->colors != NULL)
    {
        free(mode->colors);
        mode->colors = NULL;
    }
}

// Function to parse the entire Message from a JSON string
int parseMessage(const char *jsonStr, Message *message)
{
    rt_kprintf(jsonStr);
    cJSON *json = cJSON_Parse(jsonStr);

    if (json == NULL)
    {
        rt_kprintf("Failed to parse JSON\n");
        return 0; // Failed to parse JSON
    }

    cJSON *modeTypeObj = cJSON_GetObjectItem(json, "modeType");
    cJSON *modeObj = cJSON_GetObjectItem(json, "mode");
    cJSON *gnModeObj = cJSON_GetObjectItem(json, "gnMode");

    if (modeTypeObj == NULL || modeObj == NULL || gnModeObj == NULL ||
        modeTypeObj->type != cJSON_Number ||
        modeObj->type != cJSON_Object ||
        gnModeObj->type != cJSON_Object)
    {
        rt_kprintf("Invalid JSON format\n");
        cJSON_Delete(json);
        return 0; // Invalid JSON format
    }

    message->modeType = modeTypeObj->valueint;
    rt_kprintf("Parsed modeType: %d\n", message->modeType);

    if (!parseMode(modeObj, &message->mode))
    {
        rt_kprintf("Failed to parse mode\n");
        // Free any partially allocated memory
        freeMode(&message->mode);
        cJSON_Delete(json);
        return 0; // Failed to parse mode
    }

    if (!parseMode(gnModeObj, &message->gnMode))
    {
        rt_kprintf("Failed to parse gnMode\n");
        // Free any partially allocated memory
        freeMode(&message->mode);
        freeMode(&message->gnMode);
        cJSON_Delete(json);
        return 0; // Failed to parse gnMode
    }

    cJSON_Delete(json); // Free the JSON object
    rt_kprintf("Parsed message successfully\n");
    return 1; // Success
}

// Function to create a cJSON object from a Mode
static cJSON *createModeJson(const Mode *mode)
{
    cJSON *modeObj = cJSON_CreateObject();
    if (modeObj == NULL)
    {
        rt_kprintf("Failed to create mode JSON object\n");
        return NULL;
    }

    cJSON_AddNumberToObject(modeObj, "id", mode->id);

    if (mode->colors != NULL)
    {
        cJSON_AddStringToObject(modeObj, "colors", mode->colors);
    }
    else
    {
        cJSON_AddNullToObject(modeObj, "colors");
    }

    return modeObj;
}

// Function to create a cJSON object from a Message
static cJSON *createMessageJson(const Message *message)
{
    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        rt_kprintf("Failed to create message JSON object\n");
        return NULL;
    }

    cJSON_AddNumberToObject(json, "modeType", message->modeType);

    cJSON *modeObj = createModeJson(&message->mode);
    if (modeObj == NULL)
    {
        cJSON_Delete(json);
        return NULL;
    }
    cJSON_AddItemToObject(json, "mode", modeObj);

    cJSON *gnModeObj = createModeJson(&message->gnMode);
    if (gnModeObj == NULL)
    {
        cJSON_Delete(json);
        cJSON_Delete(modeObj);
        return NULL;
    }
    cJSON_AddItemToObject(json, "gnMode", gnModeObj);

    return json;
}

// Function to generate a JSON string from a Message
char *generateMessageJson(const Message *message)
{
    cJSON *json = createMessageJson(message);
    if (json == NULL)
    {
        rt_kprintf("Failed to create message JSON\n");
        return NULL;
    }

    char *jsonStr = cJSON_PrintUnformatted(json);
    if (jsonStr == NULL)
    {
        rt_kprintf("Failed to print JSON string\n");
        cJSON_Delete(json);
        return NULL;
    }

    cJSON_Delete(json);
    return jsonStr;
}

void addStateToJSON(cJSON *root, const char *key, const STATE *state) {
    cJSON *stateObj = cJSON_CreateObject();
    cJSON_AddNumberToObject(stateObj, "state_id", state->state_id);
    cJSON_AddNumberToObject(stateObj, "mode_id", state->mode_id);
    cJSON_AddNumberToObject(stateObj, "gn_mode_id", state->gn_mode_id);
    cJSON_AddItemToObject(root, key, stateObj);
}

void parseModeJSON(cJSON *root, char *envs_head, const char *envs[], int size) {
    for (int i = 0; i < size; i++) {
        uint32_t color;
        char envName[30]; // 足够长的缓冲区来存储完整的环境变量名称
        snprintf(envName, sizeof(envName), "%s_%s", envs_head, envs[i]);
        ef_get_env_blob(envName, &color, 4, NULL);
        char color_str[11];
        snprintf(color_str, sizeof(color_str), "0x%06X", color);
        cJSON_AddStringToObject(root, envName, color_str);
    }
}

void parseInitData(cJSON *root, STATE *current)
{
    STATE standby;
    STATE day;
    STATE night;

    loadStateFromFlash(&standby, "state_standby");

    loadStateFromFlash(&day, "state_day");

    loadStateFromFlash(&night, "state_night");
    // 存入state配置
    addStateToJSON(root, "current", current);
    addStateToJSON(root, "standby", &standby);
    addStateToJSON(root, "day", &day);
    addStateToJSON(root, "night", &night);

    const char *envs[] = {
        "normal_color",
        "chest_color"
        "body_color",
        "wavy_color_1",
        "wavy_color_2",
    } ;

    parseModeJSON(root,"standby",envs,5);
    parseModeJSON(root,"day",envs,5);
    parseModeJSON(root,"night",envs,5);
}