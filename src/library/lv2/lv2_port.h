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
 * SUSHI. If not, see http://www.gnu.org/licenses/
 */

/**
 * @Brief Wrapper for LV2 plugins - Wrapper for LV2 plugins - port.
 * @copyright 2017-2019 Modern Ancient Instruments Networked AB, dba Elk, Stockholm
 */

#ifndef SUSHI_LV2_PORT_H
#define SUSHI_LV2_PORT_H

#ifdef SUSHI_BUILD_WITH_LV2

#include <exception>
#include <condition_variable>
#include <map>
#include <mutex>

#include "lv2_evbuf.h"

namespace sushi {
namespace lv2 {

class LV2Model;

enum PortFlow
{
    FLOW_UNKNOWN,
    FLOW_INPUT,
    FLOW_OUTPUT
};

enum PortType
{
    TYPE_UNKNOWN,
    TYPE_CONTROL,
    TYPE_AUDIO,
    TYPE_EVENT,
    TYPE_CV
};

class Port
{
public:
    Port(const LilvPlugin* plugin, int port_index, float default_value, LV2Model* model);

    ~Port()
    {

    }

    void setFlow(PortFlow flow)
    {
        _flow = flow;
    }

    PortFlow getFlow()
    {
        return _flow;
    }

    void setType(PortType type)
    {
        _type = type;
    }

    PortType getType()
    {
        return _type;
    }

    void resetInputBuffer();

    void resetOutputBuffer();

    void setBufSize(int buf_size)
    {
        _buf_size = buf_size;
    }

    const LilvPort* get_lilv_port()
    {
        return lilv_port;
    }

    float getMin()
    {
        return min;
    }

    float getMax()
    {
        return max;
    }

    int getIndex()
    {
        return _index;
    }

// TODO: Lilv requires a pointer to this, so as to set it.
    float control; ///< For control ports, otherwise 0.0f

// TODO: Make private after next refactor!
    LV2_Evbuf* _evbuf; ///< For MIDI ports, otherwise NULL

    struct FailedCreation : public std::exception
    {
        const char * what () const throw ()
        {
            return "Failed to Create LV2 Port";
        }
    };

private:
    /**
    * @brief Allocates LV2 port buffers (only necessary for MIDI)
    */
    void _allocate_port_buffers(LV2Model *model);

    const LilvPort* lilv_port; ///< LV2 port
    enum PortType _type; ///< Data type
    enum PortFlow _flow; ///< Data flow direction

    void* widget; ///< Control widget, if applicable
    int _buf_size; ///< Custom buffer size, or 0
    int _index; ///< Port index

    // For ranges. Only used in control ports.
    float def {1.0f};
    float max {1.0f};
    float min {0.0f};

    bool _show_hidden{true};
};

} // end namespace lv2
} // end namespace sushi


#endif //SUSHI_BUILD_WITH_LV2
#ifndef SUSHI_BUILD_WITH_LV2

// (...)

#endif

#endif //SUSHI_LV2_MODEL_H