#include <cassert>

#include "sample_player_plugin.h"

namespace sushi {
namespace sample_player_plugin {

SamplePlayerPlugin::SamplePlayerPlugin()
{}

SamplePlayerPlugin::~SamplePlayerPlugin()
{}

StompBoxStatus SamplePlayerPlugin::init(const StompBoxConfig &configuration)
{
    _configuration = configuration;
    _volume_parameter = configuration.controller->register_float_parameter("volume", "Volume", 0.0f, new dBToLinPreProcessor(-120.0f, 36.0f));
    return StompBoxStatus::OK;
}

void SamplePlayerPlugin::process_events(void* events, bool realtime)
{
    int note;
    int offset;
    // noteon
    for (auto voice : _voices)
    {
        if (!voice.active())
        {
            voice.note_on(note, 1.0f, offset);
            break;
        }
    }

    // note off
    for (auto voice : _voices)
    {
        if (voice.active() && voice.current_note() == note)
        {
            voice.note_off(1.0f, offset);
            break;
        }
    }
}


void SamplePlayerPlugin::process(const SampleBuffer<AUDIO_CHUNK_SIZE>* in_buffer, SampleBuffer<AUDIO_CHUNK_SIZE>* out_buffer)
{


    float gain = _volume_parameter->value();
    /* With SampleBuffer operations */
    out_buffer->clear();
    out_buffer->add_with_gain(*in_buffer, gain);
}


}// namespace sample_player_plugin
}// namespace sushi