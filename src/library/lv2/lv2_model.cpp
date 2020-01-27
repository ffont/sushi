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
 * @Brief Wrapper for LV2 plugins - Wrapper for LV2 plugins - model.
 * @copyright 2017-2019 Modern Ancient Instruments Networked AB, dba Elk, Stockholm
 */

#ifdef SUSHI_BUILD_WITH_LV2

#include "lv2_model.h"
#include "lv2_features.h"
#include "lv2_worker.h"
#include "lv2_state.h"
#include "logging.h"

namespace {
// TODO: verify that these LV2 features work as intended:
/** These features have no data */
static const LV2_Feature static_features[] = {
        { LV2_STATE__loadDefaultState, nullptr },
        { LV2_BUF_SIZE__powerOf2BlockLength, nullptr },
        { LV2_BUF_SIZE__fixedBlockLength, nullptr },
        { LV2_BUF_SIZE__boundedBlockLength, nullptr } };

} // anonymous namespace


namespace sushi {
namespace lv2 {

SUSHI_GET_LOGGER_WITH_MODULE_NAME("lv2");

LV2Model::LV2Model(LilvWorld* worldIn):
_nodes(worldIn),
_world(worldIn)
{
    // This allows loading plu-ins from their URI's, assuming they are installed in the correct paths
    // on the local machine.
    /* Find all installed plugins */
    lilv_world_load_all(_world);

    _initialize_map_feature();
    _initialize_worker_feature();
    _initialize_unmap_feature();
    _initialize_urid_symap();
    _initialize_log_feature();
    _initialize_safe_restore_feature();
    _initialize_make_path_feature();
}

LV2Model::~LV2Model()
{
    symap_free(_symap);
}

void LV2Model::initialize_host_feature_list()
{
    /* Build feature list for passing to plugins */
    std::vector<const LV2_Feature*> features({
            &_features.map_feature,
            &_features.unmap_feature,
            &_features.log_feature,
            &_features.sched_feature,
            &_features.make_path_feature,
// TODO: Re-introduce options extension!
            //&_features.options_feature,
            &static_features[0],
            &static_features[1],
            &static_features[2],
            &static_features[3],
            nullptr
    });

    _feature_list = std::move(features);
}

void LV2Model::set_worker_interface(const LV2_Worker_Interface* iface)
{
    if(_worker)
        _worker->set_iface(iface);

    if(_state_worker)
        _state_worker->set_iface(iface);
}

void LV2Model::_initialize_urid_symap()
{
    lv2_atom_forge_init(&this->_forge, &_map);

    this->_urids.atom_Float = symap_map(this->_symap, LV2_ATOM__Float);
    this->_urids.atom_Int = symap_map(this->_symap, LV2_ATOM__Int);
    this->_urids.atom_Object = symap_map(this->_symap, LV2_ATOM__Object);
    this->_urids.atom_Path = symap_map(this->_symap, LV2_ATOM__Path);
    this->_urids.atom_String = symap_map(this->_symap, LV2_ATOM__String);
    this->_urids.atom_eventTransfer = symap_map(this->_symap, LV2_ATOM__eventTransfer);
    this->_urids.bufsz_maxBlockLength = symap_map(this->_symap, LV2_BUF_SIZE__maxBlockLength);
    this->_urids.bufsz_minBlockLength = symap_map(this->_symap, LV2_BUF_SIZE__minBlockLength);
    this->_urids.bufsz_sequenceSize = symap_map(this->_symap, LV2_BUF_SIZE__sequenceSize);
    this->_urids.log_Error = symap_map(this->_symap, LV2_LOG__Error);
    this->_urids.log_Trace = symap_map(this->_symap, LV2_LOG__Trace);
    this->_urids.log_Warning = symap_map(this->_symap, LV2_LOG__Warning);
    this->_urids.log_Entry = symap_map(this->_symap, LV2_LOG__Entry);
    this->_urids.log_Note = symap_map(this->_symap, LV2_LOG__Note);
    this->_urids.log_log = symap_map(this->_symap, LV2_LOG__log);
    this->_urids.midi_MidiEvent = symap_map(this->_symap, LV2_MIDI__MidiEvent);
    this->_urids.param_sampleRate = symap_map(this->_symap, LV2_PARAMETERS__sampleRate);
    this->_urids.patch_Get = symap_map(this->_symap, LV2_PATCH__Get);
    this->_urids.patch_Put = symap_map(this->_symap, LV2_PATCH__Put);
    this->_urids.patch_Set = symap_map(this->_symap, LV2_PATCH__Set);
    this->_urids.patch_body = symap_map(this->_symap, LV2_PATCH__body);
    this->_urids.patch_property = symap_map(this->_symap, LV2_PATCH__property);
    this->_urids.patch_value = symap_map(this->_symap, LV2_PATCH__value);
    this->_urids.time_Position = symap_map(this->_symap, LV2_TIME__Position);
    this->_urids.time_bar = symap_map(this->_symap, LV2_TIME__bar);
    this->_urids.time_barBeat = symap_map(this->_symap, LV2_TIME__barBeat);
    this->_urids.time_beatUnit = symap_map(this->_symap, LV2_TIME__beatUnit);
    this->_urids.time_beatsPerBar = symap_map(this->_symap, LV2_TIME__beatsPerBar);
    this->_urids.time_beatsPerMinute = symap_map(this->_symap, LV2_TIME__beatsPerMinute);
    this->_urids.time_frame = symap_map(this->_symap, LV2_TIME__frame);
    this->_urids.time_speed = symap_map(this->_symap, LV2_TIME__speed);
    this->_urids.ui_updateRate = symap_map(this->_symap, LV2_UI__updateRate);
}

void LV2Model::_initialize_log_feature()
{
    this->_features.llog.handle = this;
    this->_features.llog.printf = lv2_printf;
    this->_features.llog.vprintf = lv2_vprintf;
    init_feature(&this->_features.log_feature, LV2_LOG__log, &this->_features.llog);
}

void LV2Model::_initialize_map_feature()
{
    this->_symap = symap_new();
    this->_map.handle = this;
    this->_map.map = map_uri;
    init_feature(&this->_features.map_feature, LV2_URID__map, &this->_map);
}

void LV2Model::_initialize_unmap_feature()
{
    this->_unmap.handle = this;
    this->_unmap.unmap = unmap_uri;
    init_feature(&this->_features.unmap_feature, LV2_URID__unmap, &this->_unmap);
}

void LV2Model::_initialize_worker_feature()
{
    _worker = std::make_unique<Lv2_Worker>(this, true);

    // TODO: Should I really inherit this check? Why not just always create it?
    if (_safe_restore)
    {
        _state_worker = std::make_unique<Lv2_Worker>(this, true);
    }

    this->_features.sched.handle = &this->_worker;
    this->_features.sched.schedule_work = lv2_worker_schedule;

    init_feature(&this->_features.sched_feature,
                 LV2_WORKER__schedule, &this->_features.sched);

    this->_features.ssched.handle = &this->_state_worker;
    this->_features.ssched.schedule_work = lv2_worker_schedule;

    init_feature(&this->_features.state_sched_feature,
                 LV2_WORKER__schedule, &this->_features.ssched);
}

void LV2Model::process_worker_replies()
{
    if(_state_worker.get())
        _state_worker->emit_responses(_plugin_instance);

    if(_worker.get())
    {
        _worker->emit_responses(_plugin_instance);

        /* Notify the plugin the run() cycle is finished */
        // TODO: Make this a member of Lv2_worker? If not move back to wrapper.
        if (_worker->get_iface() && _worker->get_iface()->end_run) {
            _worker->get_iface()->end_run(_plugin_instance->lv2_handle);
        }
    }
}

void LV2Model::_initialize_safe_restore_feature()
{
    init_feature(&this->_features.safe_restore_feature,
                 LV2_STATE__threadSafeRestore,
                 nullptr);
}

void LV2Model::_initialize_make_path_feature()
{
    this->_features.make_path.handle = this;
    this->_features.make_path.path = make_path;
    init_feature(&this->_features.make_path_feature,
                 LV2_STATE__makePath, &this->_features.make_path);
}

HostFeatures& LV2Model::get_features()
{
    return _features;
}

std::vector<const LV2_Feature*>* LV2Model::get_feature_list()
{
    return &_feature_list;
}

LilvWorld* LV2Model::get_world()
{
    return _world;
}

LilvInstance* LV2Model::get_plugin_instance()
{
    return _plugin_instance;
}

void LV2Model::set_plugin_instance(LilvInstance* new_instance)
{
    _plugin_instance = new_instance;
}

const LilvPlugin* LV2Model::get_plugin_class()
{
    return _plugin_class;
}

void LV2Model::set_plugin_class(const LilvPlugin* new_plugin)
{
    _plugin_class = new_plugin;
}

LilvState* LV2Model::get_preset()
{
    return _preset;
}

void LV2Model::set_preset(LilvState* new_preset)
{
    if (_preset != nullptr)
    {
        lilv_state_free(_preset);
    }

    _preset = new_preset;
}

int LV2Model::get_midi_buffer_size()
{
    return _midi_buffer_size;
}

float LV2Model::get_sample_rate()
{
    return _sample_rate;
}

Port* LV2Model::get_port(int index)
{
    return _ports[index].get();
}

void LV2Model::add_port(std::unique_ptr<Port>&& port)
{
    _ports.emplace_back(std::move(port));
}

int LV2Model::get_port_count()
{
    return _ports.size();
}

const Lv2_Host_Nodes& LV2Model::get_nodes()
{
    return _nodes;
}

const LV2_URIDs& LV2Model::get_urids()
{
    return _urids;
}

LV2_URID_Map& LV2Model::get_map()
{
    return _map;
}

LV2_URID_Unmap& LV2Model::get_unmap()
{
    return _unmap;
}

LV2_URID LV2Model::map(const char* uri)
{
    std::unique_lock<std::mutex> lock(_symap_lock);
    return symap_map(_symap, uri);
}

const char* LV2Model::unmap(LV2_URID urid)
{
    std::unique_lock<std::mutex> lock(_symap_lock);
    const char* uri = symap_unmap(_symap, urid);

    return uri;
}

const LV2_Atom_Forge& LV2Model::get_forge()
{
    return _forge;
}

int LV2Model::get_plugin_latency()
{
    return _plugin_latency;
}

void LV2Model::set_plugin_latency(int latency)
{
    _plugin_latency = latency;
}

std::mutex& LV2Model::get_work_lock()
{
    return _work_lock;
}

Lv2_Worker* LV2Model::get_worker()
{
    return _worker.get();
}

Lv2_Worker* LV2Model::get_state_worker()
{
    return _state_worker.get();
}

bool LV2Model::get_exit()
{
    return _exit;
}

void LV2Model::trigger_exit()
{
    _exit = true;

    /* Terminate the worker */
    if(_worker)
    {
        _worker->finish();
        _worker->destroy();
    }
}

int LV2Model::get_control_input_index()
{
    return _control_input_index;
}

void LV2Model::set_control_input_index(int index)
{
    _control_input_index = index;
}

bool LV2Model::update_requested()
{
    return _request_update;
}

void LV2Model::request_update()
{
    _request_update = true;
}

void LV2Model::clear_update_request()
{
    _request_update = false;
}

void LV2Model::set_restore_thread_safe(bool safe)
{
    _safe_restore = safe;
}

bool LV2Model::is_restore_thread_safe()
{
    return _safe_restore;
}

}
}

#endif //SUSHI_BUILD_WITH_LV2