#include "unity.h"

#include <stdlib.h>
#include <string.h>

#include <util/json.h>

static void
_should_createJsonObject_when_callJsonCreate(void)
{
    /* Act */
    Json *const json = Json_create();

    /* Assert */
    TEST_ASSERT_NOT_NULL(json);

    /* Cleanup */
    Json_free(json);
}

static void
_should_parseJsonFromString_when_callJsonFromString(void)
{
    /* Arrange */
    const char *const json_str = "{\"key\": \"value\"}";

    /* Act */
    Json *const json = Json_from_string(json_str);

    /* Assert */
    TEST_ASSERT_NOT_NULL(json);

    /* Cleanup */
    Json_free(json);
}

static void
_should_returnNull_when_callJsonFromStringWithInvalidInput(void)
{
    const char *invalid_json_str = "{invalid_json}";
    Json *json = Json_from_string(invalid_json_str);
    TEST_ASSERT_NULL(json);
}

static void
_should_convertJsonToString_when_callJsonToString(void)
{
    /* Arrange */
    Json *const json = Json_create();
    Json_add_str(json, "key", "value");

    /* Act */
    char *const json_str = Json_to_string(json);

    /* Assert */
    TEST_ASSERT_NOT_NULL(json_str);
    TEST_ASSERT_EQUAL_STRING("{\n\t\"key\":\t\"value\"\n}", json_str);

    /* Cleanup */
    free(json_str);
    Json_free(json);
}

static void
_should_compareJsonObjects_when_callJsonCompare(void)
{
    /* Arrange */
    Json *const json1 = Json_create();
    Json_add_str(json1, "key", "value");
    Json *const json2 = Json_create();
    Json_add_str(json2, "key", "value");

    /* Act */
    const bool result = Json_compare(json1, json2, true);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    Json_free(json1);
    Json_free(json2);
}

static void
_should_readIntFromJson_when_callJsonElemToInt(void)
{
    /* Arrange */
    Json *json = Json_create();
    Json_add_int(json, "key", 42);
    int value = 0;

    /* Act */
    const int result = Json_elem_to_int(json, "key", &value);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(42, value);

    /* Cleanup */
    Json_free(json);
}

static void
_should_readStringFromJson_when_callJsonElemToStr(void)
{
    /* Arrange */
    Json *const json = Json_create();
    Json_add_str(json, "key", "value");
    char *value = NULL;

    /* Act */
    const int result = Json_elem_to_str(json, "key", &value);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_STRING("value", value);

    /* Cleanup */
    Json_free(json);
}

static void
_should_addIntToJson_when_callJsonAddInt(void)
{
    /* Arrange */
    Json *const json = Json_create();

    /* Act */
    const int result = Json_add_int(json, "key", 42);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);

    /* Cleanup */
    Json_free(json);
}

static void
_should_addStringToJson_when_callJsonAddStr(void)
{
    /* Arrange */
    Json *const json = Json_create();

    /* Act */
    const int result = Json_add_str(json, "key", "value");

    /* Assert */
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);

    /* Cleanup */
    Json_free(json);
}

static void
_callback(void *obj, const Json *json)
{
    char **value = obj;
    Json_elem_to_str(json, "key", value);
}

static void
_should_fillObjectFromJsonString_when_callJsonUtilFillFromString(void)
{
    /* Arrange */
    const char *json_str = "{\"key\": \"value\"}";
    char buffer[10] = {0};
    char *value = &buffer[0];

    /* Act */
    JsonUtil_fill_from_string(&value, json_str, &_callback);

    /* Cleanup */
    TEST_ASSERT_EQUAL_STRING("value", value);
}

void
setUp(void)
{}

void
tearDown(void)
{}

int
main(void)
{
    UNITY_BEGIN();

    RUN_TEST(_should_createJsonObject_when_callJsonCreate);
    RUN_TEST(_should_parseJsonFromString_when_callJsonFromString);
    RUN_TEST(_should_returnNull_when_callJsonFromStringWithInvalidInput);
    RUN_TEST(_should_convertJsonToString_when_callJsonToString);
    RUN_TEST(_should_compareJsonObjects_when_callJsonCompare);
    RUN_TEST(_should_readIntFromJson_when_callJsonElemToInt);
    RUN_TEST(_should_readStringFromJson_when_callJsonElemToStr);
    RUN_TEST(_should_addIntToJson_when_callJsonAddInt);
    RUN_TEST(_should_addStringToJson_when_callJsonAddStr);
    RUN_TEST(_should_fillObjectFromJsonString_when_callJsonUtilFillFromString);

    return UNITY_END();
}
