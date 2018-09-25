/**
 * @Brief real time audio processing engine
 * @copyright MIND Music Labs AB, Stockholm
 *
 */

#ifndef SUSHI_BASE_ENGINE_H
#define SUSHI_BASE_ENGINE_H

#include <memory>
#include <map>
#include <vector>
#include <utility>

#include "base_event_dispatcher.h"
#include "engine/track.h"
#include "library/time.h"
#include "library/sample_buffer.h"
#include "library/types.h"


namespace sushi {
namespace engine {

enum class EngineReturnStatus
{
    OK,
    ERROR,
    INVALID_N_CHANNELS,
    INVALID_PLUGIN_UID,
    INVALID_PLUGIN_NAME,
    INVALID_PLUGIN_TYPE,
    INVALID_PROCESSOR,
    INVALID_PARAMETER,
    INVALID_TRACK,
    INVALID_BUS,
    INVALID_CHANNEL,
    QUEUE_FULL
};

enum class PluginType
{
    INTERNAL,
    VST2X,
    VST3X
};

enum class RealtimeState
{
    STARTING,
    RUNNING,
    STOPPING,
    STOPPED
};

class BaseEngine
{
public:
    BaseEngine(float sample_rate) : _sample_rate(sample_rate)
    {}

    virtual ~BaseEngine()
    {}

    float sample_rate()
    {
        return _sample_rate;
    }

    virtual void set_sample_rate(float sample_rate)
    {
        _sample_rate = sample_rate;
    }

    virtual void set_audio_input_channels(int channels)
    {
        _audio_inputs = channels;
    }

    virtual void set_audio_output_channels(int channels)
    {
        _audio_outputs = channels;
    }

    virtual EngineReturnStatus connect_audio_input_channel(int /*engine_channel*/,
                                                           int /*track_channel*/,
                                                           const std::string& /*track_name*/)
    {
        return EngineReturnStatus::OK;
    }

    virtual EngineReturnStatus connect_audio_output_channel(int /*engine_channel*/,
                                                            int /*track_channel*/,
                                                            const std::string& /*track_name*/)
    {
        return EngineReturnStatus::OK;
    }

    virtual EngineReturnStatus connect_audio_input_bus(int /*input_bus */,
                                                       int /*track_bus*/,
                                                       const std::string & /*track_name*/)
    {
        return EngineReturnStatus::OK;
    }

    virtual EngineReturnStatus connect_audio_output_bus(int /*output_bus*/,
                                                        int /*track_bus*/,
                                                        const std::string & /*track_name*/)
    {
        return EngineReturnStatus::OK;
    }

    virtual int n_channels_in_track(int /*track_no*/)
    {
        return 2;
    }

    virtual bool realtime()
    {
        return true;
    }

    virtual void enable_realtime(bool /*enabled*/) {}

    virtual void process_chunk(SampleBuffer<AUDIO_CHUNK_SIZE>* in_buffer, SampleBuffer<AUDIO_CHUNK_SIZE>* out_buffer) = 0;

    virtual void update_time(Time /*timestamp*/, int64_t /*samples*/) = 0;

    virtual void set_output_latency(Time /*latency*/) = 0;

    virtual void set_tempo(float /*tempo*/) = 0;

    virtual void set_time_signature(TimeSignature /*signature*/) = 0;

    virtual void set_transport_mode(PlayingMode /*mode*/) = 0;

    virtual void set_tempo_sync_mode(SyncMode /*mode*/) = 0;

    virtual EngineReturnStatus send_rt_event(RtEvent& event) = 0;

    virtual EngineReturnStatus send_async_event(RtEvent& event) = 0;

    virtual std::pair<EngineReturnStatus, ObjectId> processor_id_from_name(const std::string& /*name*/)
    {
        return std::make_pair(EngineReturnStatus::OK, 0);
    };

    virtual std::pair<EngineReturnStatus, ObjectId> parameter_id_from_name(const std::string& /*processor_name*/,
                                                                           const std::string& /*parameter_name*/)
    {
        return std::make_pair(EngineReturnStatus::OK, 0);
    };

    virtual std::pair<EngineReturnStatus, const std::string> processor_name_from_id(const ObjectId /*id*/)
    {
        return std::make_pair(EngineReturnStatus::OK, "");
    };

    virtual std::pair<EngineReturnStatus, const std::string> parameter_name_from_id(const std::string& /*processor_name*/,
                                                                                    const ObjectId /*id*/)
    {
        return std::make_pair(EngineReturnStatus::OK, "");
    };

    virtual EngineReturnStatus create_track(const std::string & /*track_id*/, int /*channel_count*/)
    {
        return EngineReturnStatus::OK;
    }

    virtual EngineReturnStatus create_multibus_track(const std::string & /*track_id*/, int /*input_busses*/, int /*output_busses*/)
    {
        return EngineReturnStatus::OK;
    }

    virtual EngineReturnStatus delete_track(const std::string & /*track_id*/)
    {
        return EngineReturnStatus::OK;
    }

    virtual EngineReturnStatus add_plugin_to_track(const std::string & /*track_id*/,
                                                   const std::string & /*uid*/,
                                                   const std::string & /*name*/,
                                                   const std::string & /*file*/,
                                                   PluginType /*plugin_type*/)
    {
        return EngineReturnStatus::OK;
    }

    virtual EngineReturnStatus remove_plugin_from_track(const std::string & /*track_id*/,
                                                        const std::string & /*plugin_id*/)
    {
        return EngineReturnStatus::OK;
    }

    virtual const std::map<std::string, std::unique_ptr<Processor>>& all_processors()
    {
        static std::map<std::string, std::unique_ptr<Processor>> tmp;
        return tmp;
    }

    virtual const std::vector<Track*>& all_tracks()
    {
        static std::vector<Track*> tmp;
        return tmp;
    }

    virtual sushi::dispatcher::BaseEventDispatcher* event_dispatcher()
    {
        return nullptr;
    }

    virtual void enable_timing_statistics(bool /*enabled*/) {}

    virtual void print_timings_to_log() {}

protected:
    float _sample_rate;
    int _audio_inputs{0};
    int _audio_outputs{0};
};

} // namespace engine
} // namespace sushi

#endif //SUSHI_BASE_ENGINE_H