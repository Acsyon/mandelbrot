#include "unity.h"

#include <app/settings.h>

static const Settings ASSERT_SETTINGS_1 = {0};
static const Settings ASSERT_SETTINGS_2 = {
  .width = UINT16_C(1),
  .height = UINT16_C(2),
  .max_re = 4.0F,
  .min_re = 3.0F,
  .cntr_im = 5.0F,
  .max_itrs = UINT16_C(4),
  .num_chnks_re = UINT8_C(7),
  .num_chnks_im = UINT8_C(8),
  .zoom_fac = 9.0F,
  .fps = UINT16_C(10),
  .palette_idx = UINT8_C(11),
  .trip_mode = UINT8_C(12),
  .view_file = "13",
  .address = "14",
  .port = UINT16_C(15),
};
static const Settings ASSERT_SETTINGS_3 = {
  .width = UINT16_C(1),
  .max_re = 2.0F,
  .min_re = -1.0F,
  .cntr_im = -3.0F,
  0,
  .view_file = "test.dat",
};

static const char *const SETTINGS_DEFAULT_JSON
  = "{\"width\":800,\"height\":600,\"max_re\":1,\"min_re\":-2,\"cntr_im\":0,"
    "\"max_itrs\":500,\"num_chnks_re\":20,\"num_chnks_im\":20,\"zoom_fac\":0.5,"
    "\"fps\":30,\"palette_idx\":4,\"trip_mode\":0,\"view_file\":\"view.json\","
    "\"address\":\"127.0.0.1\",\"port\":10101}";
static const char *const SETTINGS_1_JSON = "{}";
static const char *const SETTINGS_2_JSON
  = "{\"width\":1,\"height\":2,\"max_re\":4,\"min_re\":3,\"cntr_im\":5,\"max_"
    "itrs\":4,\"num_chnks_re\":7,\"num_chnks_im\":8,\"zoom_fac\":9,\"fps\":10,"
    "\"palette_idx\":11,\"trip_mode\":12,\"view_file\":\"13\",\"address\":"
    "\"14\",\"port\":15}";
static const char *const SETTINGS_3_JSON
  = "{\"width\":1,\"max_re\":2,\"min_re\":-1,\"cntr_im\":-3,\"view_file\":"
    "\"test.dat\"}";

static void
_test_assert_equal_settings(const Settings *lhs, const Settings *rhs)
{
    TEST_ASSERT_EQUAL_INT(lhs->width, rhs->width);
    TEST_ASSERT_EQUAL_INT(lhs->height, rhs->height);
    TEST_ASSERT_EQUAL_FLOAT(lhs->max_re, rhs->max_re);
    TEST_ASSERT_EQUAL_FLOAT(lhs->min_re, rhs->min_re);
    TEST_ASSERT_EQUAL_FLOAT(lhs->cntr_im, rhs->cntr_im);
    TEST_ASSERT_EQUAL_UINT16(lhs->max_itrs, rhs->max_itrs);
    TEST_ASSERT_EQUAL_INT(lhs->num_chnks_re, rhs->num_chnks_re);
    TEST_ASSERT_EQUAL_INT(lhs->num_chnks_im, rhs->num_chnks_im);
    TEST_ASSERT_EQUAL_FLOAT(lhs->zoom_fac, rhs->zoom_fac);
    TEST_ASSERT_EQUAL_UINT16(lhs->fps, rhs->fps);
    TEST_ASSERT_EQUAL_INT(lhs->palette_idx, rhs->palette_idx);
    TEST_ASSERT_EQUAL_INT(lhs->trip_mode, rhs->trip_mode);
    TEST_ASSERT_EQUAL_STRING(lhs->view_file, rhs->view_file);
}

static void
_should_createDefaultSettings_when_callCreate(void)
{
    /* Arrange */
    /* Act */
    Settings *const settings = Settings_create();

    /* Assert */
    TEST_ASSERT_NOT_NULL(settings);
    _test_assert_equal_settings(DEFAULT_SETTINGS, settings);

    /* Cleanup */
    Settings_free(settings);
}

static void
_should_duplicateSettingsCorrectly(void)
{
    /* Act */
    Settings *const dup = Settings_duplicate(DEFAULT_SETTINGS);
    Settings *const dup1 = Settings_duplicate(&ASSERT_SETTINGS_1);
    Settings *const dup2 = Settings_duplicate(&ASSERT_SETTINGS_2);
    Settings *const dup3 = Settings_duplicate(&ASSERT_SETTINGS_3);

    /* Assert */
    _test_assert_equal_settings(DEFAULT_SETTINGS, dup);
    _test_assert_equal_settings(&ASSERT_SETTINGS_1, dup1);
    _test_assert_equal_settings(&ASSERT_SETTINGS_2, dup2);
    _test_assert_equal_settings(&ASSERT_SETTINGS_3, dup3);

    /* Cleanup */
    Settings_free(dup);
    Settings_free(dup1);
    Settings_free(dup2);
    Settings_free(dup3);
}

