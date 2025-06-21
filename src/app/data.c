#include <app/data.h>

#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

#include <app/app.h>
#include <app/settings.h>
#include <data/image.h>

struct _graphicsData {
    ImageData *imgdata;
};

GraphicsData *
_graphicsData_create(const Settings *settings)
{
    GraphicsData *const gfxdata = malloc(sizeof *gfxdata);

    gfxdata->imgdata = ImageData_create(settings);

    return gfxdata;
}

GraphicsData *
GraphicsData_app_init(void)
{
    GraphicsData *const gfxdata = App_get_graphics_data();
    if (gfxdata != NULL) {
        return gfxdata;
    }
    const Settings *const settings = App_get_settings();
    return _graphicsData_create(settings);
}

void
GraphicsData_free(GraphicsData *gfxdata)
{
    CUTIL_RETURN_IF_NULL(gfxdata);

    ImageData_free(gfxdata->imgdata);

    free(gfxdata);
}

void
GraphicsData_register_action(GraphicsData *gfxdata, enum Key key)
{
    ImageData_register_action(gfxdata->imgdata, key);
}

void
GraphicsData_resume_action(GraphicsData *gfxdata, unsigned int mseconds)
{
    ImageData_resume_action(gfxdata->imgdata, mseconds);
}

void
GraphicsData_perform_action(GraphicsData *gfxdata, enum Key key)
{
    ImageData_perform_action(gfxdata->imgdata, key);
}

const float *
GraphicsData_get_pixel_data(const GraphicsData *gfxdata)
{
    return ImageData_get_pixel_data(gfxdata->imgdata);
}

bool
GraphicsData_has_changed(const GraphicsData *gfxdata)
{
    return ImageData_has_changed(gfxdata->imgdata);
}

bool
GraphicsData_is_working(const GraphicsData *gfxdata)
{
    return ImageData_is_working(gfxdata->imgdata);
}
