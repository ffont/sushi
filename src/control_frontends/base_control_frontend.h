/**
 * @brief Base class for control frontend
 * @copyright MIND Music Labs AB, Stockholm
 *
 * This module provides run-time control of the audio engine for parameter
 * changes and plugin control.
 */
#ifndef SUSHI_BASE_CONTROL_FRONTEND_H
#define SUSHI_BASE_CONTROL_FRONTEND_H

#include <string>

#include "library/plugin_events.h"
#include "library/event_fifo.h"

namespace sushi {
namespace control_frontend {

class BaseControlFrontend
{
public:
    BaseControlFrontend(EventFifo* queue) :
            _queue(queue) {}

    virtual ~BaseControlFrontend() {};

    virtual void run() = 0;

    void send_parameter_change_event(const std::string& processor, const std::string& parameter, float value);

    void send_keyboard_event(const std::string& processor, EventType type, int note, float value);

private:
    EventFifo* _queue;
};

}; // namespace control_frontend
}; // namespace sushi

#endif //SUSHI_BASE_CONTROL_FRONTEND_H
