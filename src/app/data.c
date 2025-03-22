#include "data.h"

#include <stdlib.h>

#include "app.h"
#include "settings.h"

#include <data/image.h>

struct GraphicsData {
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
    if (gfxdata == NULL) {
        return;
    }

    ImageData_free(gfxdata->imgdata);

    free(gfxdata);
}

void
GraphicsData_register_action(GraphicsData *gfxdata, enum Key key)
{
    ImageData_register_action(gfxdata->imgdata, key);
}

int
GraphicsData_perform_action(GraphicsData *gfxdata, unsigned int mseconds)
{
    return ImageData_perform_action(gfxdata->imgdata, mseconds);
}

const float *
GraphicsData_get_pixel_data(const GraphicsData *gfxdata)
{
    return ImageData_get_pixel_data(gfxdata->imgdata);
}
