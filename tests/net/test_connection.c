#include "unity.h"

#include <pthread.h>

#include <net/connection.h>
#include <util/sys.h>

#define ADDR "127.0.0.1"
#define PORT UINT16_C(12345)

static void
_should_setupSelfConnection_when_callConstructor(void)
{
    /* Arrange */
    /* Act */
    Connection *const conn = Connection_create_self();

    /* Assert */
    TEST_ASSERT_NOT_NULL(conn);

    /* Cleanup */
    Connection_close(conn);
}

static void
_should_setupServerConnectionCorrectly_when_providePort(void)
{
    /* Arrange */
    /* Act */
    Connection *const conn = Connection_bind(PORT);

    /* Assert */
    TEST_ASSERT_NOT_NULL(conn);

    /* Cleanup */
    Connection_close(conn);
}

static void
_should_failToSetupClientConnection_when_haveInvalidAddress(void)
{
    /* Arrange */
    /* Act */
    Connection *const conn = Connection_connect("1.2.3.4.5", PORT);

    /* Assert */
    TEST_ASSERT_NULL(conn);

    /* Cleanup */
    Connection_close(conn);
}

static void
_should_failToSetupClientConnection_when_haveNoServerConnection(void)
{
    /* Arrange */
    /* Act */
    Connection *const conn = Connection_connect(ADDR, PORT);

    /* Assert */
    TEST_ASSERT_NULL(conn);

    /* Cleanup */
    Connection_close(conn);
}

static void
_should_setupClientConnectionCorrectly_when_haveServerConnection(void)
{
    /* Arrange */
    Connection *const srv_conn = Connection_bind(PORT);

    /* Act */
    Connection *const clt_conn = Connection_connect(ADDR, PORT);

    /* Assert */
    TEST_ASSERT_NOT_NULL(clt_conn);

    /* Cleanup */
    Connection_close(srv_conn);
    Connection_close(clt_conn);
}

static void
_should_returnTypeCorrectly_when_haveDifferentConnections(void)
{
    /* Arrange */
    Connection *const slf_conn = Connection_create_self();
    Connection *const srv_conn = Connection_bind(PORT);
    Connection *const clt_conn = Connection_connect(ADDR, PORT);

    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL(CONNECTION_TYPE_SELF, Connection_get_type(slf_conn));
    TEST_ASSERT_EQUAL(CONNECTION_TYPE_SERVER, Connection_get_type(srv_conn));
    TEST_ASSERT_EQUAL(CONNECTION_TYPE_CLIENT, Connection_get_type(clt_conn));

    /* Cleanup */
    Connection_close(slf_conn);
    Connection_close(srv_conn);
    Connection_close(clt_conn);
}

static void *
_server_thread_function(void *arg)
{
    /* Arrange */
    Connection *const srv_conn = arg;
    Connection_accept(srv_conn);

    const size_t data_clt = 1;
    const size_t data_srv = 2;
    size_t buf_srv;

    /* Act */
    const int64_t snd_srv
      = Connection_send(srv_conn, &data_srv, sizeof data_srv);
    const int64_t rcv_srv
      = Connection_receive(srv_conn, &buf_srv, sizeof buf_srv);

    /* Assert */
    TEST_ASSERT_EQUAL_INT64(sizeof data_srv, snd_srv);
    TEST_ASSERT_EQUAL_INT64(sizeof buf_srv, rcv_srv);
    TEST_ASSERT_EQUAL_size_t(data_clt, buf_srv);

    return NULL;
}

static void *
_client_thread_function(void *arg)
{
    /* Arrange */
    Connection *const clt_conn = arg;

    const size_t data_clt = 1;
    const size_t data_srv = 2;
    size_t buf_clt;

    /* Act */
    const int64_t rcv_clt
      = Connection_receive(clt_conn, &buf_clt, sizeof buf_clt);
    const int64_t snd_clt
      = Connection_send(clt_conn, &data_clt, sizeof data_clt);

    /* Assert */
    TEST_ASSERT_EQUAL_INT64(sizeof buf_clt, rcv_clt);
    TEST_ASSERT_EQUAL_INT64(sizeof data_clt, snd_clt);
    TEST_ASSERT_EQUAL_size_t(data_srv, buf_clt);

    return NULL;
}

static void
_should_sendDataCorrectly_when_haveValidConnection(void)
{
    pthread_t server_thread;

    Connection *const srv_conn = Connection_bind(PORT);
    Connection *const clt_conn = Connection_connect(ADDR, PORT);

    /* Start server in a new thread */
    pthread_create(&server_thread, NULL, &_server_thread_function, srv_conn);

    /* Small delay to ensure server starts before client connects */
    msleep(100);

    /* Run client test */
    _client_thread_function(clt_conn);

    /* Wait for server to finish */
    pthread_join(server_thread, NULL);

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
    Connection_set_reuse_address(true);

    UNITY_BEGIN();

    RUN_TEST(_should_setupSelfConnection_when_callConstructor);
    RUN_TEST(_should_setupServerConnectionCorrectly_when_providePort);
    RUN_TEST(_should_failToSetupClientConnection_when_haveInvalidAddress);
    RUN_TEST(_should_failToSetupClientConnection_when_haveNoServerConnection);
    RUN_TEST(_should_setupClientConnectionCorrectly_when_haveServerConnection);
    RUN_TEST(_should_returnTypeCorrectly_when_haveDifferentConnections);
    RUN_TEST(_should_sendDataCorrectly_when_haveValidConnection);

    return UNITY_END();
}
