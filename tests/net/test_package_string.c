#include "unity.h"

#include <threads.h>

#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <net/connection.h>
#include <net/package.h>
#include <net/package/string.h>
#include <util/sys.h>

#define ADDR "127.0.0.1"
#define PORT UINT16_C(12345)

#define ASSERT_STR "TEST data String"

static void
_should_createPackage_when_moveString(void)
{
    /* Arrange */
    char *const assert_str = cutil_strdup(ASSERT_STR);

    /* Act */
    Package *const pkg = Package_from_string_move(assert_str);

    /* Assert */
    const char *const str = Package_to_string(pkg);
    TEST_ASSERT_TRUE(assert_str == str);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);
    TEST_ASSERT_NOT_EQUAL_UINT64(0, Package_get_hash(pkg));
    TEST_ASSERT_TRUE(Package_verify(pkg));

    /* Cleanup */
    Package_free(pkg);
}

static void
_should_createPackage_when_copyString(void)
{
    /* Arrange */
    const char *const assert_str = ASSERT_STR;

    /* Act */
    Package *const pkg = Package_from_string_copy(assert_str);

    /* Assert */
    const char *const str = Package_to_string(pkg);
    TEST_ASSERT_FALSE(assert_str == str);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);
    TEST_ASSERT_NOT_EQUAL_UINT64(0, Package_get_hash(pkg));
    TEST_ASSERT_TRUE(Package_verify(pkg));

    /* Cleanup */
    Package_free(pkg);
}

static int
_server_thread_function(void *arg)
{
    /* Arrange */
    Connection *const srv_conn = arg;
    Connection_accept(srv_conn);

    const char *const assert_str = ASSERT_STR;
    Package *const pkg = Package_from_string_copy(assert_str);

    /* Act */
    const bool successful = Package_send(pkg, srv_conn);

    /* Assert */
    TEST_ASSERT_TRUE(successful);

    /* Cleanup */
    Package_free(pkg);

    return EXIT_SUCCESS;
}

static int
_client_thread_function(void *arg)
{
    /* Arrange */
    Connection *const clt_conn = arg;

    const char *const assert_str = ASSERT_STR;
    Package *const pkg = Package_from_string_copy(assert_str);

    /* Act */
    const bool successful = Package_receive(pkg, clt_conn);

    /* Assert */
    TEST_ASSERT_TRUE(successful);
    const char *const str = Package_to_string(pkg);
    TEST_ASSERT_FALSE(assert_str == str);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);
    TEST_ASSERT_NOT_EQUAL_UINT64(0, Package_get_hash(pkg));
    TEST_ASSERT_TRUE(Package_verify(pkg));

    /* Cleanup */
    Package_free(pkg);

    return EXIT_SUCCESS;
}

static void
_should_sendDataCorrectly_when_haveValidConnection(void)
{
    thrd_t server_thread;

    Connection *const srv_conn = Connection_bind(PORT);
    Connection *const clt_conn = Connection_connect(ADDR, PORT);

    /* Start server in a new thread */
    thrd_create(&server_thread, &_server_thread_function, srv_conn);

    /* Small delay to ensure server starts before client connects */
    msleep(100);

    /* Run client test */
    _client_thread_function(clt_conn);

    /* Wait for server to finish */
    thrd_join(server_thread, NULL);

    /* Cleanup */
    Connection_close(srv_conn);
    Connection_close(clt_conn);
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
    connection_global_startup();

    Connection_set_reuse_address(true);

    UNITY_BEGIN();

    RUN_TEST(_should_createPackage_when_moveString);
    RUN_TEST(_should_createPackage_when_copyString);
    RUN_TEST(_should_sendDataCorrectly_when_haveValidConnection);

    connection_global_cleanup();

    return UNITY_END();
}
