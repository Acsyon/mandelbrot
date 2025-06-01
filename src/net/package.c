#include <net/package.h>

#include <stdlib.h>
#include <string.h>

#include <cutil/log.h>

/**
 * Size of (additional) buffer in Package
 */
#define ADDITIONAL_BUFFER_SIZE 64

/**
 * Internal representation of a Package.
 */
struct Package {
    const PackageType *type; /**< Type, i.e., vtable */
    uint64_t capacity;       /**< Capacity of `data` */
    uint64_t size;           /**< Size of `data` */
    uint64_t hash;           /**< Hash of `data` */
    void *data;              /**< Buffer for data to be sent/received */
    size_t namelen;          /**< Length of the type name */
    void *buf;               /**< Additional buffer */
};

Package *
Package_create(const PackageType *type)
{
    Package *const pkg = malloc(sizeof *pkg);

    pkg->type = type;
    pkg->capacity = UINT64_C(0);
    pkg->size = UINT64_C(0);
    pkg->hash = UINT64_C(0);
    pkg->data = NULL;
    pkg->namelen = strlen(type->name) + 1;
    pkg->buf = malloc(ADDITIONAL_BUFFER_SIZE);

    return pkg;
}

void
Package_free(Package *pkg)
{
    if (pkg == NULL) {
        return;
    }

    free(pkg->data);
    free(pkg->buf);

    free(pkg);
}

const PackageType *
Package_get_type(const Package *pkg)
{
    return pkg->type;
}

uint64_t
Package_get_size(const Package *pkg)
{
    return pkg->size;
}

const void *
Package_get_data(const Package *pkg)
{
    return pkg->data;
}

uint64_t
Package_get_hash(const Package *pkg)
{
    return pkg->hash;
}

/**
 * Initializes `pkg` with `data`. Auxiliary internal function for to be used in
 * 'Package_init' and 'Package_move_init'.
 *
 * @param[in, out] pkg Package to be initialized
 * @param[in] data data to initialize package with
 */
static void
_package_init_aux(Package *pkg, void *data)
{
    if (pkg->data != NULL) {
        cutil_log_warn("Package has already been initialized");
        return;
    }

    const PackageType *const type = pkg->type;
    pkg->data = data;
    pkg->size = type->size(pkg->data);
    pkg->capacity = pkg->size;
    pkg->hash = type->hash(pkg->data);
}

void
Package_init(Package *pkg, const void *params)
{
    const PackageType *const type = pkg->type;
    void *const data = type->init(params);
    _package_init_aux(pkg, data);
}

void
Package_move_init(Package *pkg, void *data)
{
    const PackageType *const type = pkg->type;
    if (!type->supports_move) {
        cutil_log_warn(
          "'move_init' is not supported for Package type '%s'", type->name
        );
        return;
    }
    _package_init_aux(pkg, data);
}

void
Package_update(Package *pkg, const void *params)
{
    const PackageType *const type = pkg->type;
    if (type->update == NULL) {
        cutil_log_warn(
          "'update' is not supported for Package type '%s'", type->name
        );
        return;
    }

    type->update(pkg->data, pkg->size, params);
    pkg->hash = type->hash(pkg->data);
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
            cutil_log_error("Error sending Package field '%s'", #DATA);        \
            goto err;                                                          \
        }                                                                      \
    } while (0)

    const PackageType *const type = pkg->type;
    SEND(type->name, pkg->namelen);
    SEND(&pkg->hash, sizeof pkg->hash);
    SEND(&pkg->size, sizeof pkg->size);
    SEND(pkg->data, pkg->size);
    return true;
err:
    cutil_log_error("Failed to send Package correctly");
    return false;

#undef SEND
}

bool
Package_receive(Package *pkg, const Connection *conn)
{
#define RECEIVE(BUF, SIZE)                                                     \
    do {                                                                       \
        if (Connection_receive(conn, (BUF), (SIZE)) != (int64_t) (SIZE)) {     \
            cutil_log_debug("Error receiving Package field '%s'", #BUF);       \
            goto err;                                                          \
        }                                                                      \
    } while (0)

    const PackageType *const type = pkg->type;
    RECEIVE(pkg->buf, pkg->namelen);
    if (strcmp(type->name, pkg->buf) != 0) {
        cutil_log_debug(
          "Type name not correct: expected '%s', was '%s'", type->name, pkg->buf
        );
        goto err;
    }
    RECEIVE(&pkg->hash, sizeof pkg->hash);
    RECEIVE(&pkg->size, sizeof pkg->size);
    if (pkg->data == NULL) {
        pkg->data = malloc(pkg->size);
    }
    if (pkg->size > pkg->capacity) {
        pkg->capacity = pkg->size;
        pkg->data = realloc(pkg->data, pkg->capacity);
    }
    RECEIVE(pkg->data, pkg->size);
    return true;
err:
    cutil_log_error("Failed to receive Package correctly");
    return false;

#undef RECEIVE
}
