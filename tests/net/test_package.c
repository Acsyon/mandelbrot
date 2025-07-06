#include "unity.h"

#include <threads.h>

#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

#include <net/connection.h>
#include <net/package.h>
#include <net/package/string.h>
#include <util/sys.h>

#define ADDR "127.0.0.1"
#define PORT UINT16_C(12345)

#define ASSERT_STR "TEST data String"

struct _testData {
    const void *data;
};

static void *
_init(const void *params)
{
    struct _testData *const data = malloc(sizeof *data);
    data->data = params;
    return data;
}

static void
_free(void *pkgdata)
{
    CUTIL_RETURN_IF_NULL(pkgdata);

    free(pkgdata);
}

void
_set(void *pkgdata, const void *data)
{
    struct _testData *const pkgtd = pkgdata;
    pkgtd->data = data;
}

void
_get(const void *pkgdata, void *data)
{
    struct _testData *const td = data;
    *td = *(const struct _testData *) pkgdata;
}

static uint64_t
_hash(const void *data)
{
    const struct _testData *const td = data;
    return (uint64_t) td->data;
}

static bool
_send(const void *pkgdata, const Connection *conn)
{
    const struct _testData *const pkgtd = pkgdata;
    const uint64_t ptr = (uint64_t) pkgtd->data;
    if (Connection_send(conn, &ptr, sizeof ptr) != (int64_t) sizeof ptr) {
        return false;
    }
    return true;
}

static bool
_recv(void *pkgdata, const Connection *conn)
{
    struct _testData *const pkgtd = pkgdata;
    uint64_t ptr = 0;
    if (Connection_receive(conn, &ptr, sizeof ptr) != (int64_t) sizeof ptr) {
        return false;
    }
    pkgtd->data = (const void *) ptr;
    return true;
}

static const PackageType PACKAGE_TYPE_TEST_OBJECT = {
  .name = "TEST",
  .init = &_init,
  .free = &_free,
  .set = &_set,
  .get = &_get,
  .hash = &_hash,
  .send = &_send,
  .recv = &_recv,
};
const PackageType *const PACKAGE_TYPE_TEST = &PACKAGE_TYPE_TEST_OBJECT;

static const void *
_package_return_data(Package *pkg)
{
    struct _testData pkgtd;
    Package_get_data(pkg, &pkgtd);
    return pkgtd.data;
}

static void
_should_createPackage_when_provideType(void)
{
    /* Arrange */
    const PackageType *const type = PACKAGE_TYPE_TEST;

    /* Act */
    Package *const pkg = Package_create(type, NULL);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(type, Package_get_type(pkg));
    TEST_ASSERT_NULL(_package_return_data(pkg));
    TEST_ASSERT_EQUAL_UINT64(0, Package_get_hash(pkg));
    TEST_ASSERT_TRUE(Package_verify(pkg));

    /* Cleanup */
    Package_free(pkg);
}

static void
_should_createPackage_when_provideTypeAndData(void)
{
    /* Arrange */
    const char *const assert_str = ASSERT_STR;
    const PackageType *const type = PACKAGE_TYPE_TEST;

    /* Act */
    Package *const pkg = Package_create(type, assert_str);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(assert_str, _package_return_data(pkg));
    TEST_ASSERT_EQUAL_UINT64(assert_str, Package_get_hash(pkg));
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
    const PackageType *const type = PACKAGE_TYPE_TEST;
    Package *const pkg = Package_create(type, assert_str);

    /* Act */
    const bool was_successful = Package_send(pkg, srv_conn);

    /* Assert */
    TEST_ASSERT_TRUE(was_successful);

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
    const PackageType *const type = PACKAGE_TYPE_TEST;
    Package *const pkg = Package_create(type, assert_str);

    /* Act */
    const bool was_successful = Package_receive(pkg, clt_conn);

    /* Assert */
    TEST_ASSERT_TRUE(was_successful);
    TEST_ASSERT_EQUAL_PTR(assert_str, _package_return_data(pkg));
    TEST_ASSERT_EQUAL_UINT64(assert_str, Package_get_hash(pkg));
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

    RUN_TEST(_should_createPackage_when_provideType);
    RUN_TEST(_should_createPackage_when_provideTypeAndData);
    RUN_TEST(_should_sendDataCorrectly_when_haveValidConnection);

    connection_global_cleanup();

    return UNITY_END();
}
