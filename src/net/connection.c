#include "connection.h"

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include <unistd.h>

#include <arpa/inet.h>
#include <cutil/log.h>
#include <sys/socket.h>

#define MAXIMUM_CONNECTIONS 1

static bool _reuse_address = false;

struct Connection {
    enum ConnectionType type;
    int srv_sock;
    struct sockaddr_in srv_addr;
    int clt_sock;
    struct sockaddr_in clt_addr;
};

static Connection *
_connection_calloc(void)
{
    Connection *const conn = malloc(sizeof *conn);

    conn->srv_sock = -1;
    conn->srv_addr = (struct sockaddr_in) {0};
    conn->clt_sock = -1;
    conn->clt_addr = (struct sockaddr_in) {0};

    return conn;
}

void
Connection_close(Connection *conn)
{
    if (conn == NULL) {
        return;
    }

    close(conn->srv_sock);
    close(conn->clt_sock);

    free(conn);
}

Connection *
Connection_create_self(void)
{
    Connection *const conn = _connection_calloc();

    conn->type = CONNECTION_TYPE_SELF;

    return conn;
}

Connection *
Connection_bind(uint16_t port)
{
    Connection *const conn = _connection_calloc();

    conn->type = CONNECTION_TYPE_SERVER;

    conn->srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->srv_sock == -1) {
        cutil_log_error("Creation of socket failed\n");
        goto err;
    }

    conn->srv_addr.sin_family = AF_INET;
    conn->srv_addr.sin_addr.s_addr = INADDR_ANY;
    conn->srv_addr.sin_port = htons(port);

    if (_reuse_address) {
        int optval = 1;
        setsockopt(
          conn->srv_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)
        );
    }

    const struct sockaddr *const sockaddr = (struct sockaddr *) &conn->srv_addr;
    if (bind(conn->srv_sock, sockaddr, sizeof conn->srv_addr) < 0) {
        cutil_log_error("Binding socket failed\n");
        goto err;
    }

    if (listen(conn->srv_sock, MAXIMUM_CONNECTIONS) < 0) {
        cutil_log_error("Listening failed\n");
        goto err;
    }

    cutil_log_debug("Server listening on port %" PRIu16 "...\n", port);

    return conn;

err:
    Connection_close(conn);
    return NULL;
}

Connection *
Connection_connect(const char *addr, uint16_t port)
{
    Connection *const conn = _connection_calloc();

    conn->type = CONNECTION_TYPE_CLIENT;

    conn->srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->srv_sock == -1) {
        cutil_log_error("Creation of socket failed\n");
        goto err;
    }

    conn->srv_addr.sin_family = AF_INET;
    conn->srv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, addr, &conn->srv_addr.sin_addr) <= 0) {
        cutil_log_error("Invalid address/Address not supported\n");
        goto err;
    }

    const struct sockaddr *const sockaddr = (struct sockaddr *) &conn->srv_addr;
    if (connect(conn->srv_sock, sockaddr, sizeof conn->srv_addr) < 0) {
        cutil_log_error("Connection failed\n");
        goto err;
    }

    cutil_log_debug("Established connection to %s:%" PRIu16 "\n", addr, port);

    return conn;

err:
    Connection_close(conn);
    return NULL;
}

void
Connection_accept(Connection *conn)
{
    if (conn->type != CONNECTION_TYPE_SERVER) {
        cutil_log_warn("Only server can accept connetions\n");
        return;
    }

    cutil_log_debug("Waiting for client to connect...\n");

    struct sockaddr *const sockaddr = (struct sockaddr *) &conn->clt_addr;
    socklen_t addrlen = sizeof conn->clt_addr;
    conn->clt_sock = accept(conn->srv_sock, sockaddr, &addrlen);
    if (conn->clt_sock < 0) {
        cutil_log_error("Accept failed\n");
    }

    cutil_log_debug("Client connected\n");
}

static int
_connection_get_other_socket(Connection *conn)
{
    switch (conn->type) {
    case CONNECTION_TYPE_SERVER:
        return conn->clt_sock;
    case CONNECTION_TYPE_CLIENT:
        return conn->srv_sock;
    default:
        return -1;
    }
}

int64_t
Connection_send(Connection *conn, const void *buf, size_t size)
{
    if (conn->type == CONNECTION_TYPE_SELF) {
        return INT64_C(0);
    }
    const int sock = _connection_get_other_socket(conn);
    return send(sock, buf, size, 0);
}

int64_t
Connection_receive(Connection *conn, void *buf, size_t size)
{
    if (conn->type == CONNECTION_TYPE_SELF) {
        return INT64_C(0);
    }
    const int sock = _connection_get_other_socket(conn);
    for (;;) {
        const ssize_t avail = recv(sock, buf, size, MSG_PEEK);
        if (avail == (ssize_t) size) {
            const ssize_t res = recv(sock, buf, size, 0);
            if (res < -1) {
                cutil_log_error("recv failed or connection closed\n");
            }
            return res;
        } else if (avail == 0) {
            cutil_log_error("Connection closed by peer\n");
            return avail;
        } else if (avail == -1) {
            cutil_log_error("recv (MSG_PEEK) failed: %i\n", errno);
            return avail;
        }
    }
    return -1;
}

enum ConnectionType
Connection_get_type(const Connection *conn)
{
    return conn->type;
}

bool
Connection_set_reuse_address(bool reuse_address)
{
    const bool old_val = _reuse_address;
    _reuse_address = reuse_address;
    return old_val;
}
