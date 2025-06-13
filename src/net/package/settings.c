#include <net/package/settings.h>

#include <cutil/io/log.h>
#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

#include <net/package/string.h>

Package *
Package_from_settings(const Settings *settings)
{
    char *const str = JsonUtil_to_string(settings, &Settings_to_Json_void);
    Package *const pkg = Package_from_string(str);
    return pkg;
}

void
Package_to_settings(const Package *pkg, Settings *settings)
{
    const char *const str = Package_to_string(pkg);
    CUTIL_RETURN_IF_NULL(str);
    JsonUtil_fill_from_string(settings, str, &Settings_fill_from_Json_void);
}
