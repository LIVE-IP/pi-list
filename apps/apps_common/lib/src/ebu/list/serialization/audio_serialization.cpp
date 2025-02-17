#include "ebu/list/serialization/audio_serialization.h"

using namespace ebu_list;
using namespace std;

nlohmann::json ebu_list::to_json(const audio_stream_details& details)
{
    nlohmann::json statistics;
    statistics["sample_size"] = details.sample_size;
    statistics["samples_per_packet"] = details.samples_per_packet;
    statistics["packet_count"] = details.packet_count;
    statistics["sample_count"] = details.sample_count;
    statistics["first_packet_ts"] = std::chrono::duration_cast<std::chrono::nanoseconds>(details.first_packet_ts.time_since_epoch()).count();
    statistics["last_packet_ts"] = std::chrono::duration_cast<std::chrono::nanoseconds>(details.last_packet_ts.time_since_epoch()).count();

    nlohmann::json j;
    j["media_specific"] = st2110::d30::to_json(details.audio);
    j["statistics"] = statistics;

    return j;
}

audio_stream_details ebu_list::parse_audio_json(const nlohmann::json& j)
{
    audio_stream_details desc{};
    desc.audio = st2110::d30::from_json(j);
    return desc;
}

nlohmann::json st2110::d30::to_json(const st2110::d30::audio_description& desc)
{
    nlohmann::json j;
    j["encoding"] = to_string(desc.encoding);
    j["sampling"] = to_string(desc.sampling);
    j["number_channels"] = desc.number_channels;
    j["packet_time"] = d30::to_string(desc.packet_time);

    return j;
}

st2110::d30::audio_description st2110::d30::from_json(const nlohmann::json& j)
{
    audio_description desc{};

    if( j.empty() ) // media_specific was empty, using default values
        return desc;

    desc.sampling = media::audio::parse_audio_sampling(j.at("sampling").get<string>());
    desc.encoding = media::audio::parse_audio_encoding(j.at("encoding").get<string>());
    desc.number_channels = j.at("number_channels").get<uint8_t>();
    desc.packet_time = st2110::d30::parse_packet_time(j.at("packet_time").get<string>());

    return desc;
}