static void
_should_returnCorrectViewDetails(void)
{
    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_FLOAT(-0.5, Settings_get_center_real(DEFAULT_SETTINGS));
    TEST_ASSERT_EQUAL_FLOAT(0.0, Settings_get_center_real(&ASSERT_SETTINGS_1));
    TEST_ASSERT_EQUAL_FLOAT(3.5, Settings_get_center_real(&ASSERT_SETTINGS_2));
    TEST_ASSERT_EQUAL_FLOAT(0.5, Settings_get_center_real(&ASSERT_SETTINGS_3));

    TEST_ASSERT_EQUAL_FLOAT(0.0, Settings_get_center_imag(DEFAULT_SETTINGS));
    TEST_ASSERT_EQUAL_FLOAT(0.0, Settings_get_center_imag(&ASSERT_SETTINGS_1));
    TEST_ASSERT_EQUAL_FLOAT(5.0, Settings_get_center_imag(&ASSERT_SETTINGS_2));
    TEST_ASSERT_EQUAL_FLOAT(-3.0, Settings_get_center_imag(&ASSERT_SETTINGS_3));

    TEST_ASSERT_EQUAL_FLOAT(
      0.00375, Settings_get_units_per_pixel(DEFAULT_SETTINGS)
    );
    TEST_ASSERT_EQUAL_FLOAT(
      NAN, Settings_get_units_per_pixel(&ASSERT_SETTINGS_1)
    );
    TEST_ASSERT_EQUAL_FLOAT(
      1.0, Settings_get_units_per_pixel(&ASSERT_SETTINGS_2)
    );
    TEST_ASSERT_EQUAL_FLOAT(
      3.0, Settings_get_units_per_pixel(&ASSERT_SETTINGS_3)
    );
}

static void
_should_createCorrectSettings_when_useReadFromJson(void)
{
    /* Arrange */
    Settings dup = {0};
    Settings dup1 = {0};
    Settings dup2 = {0};
    Settings dup3 = {0};

    Json *const json = Json_from_string(SETTINGS_DEFAULT_JSON);
    Json *const json1 = Json_from_string(SETTINGS_1_JSON);
    Json *const json2 = Json_from_string(SETTINGS_2_JSON);
    Json *const json3 = Json_from_string(SETTINGS_3_JSON);

    /* Act */
    Settings_fill_from_Json(&dup, json);
    Settings_fill_from_Json(&dup1, json1);
    Settings_fill_from_Json(&dup2, json2);
    Settings_fill_from_Json(&dup3, json3);

    /* Assert */
    _test_assert_equal_settings(DEFAULT_SETTINGS, &dup);
    _test_assert_equal_settings(&ASSERT_SETTINGS_1, &dup1);
    _test_assert_equal_settings(&ASSERT_SETTINGS_2, &dup2);
    _test_assert_equal_settings(&ASSERT_SETTINGS_3, &dup3);

    /* Cleanup */
    Json_free(json);
    Json_free(json1);
    Json_free(json2);
    Json_free(json3);
}

/* In this test, we only use JSON strings which are fully specified. Otherwise,
 * 'Json_compare' will return false and I don't want to write a comparison
 * function that ignores empty fields just for these tests...*/
static void
_should_createCorrectJson_when_useCreatefromSettings(void)
{
    /* Arrange */
    Json *const assert_json = Json_from_string(SETTINGS_DEFAULT_JSON);
    Json *const assert_json2 = Json_from_string(SETTINGS_2_JSON);

    /* Act */
    Json *const json = Settings_to_Json(DEFAULT_SETTINGS);
    Json *const json2 = Settings_to_Json(&ASSERT_SETTINGS_2);

    /* Assert */
    TEST_ASSERT_TRUE(Json_compare(assert_json, json, true));
    TEST_ASSERT_TRUE(Json_compare(assert_json2, json2, true));

    /* Cleanup */
    Json_free(json);
    Json_free(json2);

    Json_free(assert_json);
    Json_free(assert_json2);
}

static void
_should_duplicateSettingsCorrectly_when_useJsonConversion(void)
{
    /* Arrange */
    Settings dup = {0};
    Settings dup1 = {0};
    Settings dup2 = {0};
    Settings dup3 = {0};

    /* Act */
    Json *const json = Settings_to_Json(DEFAULT_SETTINGS);
    Json *const json1 = Settings_to_Json(&ASSERT_SETTINGS_1);
    Json *const json2 = Settings_to_Json(&ASSERT_SETTINGS_2);
    Json *const json3 = Settings_to_Json(&ASSERT_SETTINGS_3);

    Settings_fill_from_Json(&dup, json);
    Settings_fill_from_Json(&dup1, json1);
    Settings_fill_from_Json(&dup2, json2);
    Settings_fill_from_Json(&dup3, json3);

    /* Assert */
    _test_assert_equal_settings(DEFAULT_SETTINGS, &dup);
    _test_assert_equal_settings(&ASSERT_SETTINGS_1, &dup1);
    _test_assert_equal_settings(&ASSERT_SETTINGS_2, &dup2);
    _test_assert_equal_settings(&ASSERT_SETTINGS_3, &dup3);

    /* Cleanup */
    Json_free(json);
    Json_free(json1);
    Json_free(json2);
    Json_free(json3);
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

    RUN_TEST(_should_createDefaultSettings_when_callCreate);
    RUN_TEST(_should_duplicateSettingsCorrectly);
    RUN_TEST(_should_returnCorrectViewDetails);
    RUN_TEST(_should_createCorrectSettings_when_useReadFromJson);
    RUN_TEST(_should_createCorrectJson_when_useCreatefromSettings);
    RUN_TEST(_should_duplicateSettingsCorrectly_when_useJsonConversion);

    return UNITY_END();
}
