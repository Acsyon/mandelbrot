/* net/connection.h
 *
 * Header for network connections
 *
 */

#ifndef MANDELBROT_NET_CONNECTION_H_INCLUDED
#define MANDELBROT_NET_CONNECTION_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Enumerator for different types of connections
 */
enum ConnectionType {
    CONNECTION_TYPE_SERVER,
    CONNECTION_TYPE_CLIENT,
    CONNECTION_TYPE_SELF,
};

/**
 * Opaque Connection object
 */
typedef struct Connection Connection;

/**
 * Closes the connection `conn`.
 *
 * @param[in] conn Connection to close
 */
void
Connection_close(Connection *conn);

/**
 * Creates auxiliary Connection to support classic mode.
 *
 * @return newly created Connection to self
 */
Connection *
Connection_create_self(void);

/**
 * Creates a Connection bound to port `port`.
 *
 * @param[in] port port to listen to
 *
 * @return newly created Connection bound to address
 */
Connection *
Connection_bind(uint16_t port);

/**
 * Creates a Connection connected to address `addr` and port `port`.
 *
 * @param[in] addr address to connect to
 * @param[in] port port to listen to
 *
 * @return newly created Connection bound to address
 */
Connection *
Connection_connect(const char *addr, uint16_t port);

/**
 * Awaits a Connection.
 *
 * @param[in] conn Connection to await connection for
 */
void
Connection_accept(Connection *conn);

/**
 * Sends `size` bytes of data stored in `buf` via Connection `conn`.
 *
 * @param[in] conn Connection to send data via
 * @param[in] buf buffer to send data of
 * @param[in] size number of bytes to send
 */
int64_t
Connection_send(Connection *conn, const void *buf, size_t size);

/**
 * Receives `size` bytes of data via Connection `conn` and stores it in `buf`.
 *
 * @param[in] conn Connection to send data via
 * @param[in] buf buffer to write data to
 * @param[in] size number of bytes to write
 */
int64_t
Connection_receive(Connection *conn, void *buf, size_t size);

/**
 * Returns the ConnectionType of Connection `conn`.
 *
 * @param[in] conn Connection to return type of
 *
 * @return ConnectionType of Connection `conn`
 */
enum ConnectionType
Connection_get_type(const Connection *conn);

/**
 * Sets value of `reuse_address`, i.e., if TIME_WAIT should be ignored when
 * binding sockets (should only be activated to make unit tests pass in short
 * succession).
 *
 * @param[in] reuse_address new value of `reuse_address`
 *
 * @return old value of `reuse_address`
 */
bool
Connection_set_reuse_address(bool reuse_address);

#endif /* MANDELBROT_NET_CONNECTION_H_INCLUDED */
