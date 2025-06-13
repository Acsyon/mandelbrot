#include <net/package/string.h>

#include <cutil/io/log.h>
#include <cutil/std/string.h>

#include <util/json.h>

#define PACKAGE_TYPE_NAME "STRING"

static void *
_init(const void *params)
{
    return cutil_strdup(params);
}

static uint64_t
_size(const void *data)
{
    const char *const str = data;
    return strlen(str) + 1;
}

static uint64_t
_hash_djb33(const void *data)
{
    const char *str = data;
    uint32_t hash = 5381;
    char c;
    while ((c = *str++)) {
        hash += (hash << 5);
        hash ^= c;
    }
    return hash;
}

static const PackageType PACKAGE_TYPE_STRING_OBJECT = {
  .name = PACKAGE_TYPE_NAME,
  .init = &_init,
  .update = NULL,
  .size = &_size,
  .hash = &_hash_djb33,
  .supports_move = true,
};
const PackageType *const PACKAGE_TYPE_STRING = &PACKAGE_TYPE_STRING_OBJECT;

Package *
Package_from_string(char *str)
{
    Package *const pkg = Package_create(PACKAGE_TYPE_STRING);
    Package_move_init(pkg, str);
    return pkg;
}

const char *
Package_to_string(const Package *pkg)
{
    if (Package_get_type(pkg) != PACKAGE_TYPE_STRING) {
        cutil_log_warn("Cannot get string from Package: wrong type");
        return NULL;
    }
    return Package_get_data(pkg);
}
