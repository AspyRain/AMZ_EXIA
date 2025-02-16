/*
  Copyright (c) 2009 Dave Gamble
 
  许可证声明：在此软件及其相关文档文件（“软件”）的副本或实质部分中，允许任何人不受限制地进行以下操作：
  使用、复制、修改、合并、发布、分发、再许可和/或销售软件的副本，并允许获得软件的人这样做，
  但需遵守以下条件：
 
  上述版权声明和本许可声明应包含在所有副本或实质部分的软件中。
 
  软件按“原样”提供，不附带任何明示或暗示的保证，包括但不限于对适销性、特定用途适用性和非侵权性的保证。
  在任何情况下，作者或版权持有人都不对任何索赔、损害或其他责任负责，无论是在合同、侵权行为还是其他行为中，
  由软件或其使用或其他交易引起或与之相关的。
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

/* cJSON 类型定义： */
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Number 3
#define cJSON_String 4
#define cJSON_Array 5
#define cJSON_Object 6
	
#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* cJSON 结构体定义： */
typedef struct cJSON {
    struct cJSON *next, *prev; /* next/prev 允许你遍历数组/对象链。或者，可以使用 GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *child;       /* 数组或对象项将有一个 child 指针，指向数组/对象中的项链表。 */

    int type;                  /* 项目的类型，如上定义。 */

    char *valuestring;         /* 项目字符串，如果类型为 cJSON_String */
    int valueint;              /* 项目数字，如果类型为 cJSON_Number */
    double valuedouble;        /* 项目数字，如果类型为 cJSON_Number */

    char *string;              /* 项目名称字符串，如果此项目是对象的子项或对象的子项列表中的项。 */
} cJSON;

/* cJSON 钩子结构体，用于自定义内存管理函数： */
typedef struct cJSON_Hooks {
    void *(*malloc_fn)(size_t sz); /* 自定义 malloc 函数 */
    void (*free_fn)(void *ptr);    /* 自定义 free 函数 */
} cJSON_Hooks;

/* 提供自定义的 malloc, realloc 和 free 函数给 cJSON */
extern void cJSON_InitHooks(cJSON_Hooks* hooks);

/* 提供一个 JSON 字符串块，返回一个可以查询的 cJSON 对象。使用完毕后调用 cJSON_Delete 释放内存。 */
extern cJSON *cJSON_Parse(const char *value);
/* 将 cJSON 实体渲染为文本，用于传输/存储。使用完毕后释放 char*。 */
extern char  *cJSON_Print(cJSON *item);
/* 将 cJSON 实体渲染为文本，用于传输/存储，不带任何格式化。使用完毕后释放 char*。 */
extern char  *cJSON_PrintUnformatted(cJSON *item);
/* 将 cJSON 实体渲染为文本，使用缓冲策略。prebuffer 是最终大小的猜测。猜测准确可以减少重新分配。fmt=0 表示不格式化，=1 表示格式化 */
extern char *cJSON_PrintBuffered(cJSON *item, int prebuffer, int fmt);
/* 删除 cJSON 实体及其所有子实体。 */
extern void   cJSON_Delete(cJSON *c);

/* 返回数组（或对象）中的项目数量。 */
extern int    cJSON_GetArraySize(cJSON *array);
/* 从数组 "array" 中获取第 "item" 个项。获取失败返回 NULL。 */
extern cJSON *cJSON_GetArrayItem(cJSON *array, int item);
/* 从对象 "object" 中获取名为 "string" 的项。不区分大小写。 */
extern cJSON *cJSON_GetObjectItem(cJSON *object, const char *string);

/* 用于分析解析失败的情况。返回指向解析错误的指针。通常在 cJSON_Parse() 返回 0 时定义。cJSON_Parse() 成功时为 0。 */
extern const char *cJSON_GetErrorPtr(void);
	
/* 这些函数创建适当类型的 cJSON 项。 */
extern cJSON *cJSON_CreateNull(void);
extern cJSON *cJSON_CreateTrue(void);
extern cJSON *cJSON_CreateFalse(void);
extern cJSON *cJSON_CreateBool(int b);
extern cJSON *cJSON_CreateNumber(double num);
extern cJSON *cJSON_CreateString(const char *string);
extern cJSON *cJSON_CreateArray(void);
extern cJSON *cJSON_CreateObject(void);

/* 这些实用函数创建包含 count 个项的数组。 */
extern cJSON *cJSON_CreateIntArray(const int *numbers, int count);
extern cJSON *cJSON_CreateFloatArray(const float *numbers, int count);
extern cJSON *cJSON_CreateDoubleArray(const double *numbers, int count);
extern cJSON *cJSON_CreateStringArray(const char **strings, int count);

/* 将项附加到指定的数组/对象。 */
extern void cJSON_AddItemToArray(cJSON *array, cJSON *item);
extern void cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
extern void cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item); /* 当字符串肯定是常量（即字面量或等效），并且肯定会在 cJSON 对象生存期间存在时使用此函数 */
/* 将项的引用附加到指定的数组/对象。当你要将现有的 cJSON 项添加到新的 cJSON 中，但不想破坏现有的 cJSON 时使用此函数。 */
extern void cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
extern void cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item);

/* 从数组/对象中移除/分离项。 */
extern cJSON *cJSON_DetachItemFromArray(cJSON *array, int which);
extern void   cJSON_DeleteItemFromArray(cJSON *array, int which);
extern cJSON *cJSON_DetachItemFromObject(cJSON *object, const char *string);
extern void   cJSON_DeleteItemFromObject(cJSON *object, const char *string);
	
/* 更新数组项。 */
extern void cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem); /* 将新项插入到指定位置，现有项向右移动。 */
extern void cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
extern void cJSON_ReplaceItemInObject(cJSON *object, const char *string, cJSON *newitem);

/* 复制一个 cJSON 项 */
extern cJSON *cJSON_Duplicate(cJSON *item, int recurse);
/* 复制将创建一个新的、与你传递的项完全相同的 cJSON 项，位于新的内存中，需要释放。
   当 recurse!=0 时，它将复制连接到该项的所有子项。
   复制返回的项的 next 和 prev 指针始终为零。 */

/* ParseWithOpts 允许你要求（并检查）JSON 以空字符终止，并检索解析结束的指针。 */
extern cJSON *cJSON_ParseWithOpts(const char *value, const char **return_parse_end, int require_null_terminated);

extern void cJSON_Minify(char *json);

/* 快速创建项目的宏。 */
#define cJSON_AddNullToObject(object, name)     cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object, name)     cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object, name)    cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddBoolToObject(object, name, b)  cJSON_AddItemToObject(object, name, cJSON_CreateBool(b))
#define cJSON_AddNumberToObject(object, name, n) cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object, name, s) cJSON_AddItemToObject(object, name, cJSON_CreateString(s))

/* 当分配整数值时，需要将其传播到 valuedouble。 */
#define cJSON_SetIntValue(object, val)          ((object) ? (object)->valueint = (object)->valuedouble = (val) : (val))
#define cJSON_SetNumberValue(object, val)       ((object) ? (object)->valueint = (object)->valuedouble = (val) : (val))

#ifdef __cplusplus
}
#endif

#endif