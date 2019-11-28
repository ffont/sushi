/*
 * Copyright 2017-2019 Modern Ancient Instruments Networked AB, dba Elk
 *
 * SUSHI is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Affero General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * SUSHI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License along with
 * SUSHI.  If not, see http://www.gnu.org/licenses/
 */

/**
 * @Brief Wrapper for LV2 plugins - Wrapper for LV2 plugins - extra features.
 * @copyright 2017-2019 Modern Ancient Instruments Networked AB, dba Elk, Stockholm
 */

#ifdef SUSHI_BUILD_WITH_LV2

#include "lv2_features.h"

#include "logging.h"

namespace sushi {
namespace lv2 {

    SUSHI_GET_LOGGER_WITH_MODULE_NAME("lv2");

Port* port_by_symbol(LV2Model* model, const char* sym)
{
    for (uint32_t i = 0; i < model->num_ports; ++i)
    {
        Port* port = model->ports[i].get();
        const LilvNode* port_sym = lilv_port_get_symbol(model->plugin, port->get_lilv_port());

        if (!strcmp(lilv_node_as_string(port_sym), sym))
        {
            return port;
        }
    }

    return nullptr;
}

int lv2_vprintf(LV2_Log_Handle handle,
                LV2_URID type,
                const char *fmt,
                va_list ap)
{
    LV2Model* model  = (LV2Model*)handle;
    if (type == model->urids.log_Trace && TRACE_OPTION)
    {
        SUSHI_LOG_WARNING("LV2 trace: {}", fmt);
    }
    else if (type == model->urids.log_Error)
    {
        SUSHI_LOG_ERROR("LV2 Error: {}", fmt);
    }
    else if (type == model->urids.log_Warning)
    {
        SUSHI_LOG_WARNING("LV2 warning: {}", fmt);
    }
    else if (type == model->urids.log_Entry)
    {
        SUSHI_LOG_WARNING("LV2 Entry: {}", fmt);
    }
    else if (type == model->urids.log_Note)
    {
        SUSHI_LOG_WARNING("LV2 Note: {}", fmt);
    }
    else if (type == model->urids.log_log)
    {
        SUSHI_LOG_WARNING("LV2 log: {}", fmt);
    }

    return 0;
}

int lv2_printf(LV2_Log_Handle handle,
               LV2_URID type,
               const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int ret = lv2_vprintf(handle, type, fmt, args);
    va_end(args);
    return ret;
}

}
}

#endif //SUSHI_BUILD_WITH_LV2