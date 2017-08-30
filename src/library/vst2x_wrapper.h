/**
 * @Brief Wrapper for VST 2.x plugins.
 * @copyright MIND Music Labs AB, Stockholm
 *
 */

#ifndef SUSHI_VST2X_PLUGIN_H
#define SUSHI_VST2X_PLUGIN_H

#include "library/processor.h"
#include "library/vst2x_plugin_loader.h"
#include "library/vst2x_midi_event_fifo.h"

#include <map>

namespace sushi {
namespace vst2 {

/* Should match the maximum reasonable number of channels of a vst */
constexpr int VST_WRAPPER_MAX_N_CHANNELS = 8;
constexpr int VST_WRAPPER_MIDI_EVENT_QUEUE_SIZE = 256;

/**
 * @brief internal wrapper class for loading VST plugins and make them accesible as Processor to the Engine.
 */

class Vst2xWrapper : public Processor
{
public:
    MIND_DECLARE_NON_COPYABLE(Vst2xWrapper)
    /**
     * @brief Create a new Processor that wraps the plugin found in the given path.
     */

    Vst2xWrapper(const std::string &vst_plugin_path) :
            _sample_rate{0},
            _process_inputs{},
            _process_outputs{},
            _can_do_soft_bypass{false},
            _plugin_path{vst_plugin_path},
            _library_handle{nullptr},
            _plugin_handle{nullptr}
    {
        _max_input_channels = VST_WRAPPER_MAX_N_CHANNELS;
        _max_output_channels = VST_WRAPPER_MAX_N_CHANNELS;
        std::fill(_dummy_input, &_dummy_input[AUDIO_CHUNK_SIZE], 0.0f);
    }

    virtual ~Vst2xWrapper()
    {
        _cleanup();
    }

    /* Inherited from Processor */
    ProcessorReturnCode init(float sample_rate) override;

    void configure(float sample_rate) override;

    void process_event(Event event) override;

    void process_audio(const ChunkSampleBuffer &in_buffer, ChunkSampleBuffer &out_buffer) override;

    bool set_input_channels(int channels) override;

    bool set_output_channels(int channels) override;

    void set_enabled(bool enabled) override;

    void set_bypassed(bool bypassed) override;

private:
    /**
     * @brief Tell the plugin that we're done with it and release all resources
     * we allocated during initialization.
     */
    void _cleanup();

    /**
     * @brief Commodity function to access VsT internals
     */
    int _vst_dispatcher(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
    {
        return static_cast<int>(_plugin_handle->dispatcher(_plugin_handle, opcode, index, value, ptr, opt));
    }

    /**
     * @brief Iterate over VsT parameters and register internal FloatParameterDescriptor
     *        for each one of them.
     * @return True if all parameters were registered properly.
     */
    bool _register_parameters();

    bool _update_speaker_arrangements(int inputs, int outputs);

    void _map_audio_buffers(const ChunkSampleBuffer &in_buffer, ChunkSampleBuffer &out_buffer);

    float _sample_rate;
    /** Wrappers for preparing data to pass to processReplacing */
    float* _process_inputs[VST_WRAPPER_MAX_N_CHANNELS];
    float* _process_outputs[VST_WRAPPER_MAX_N_CHANNELS];
    float _dummy_input[AUDIO_CHUNK_SIZE];
    float _dummy_output[AUDIO_CHUNK_SIZE];
    Vst2xMidiEventFIFO<VST_WRAPPER_MIDI_EVENT_QUEUE_SIZE> _vst_midi_events_fifo;
    bool _can_do_soft_bypass;

    std::string _plugin_path;
    LibraryHandle _library_handle;
    AEffect *_plugin_handle;
};

VstSpeakerArrangementType arrangement_from_channels(int channels);


} // end namespace vst2
} // end namespace sushi

#endif //SUSHI_VST2X_PLUGIN_H
