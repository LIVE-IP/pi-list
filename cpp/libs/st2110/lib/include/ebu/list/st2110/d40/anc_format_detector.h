#pragma once

#include "ebu/list/st2110/common_video_detector.h"

namespace ebu_list::st2110::d40
{
    class anc_format_detector : public detector
    {
    public:
        anc_format_detector();

        status handle_data(const rtp::packet& packet) override;
        virtual details get_details() const override;

    private:
        common_video_detector detector_;
    };
}
