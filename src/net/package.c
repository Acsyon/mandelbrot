#include <net/package.h>

#include <stdlib.h>

#include <cutil/log.h>

struct Package {
    const PackageType *type;
    uint64_t size;
    uint64_t hash;
    void *data;
};

Package *
Package_create(const PackageType *type)
{
    Package *const pkg = malloc(sizeof *pkg);

    pkg->type = type;
    pkg->size = UINT64_C(0);
    pkg->hash = UINT64_C(0);
    pkg->data = NULL;

    return pkg;
}

void
Package_free(Package *pkg)
{
    if (pkg == NULL) {
        return;
    }

    free(pkg->data);

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

static void
_package_init_aux(Package *pkg, void *data)
{
    if (pkg->data != NULL) {
        cutil_log_warn("Package has already been initialized");
    }

    const PackageType *const type = pkg->type;
    pkg->data = data;
    pkg->size = type->size(pkg->data);
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
