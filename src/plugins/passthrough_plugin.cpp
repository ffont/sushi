#include <algorithm>
#include <cassert>

#include "passthrough_plugin.h"

namespace sushi {
namespace passthrough_plugin {

PassthroughPlugin::PassthroughPlugin(HostControl host_control) : InternalPlugin(host_control)
{
    Processor::set_name(DEFAULT_NAME);
    Processor::set_label(DEFAULT_LABEL);
}

PassthroughPlugin::~PassthroughPlugin()
{}

void
PassthroughPlugin::process_audio(const ChunkSampleBuffer &in_buffer, ChunkSampleBuffer &out_buffer)
{
    bypass_process(in_buffer, out_buffer, _current_input_channels, _current_output_channels);

    /* Pass keyboard data/midi through */
    while (!_event_queue.empty())
    {
        RtEvent event;
        if (_event_queue.pop(event))
        {
            output_event(event);
        }
    }
}


}// namespace passthrough_plugin
}// namespace sushi