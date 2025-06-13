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
_should_createPackage_when_provideType(void)
{
    /* Arrange */
    const PackageType *const type = PACKAGE_TYPE_STRING;

    /* Act */
    Package *const pkg = Package_create(type);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(type, Package_get_type(pkg));
    TEST_ASSERT_EQUAL_UINT64(0, Package_get_size(pkg));
    TEST_ASSERT_NULL(Package_get_data(pkg));
    TEST_ASSERT_EQUAL_UINT64(0, Package_get_hash(pkg));

    /* Cleanup */
    Package_free(pkg);
}

static void
_should_initStringData_when_useInit(void)
{
    /* Arrange */
    const char *const assert_str = ASSERT_STR;
    const uint64_t assert_size = sizeof ASSERT_STR;
    const PackageType *const type = PACKAGE_TYPE_STRING;
    Package *const pkg = Package_create(type);

    /* Act */
    Package_init(pkg, assert_str);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(assert_size, Package_get_size(pkg));
    TEST_ASSERT_EQUAL_STRING(assert_str, Package_get_data(pkg));
    TEST_ASSERT_NOT_EQUAL_UINT64(0, Package_get_hash(pkg));

    /* Cleanup */
    Package_free(pkg);
}

static void
_should_initStringData_when_useMoveInit(void)
{
    /* Arrange */
    char *const assert_str = cutil_strdup(ASSERT_STR);
    const uint64_t assert_size = sizeof ASSERT_STR;
    const PackageType *const type = PACKAGE_TYPE_STRING;
    Package *const pkg = Package_create(type);

    /* Act */
    Package_move_init(pkg, assert_str);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(assert_size, Package_get_size(pkg));
    TEST_ASSERT_EQUAL_STRING(assert_str, Package_get_data(pkg));
    TEST_ASSERT_NOT_EQUAL_UINT64(0, Package_get_hash(pkg));

    /* Cleanup */
    Package_free(pkg);
}

static void
_should_verifyCorrectly_when_dataIsValid(void)
{
    /* Arrange */
    const char *const assert_str = ASSERT_STR;
    const PackageType *const type = PACKAGE_TYPE_STRING;
    Package *const pkg = Package_create(type);

    /* Act */
    Package_init(pkg, assert_str);

    /* Assert */
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
    const PackageType *const type = PACKAGE_TYPE_STRING;
    Package *const pkg = Package_create(type);
    Package_init(pkg, assert_str);

    /* Act */
    const bool succesful = Package_send(pkg, srv_conn);

    /* Assert */
    TEST_ASSERT_TRUE(succesful);

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
    const uint64_t assert_size = sizeof ASSERT_STR;
    const PackageType *const type = PACKAGE_TYPE_STRING;
    Package *const pkg = Package_create(type);

    /* Act */
    const bool succesful = Package_receive(pkg, clt_conn);

    /* Assert */
    TEST_ASSERT_TRUE(succesful);
    TEST_ASSERT_EQUAL_UINT64(assert_size, Package_get_size(pkg));
    TEST_ASSERT_EQUAL_STRING(assert_str, Package_get_data(pkg));
    TEST_ASSERT_NOT_EQUAL_UINT64(0, Package_get_hash(pkg));

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

    RUN_TEST(_should_createPackage_when_provideType);
    RUN_TEST(_should_initStringData_when_useInit);
    RUN_TEST(_should_initStringData_when_useMoveInit);
    RUN_TEST(_should_verifyCorrectly_when_dataIsValid);
    RUN_TEST(_should_sendDataCorrectly_when_haveValidConnection);

    connection_global_cleanup();

    return UNITY_END();
}
