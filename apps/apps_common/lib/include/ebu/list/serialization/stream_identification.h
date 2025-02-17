#pragma once

#include "ebu/list/serialization/serializable_stream_info.h"
#include "ebu/list/serialization/video_serialization.h"
#include "ebu/list/serialization/audio_serialization.h"

#include <variant>
#include <tuple>

namespace ebu_list
{
    using video_or_audio = std::variant<video_stream_details, audio_stream_details>;
    using stream_with_details = std::pair<serializable_stream_info, video_or_audio>;

    stream_with_details from_json(const path& json_file);
    nlohmann::json to_json(const stream_with_details& stream_info);

    void write_stream_id_info(const path& dir, const stream_with_details& stream_info);
    std::vector<stream_with_details> scan_folder(const path& folder_path);
}
