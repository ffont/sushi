/**
 * @Brief Wrapper for VST 2.x plugins.
 * @copyright MIND Music Labs AB, Stockholm
 *
 */

#ifndef SUSHI_LV2_PLUGIN_H
#define SUSHI_LV2_PLUGIN_H

#ifdef SUSHI_BUILD_WITH_LV2

#include <map>

#include "library/processor.h"
#include "library/lv2_plugin_loader.h"
//#include "library/lv2_midi_event_fifo.h"
#include "../engine/base_event_dispatcher.h"

#include "lv2/atom/atom.h"
#include "lv2/buf-size/buf-size.h"
#include "lv2/data-access/data-access.h"
#include "lv2/options/options.h"
#include "lv2/parameters/parameters.h"
#include "lv2/patch/patch.h"
#include "lv2/port-groups/port-groups.h"
#include "lv2/port-props/port-props.h"
#include "lv2/presets/presets.h"
#include "lv2/state/state.h"
#include "lv2/time/time.h"
#include "lv2/ui/ui.h"
#include "lv2/urid/urid.h"
#include "lv2/worker/worker.h"

namespace sushi {
namespace lv2 {

/* Should match the maximum reasonable number of channels of a vst */
constexpr int LV2_WRAPPER_MAX_N_CHANNELS = 8;
constexpr int LV2_WRAPPER_MIDI_EVENT_QUEUE_SIZE = 256;

//void populate_nodes(Jalv& jalv, LilvWorld* world);

/**
 * @brief internal wrapper class for loading VST plugins and make them accessible as Processor to the Engine.
 */

class Lv2Wrapper : public Processor
{
public:
    MIND_DECLARE_NON_COPYABLE(Lv2Wrapper)
    /**
     * @brief Create a new Processor that wraps the plugin found in the given path.
     */
    Lv2Wrapper(HostControl host_control, const std::string &lv2_plugin_uri) :
            Processor(host_control),
            _sample_rate{0},
            _process_inputs{},
            _process_outputs{},
            _can_do_soft_bypass{false},
            _double_mono_input{false},
            _plugin_path{lv2_plugin_uri}
    {
        _max_input_channels = LV2_WRAPPER_MAX_N_CHANNELS;
        _max_output_channels = LV2_WRAPPER_MAX_N_CHANNELS;
    }

    virtual ~Lv2Wrapper()
    {
        _cleanup();
    }

    /* Inherited from Processor */
    ProcessorReturnCode init(float sample_rate) override;

    void configure(float sample_rate) override;

    void process_event(RtEvent event) override;

    void process_audio(const ChunkSampleBuffer &in_buffer, ChunkSampleBuffer &out_buffer) override;

    void set_input_channels(int channels) override;

    void set_output_channels(int channels) override;

    void set_enabled(bool enabled) override;

    void set_bypassed(bool bypassed) override;

    std::pair<ProcessorReturnCode, float> parameter_value(ObjectId parameter_id) const override;

    std::pair<ProcessorReturnCode, float> parameter_value_normalised(ObjectId parameter_id) const override;

    std::pair<ProcessorReturnCode, std::string> parameter_value_formatted(ObjectId parameter_id) const override;

    bool supports_programs() const override {return _number_of_programs > 0;}

    int program_count() const override {return _number_of_programs;}

    int current_program() const override;

    std::string current_program_name() const override;

    std::pair<ProcessorReturnCode, std::string> program_name(int program) const override;

    std::pair<ProcessorReturnCode, std::vector<std::string>> all_program_names() const override;

    ProcessorReturnCode set_program(int program) override;

private:
    void create_ports(const LilvPlugin *plugin, const JalvNodes& nodes);
    void create_port(const LilvPlugin *plugin, uint32_t port_index, float default_value, const JalvNodes& nodes);

    /**
     * @brief Tell the plugin that we're done with it and release all resources
     * we allocated during initialization.
     */
    void _cleanup();

    /**
     * @brief Iterate over LV2 parameters and register internal FloatParameterDescriptor
     *        for each one of them.
     * @return True if all parameters were registered properly.
     */
    bool _register_parameters();

    bool _update_speaker_arrangements(int inputs, int outputs);

    /**
     * @brief For plugins that support stereo I/0 and not mono through SetSpeakerArrangements,
     *        we can provide the plugin with a dual mono input/output instead.
     *        Calling this sets up possible dual mono mode.
     * @param speaker_arr_status True if the plugin supports the given speaker arrangement.
     */
    void _update_mono_mode(bool speaker_arr_status);

    void _map_audio_buffers(const ChunkSampleBuffer &in_buffer, ChunkSampleBuffer &out_buffer);

    float _sample_rate;
    /** Wrappers for preparing data to pass to processReplacing */
    float* _process_inputs[LV2_WRAPPER_MAX_N_CHANNELS];
    float* _process_outputs[LV2_WRAPPER_MAX_N_CHANNELS];
    ChunkSampleBuffer _dummy_input{1};
    ChunkSampleBuffer _dummy_output{1};
//  Lv2MidiEventFIFO<LV2_WRAPPER_MIDI_EVENT_QUEUE_SIZE> _vst_midi_events_fifo;
    bool _can_do_soft_bypass;
    bool _double_mono_input;
    int _number_of_programs{0};

    std::string _plugin_path;

    // Ilias TODO: Check, can this initialization ever fail? then, make it pointer, and move construction to init();
    // TODO: Currently, this is instantiated in wrapper.
    // But if there's more than one plugin, there should not be two instances of _loader, right?
    PluginLoader _loader;
    Jalv _jalv;

    bool show_hidden{true};

    uint32_t _buffer_size; ///< Plugin <= >UI communication buffer size


    // For iterating over buffers in process_audio
    uint32_t _p = 0, _i = 0, _o = 0;

//    VstTimeInfo _time_info;
};

//VstSpeakerArrangementType arrangement_from_channels(int channels);

} // end namespace lv2
} // end namespace sushi


#endif //SUSHI_BUILD_WITH_LV2
#ifndef SUSHI_BUILD_WITH_LV2

#include "library/processor.h"

namespace sushi {
namespace lv2 {
/* If LV2 support is disabled in the build, the wrapper is replaced with this
   minimal dummy processor whose purpose is to log an error message if a user
   tries to load an LV2 plugin */
class Lv2Wrapper : public Processor
{
public:
    Lv2Wrapper(HostControl host_control, const std::string& /* vst_plugin_path */, const std::string& /* plugin_name */) :
        Processor(host_control) {}
    ProcessorReturnCode init(float sample_rate) override;
    void process_event(RtEvent /*event*/) override {}
    void process_audio(const ChunkSampleBuffer & /*in*/, ChunkSampleBuffer & /*out*/) override {}
};

}// end namespace lv2
}// end namespace sushi
#endif

#endif //SUSHI_LV2_PLUGIN_H