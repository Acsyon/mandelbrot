#include <util/json.h>

#include <cutil/io/log.h>
#include <cutil/std/stdlib.h>

#include <util/util.h>

Json *
Json_create(void)
{
    return cJSON_CreateObject();
}

Json *
Json_from_string(const char *str)
{
    cJSON *const json = cJSON_Parse(str);
    if (json == NULL) {
        const char *errptr = cJSON_GetErrorPtr();
        cutil_log_error("Error while reading JSON before '%s'", errptr);
    }
    return json;
}

void
Json_free(Json *json)
{
    cJSON_Delete(json);
}

char *
Json_to_string(const Json *json)
{
    return cJSON_Print(json);
}

bool
Json_compare(const Json *lhs, const Json *rhs, bool case_sensitive)
{
    return cJSON_Compare(lhs, rhs, case_sensitive);
}

#define DEFINE_JSON_TO_TYPE_FUNC(TYPE)                                         \
    int Json_elem_to_##TYPE(const Json *json, const char *elem, TYPE *ptr)     \
    {                                                                          \
        const cJSON *const item                                                \
          = cJSON_GetObjectItemCaseSensitive(json, elem);                      \
        if (item == NULL) {                                                    \
            return EXIT_FAILURE;                                               \
        }                                                                      \
        *ptr = (TYPE) cJSON_GetNumberValue(item);                              \
        return EXIT_SUCCESS;                                                   \
    }

DEFINE_JSON_TO_TYPE_FUNC(uint8_t)
DEFINE_JSON_TO_TYPE_FUNC(uint16_t)
DEFINE_JSON_TO_TYPE_FUNC(size_t)
DEFINE_JSON_TO_TYPE_FUNC(double)

int
Json_elem_to_str(const Json *json, const char *elem, char **ptr)
{
    const cJSON *const item = cJSON_GetObjectItemCaseSensitive(json, elem);
    if (item == NULL) {
        return EXIT_FAILURE;
    }
    *ptr = cJSON_GetStringValue(item);
    return EXIT_SUCCESS;
}

#define DEFINE_JSON_ADD_TYPE_FUNC(TYPE)                                        \
    int Json_add_##TYPE(Json *json, const char *elem, TYPE var)                \
    {                                                                          \
        cJSON *const item = cJSON_CreateNumber((double) var);                  \
        if (item == NULL) {                                                    \
            return EXIT_FAILURE;                                               \
        }                                                                      \
        const cJSON_bool status = cJSON_AddItemToObject(json, elem, item);     \
        if (!status) {                                                         \
            return EXIT_FAILURE;                                               \
        }                                                                      \
        return EXIT_SUCCESS;                                                   \
    }

DEFINE_JSON_ADD_TYPE_FUNC(uint8_t)
DEFINE_JSON_ADD_TYPE_FUNC(uint16_t)
DEFINE_JSON_ADD_TYPE_FUNC(size_t)
DEFINE_JSON_ADD_TYPE_FUNC(double)

int
Json_add_str(Json *json, const char *elem, const char *var)
{
    cJSON *const item = cJSON_CreateString(var);
    if (item == NULL) {
        return EXIT_FAILURE;
    }
    const cJSON_bool status = cJSON_AddItemToObject(json, elem, item);
    if (!status) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void
JsonUtil_fill_from_string(
  void *obj, const char *str, JsonUtilReadCallback *callback
)
{
    Json *const json = Json_from_string(str);
    if (json == NULL) {
        return;
    }
    callback(obj, json);
}

void
JsonUtil_fread(void *obj, FILE *in, JsonUtilReadCallback *callback)
{
    char *const str = Util_file_to_str(in);
    JsonUtil_fill_from_string(obj, str, callback);
    free(str);
}

void
JsonUtil_read(void *obj, const char *fname, JsonUtilReadCallback *callback)
{
    FILE *const in = fopen(fname, "r");
    if (in == NULL) {
        cutil_log_error("Cannot open file '%s'", fname);
        return;
    }
    JsonUtil_fread(obj, in, callback);
    fclose(in);
}

char *
JsonUtil_to_string(const void *obj, JsonUtilWriteCallback *callback)
{
    Json *const json = callback(obj);
    if (json == NULL) {
        return NULL;
    }
    char *const str = Json_to_string(json);
    Json_free(json);
    return str;
}

void
JsonUtil_fwrite(const void *obj, FILE *out, JsonUtilWriteCallback *callback)
{
    char *const str = JsonUtil_to_string(obj, callback);
    if (str == NULL) {
        return;
    }
    fprintf(out, "%s\n", str);
    free(str);
}

void
JsonUtil_write(
  const void *obj, const char *fname, JsonUtilWriteCallback *callback
)
{
    FILE *const out = fopen(fname, "w");
    if (out == NULL) {
        cutil_log_error("Cannot open file '%s'", fname);
        return;
    }
    JsonUtil_fwrite(obj, out, callback);
    fclose(out);
}
