#include <net/package.h>

#include <cutil/io/log.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

/**
 * Size of (additional) buffer in Package
 */
#define ADDITIONAL_BUFFER_SIZE 64

/**
 * Internal representation of a Package.
 */
struct _package {
    const PackageType *type; /**< Type, i.e., vtable */
    void *data;              /**< Generic data object to be sent/received */
    uint64_t hash;           /**< Hash of `data` */
    size_t namelen;          /**< Length of the type name */
    void *buf;               /**< Additional buffer */
};

Package *
Package_create(const PackageType *type, const void *params)
{
    Package *const pkg = malloc(sizeof *pkg);

    pkg->type = type;
    pkg->data = type->init(params);
    pkg->hash = type->hash(pkg->data);
    pkg->namelen = strlen(type->name) + 1;
    pkg->buf = malloc(ADDITIONAL_BUFFER_SIZE);

    return pkg;
}

void
Package_free(Package *pkg)
{
    CUTIL_RETURN_IF_NULL(pkg);

    const PackageType *const type = pkg->type;
    type->free(pkg->data);
    free(pkg->buf);

    free(pkg);
}

void
Package_set_data(Package *pkg, const void *data)
{
    const PackageType *const type = pkg->type;
    type->set(pkg->data, data);
    pkg->hash = type->hash(pkg->data);
}

void
Package_get_data(const Package *pkg, void *data)
{
    const PackageType *const type = pkg->type;
    type->get(pkg->data, data);
}

const PackageType *
Package_get_type(const Package *pkg)
{
    return pkg->type;
}

uint64_t
Package_get_hash(const Package *pkg)
{
    return pkg->hash;
}

bool
Package_verify(const Package *pkg)
{
    const PackageType *const type = pkg->type;
    const uint64_t hash = type->hash(pkg->data);
    return (hash == pkg->hash);
}

bool
Package_send(const Package *pkg, const Connection *conn)
{
#define SEND(DATA, SIZE)                                                       \
    do {                                                                       \
        if (Connection_send(conn, (DATA), (SIZE)) != (int64_t) (SIZE)) {       \
            cutil_log_error("Failed to send Package field '%s'", #DATA);       \
            return false;                                                      \
        }                                                                      \
    } while (0)

    const PackageType *const type = pkg->type;
    SEND(type->name, pkg->namelen);
    SEND(&pkg->hash, sizeof pkg->hash);
    if (!type->send(pkg->data, conn)) {
        cutil_log_error("Failed to send Package data");
        return false;
    }
    return true;

#undef SEND
}

bool
Package_receive(Package *pkg, const Connection *conn)
{
#define RECEIVE(BUF, SIZE)                                                     \
    do {                                                                       \
        if (Connection_receive(conn, (BUF), (SIZE)) != (int64_t) (SIZE)) {     \
            cutil_log_debug("Failed to receive Package field '%s'", #BUF);     \
            return false;                                                      \
        }                                                                      \
    } while (0)

    const PackageType *const type = pkg->type;
    RECEIVE(pkg->buf, pkg->namelen);
    if (strcmp(type->name, pkg->buf) != 0) {
        cutil_log_debug(
          "Type name not correct: expected '%s', was '%s'", type->name, pkg->buf
        );
        return false;
    }
    RECEIVE(&pkg->hash, sizeof pkg->hash);
    if (!type->recv(pkg->data, conn)) {
        cutil_log_error("Failed to receive Package data");
        return false;
    }
    return true;

#undef RECEIVE
}
