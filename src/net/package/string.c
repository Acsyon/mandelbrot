#include <net/package/string.h>

#include <cutil/io/log.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

#include <util/json.h>

#define PACKAGE_TYPE_NAME "STRING"

typedef struct {
    size_t strlen;
    const char *str;
    size_t buflen;
    char *buf;
} _stringContainer;

static void *
_init(const void *params)
{
    _stringContainer *const sc = malloc(sizeof *sc);
    *sc = *(const _stringContainer *) params;
    return sc;
}

static void
_free(void *pkgdata)
{
    CUTIL_RETURN_IF_NULL(pkgdata);

    _stringContainer *const pkgsc = pkgdata;
    free(pkgsc->buf);

    free(pkgdata);
}

void
_set(void *pkgdata, const void *data)
{
    _stringContainer *const pkgsc = pkgdata;
    *pkgsc = *(const _stringContainer *) data;
}

void
_get(const void *pkgdata, void *data)
{
    _stringContainer *const sc = data;
    *sc = *(const _stringContainer *) pkgdata;
}

static uint64_t
_hash_djb33(const void *data)
{
    const _stringContainer *const sc = data;
    const char *str = sc->str;
    uint32_t hash = 5381;
    char c;
    while ((c = *str++)) {
        hash += (hash << 5);
        hash ^= c;
    }
    return hash;
}

static bool
_send(const void *pkgdata, const Connection *conn)
{
    const _stringContainer *const pkgsc = pkgdata;
    const uint64_t size = pkgsc->strlen;
    if (Connection_send(conn, &size, sizeof size) != (int64_t) sizeof size) {
        return false;
    }
    if (Connection_send(conn, pkgsc->str, size) != (int64_t) size) {
        return false;
    }
    return true;
}

static bool
_recv(void *pkgdata, const Connection *conn)
{
    _stringContainer *const pkgsc = pkgdata;
    uint64_t size = 0;
    if (Connection_receive(conn, &size, sizeof size) != (int64_t) sizeof size) {
        return false;
    }
    if (pkgsc->buflen < size) {
        pkgsc->buf = realloc(pkgsc->buf, size * sizeof *pkgsc->buf);
    }
    if (Connection_receive(conn, pkgsc->buf, size) != (int64_t) size) {
        return false;
    }
    return true;
}

static const PackageType PACKAGE_TYPE_STRING_OBJECT = {
  .name = PACKAGE_TYPE_NAME,
  .init = &_init,
  .free = &_free,
  .set = &_set,
  .get = &_get,
  .hash = &_hash_djb33,
  .send = &_send,
  .recv = &_recv,
};
const PackageType *const PACKAGE_TYPE_STRING = &PACKAGE_TYPE_STRING_OBJECT;

Package *
Package_from_string_move(char *str)
{
    const size_t len = strlen(str);
    return Package_from_string_move_num(len, str);
}

Package *
Package_from_string_move_num(size_t len, char *str)
{
    const size_t buflen = len + 1;
    return Package_create(
      PACKAGE_TYPE_STRING,
      &(const _stringContainer) {
        .strlen = buflen, .str = str, .buflen = buflen, .buf = str
      }
    );
}

Package *
Package_from_string_copy(const char *str)
{
    const size_t len = strlen(str);
    return Package_from_string_copy_num(len, str);
}

Package *
Package_from_string_copy_num(size_t len, const char *str)
{
    char *const cpy = cutil_strndup(str, len);
    return Package_from_string_move_num(len, cpy);
}

const char *
Package_to_string(const Package *pkg)
{
    if (Package_get_type(pkg) != PACKAGE_TYPE_STRING) {
        cutil_log_warn("Cannot get string from Package: wrong type");
        return NULL;
    }
    _stringContainer sc;
    Package_get_data(pkg, &sc);
    return sc.str;
}
