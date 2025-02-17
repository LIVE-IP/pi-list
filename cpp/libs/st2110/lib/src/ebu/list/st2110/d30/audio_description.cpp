#include "ebu/list/st2110/d30/audio_description.h"

using namespace ebu_list::media;
using namespace ebu_list::st2110;
using namespace ebu_list::st2110::d30;

audio_packet_time d30::parse_packet_time(std::string_view s)
{
    return audio_packet_time(std::stof(s.data()));
}

std::string d30::to_string(audio_packet_time t)
{
    return std::to_string(t.count());
}

//-----------------------------------------------

void st2110_30_sdp_serializer::additional_attributes(std::vector<std::string>& current_lines, const ebu_list::media::network_media_description& /*network_info*/)
{
    // todo: implement source-filters (rfc4570)
    //current_lines.emplace_back("a=source-filter:incl IN IP4 239.100.9.10 192.168.100.2");

    /** Obligatory Parameters **/
    current_lines.emplace_back(fmt::format("a=ptime:{}",audio_desc_.packet_time.count()));

    // todo: add PTP
    //current_lines.emplace_back("a=ts-refclk:ptp=IEEE1588-2008:39-A7-94-FF-FE-07-CB-D0:37");
    //current_lines.emplace_back("a=mediaclk:direct=0");
}

void st2110_30_sdp_serializer::write_rtpmap_line(std::vector<std::string>& current_lines, const ebu_list::media::network_media_description& media_description)
{
    // a=rtpmap:<payload_type> <encoding>/<clock_rate>/<channels>
    current_lines.emplace_back(fmt::format("a=rtpmap:{} {}/{}/{}", media_description.network.payload_type, to_string(audio_desc_.encoding), to_int(audio_desc_.sampling), audio_desc_.number_channels));
}

st2110_30_sdp_serializer::st2110_30_sdp_serializer(const d30::audio_description& audio_des)
    : audio_desc_(audio_des)
{
}