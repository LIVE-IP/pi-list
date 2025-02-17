#include "ebu/list/st2110/pch.h"
#include "ebu/list/st2110/common_video_detector.h"
#include "ebu/list/st2110/d20/video_format_detector.h"
#include "ebu/list/st2110/d30/audio_format_detector.h"

using namespace ebu_list;
using namespace ebu_list::st2110;

//////////////////////////////////////////////////////////////////////

common_video_detector::common_video_detector(settings _settings)
    : settings_(_settings)
{
}

detector::status common_video_detector::handle_data(const rtp::packet& packet)
{
    if (status_ != detector::status::detecting)
    {
        return status_;
    }

    if (!current_frame_rtp_timestamp_ || last_frame_was_marked_)
    {
        last_frame_was_marked_ = false;
        ++current_frame_;
        current_frame_rtp_timestamp_ = packet.info.rtp().timestamp();
        current_frame_packets_ = 0;

        rate_.on_packet(current_frame_rtp_timestamp_.value());
    }
    else
    {
        assert(current_frame_rtp_timestamp_.has_value());

        if (current_frame_rtp_timestamp_ != packet.info.rtp().timestamp())
        {
            logger()->debug("INVALID: Different RTP timestamps in the same frame");
            status_ = detector::status::invalid;
            return status_;
        }
    }

    packets_per_frame_.on_packet(packet.info.rtp.view());

    ++current_frame_packets_;

    if (current_frame_packets_ > settings_.maximum_packets_per_frame)
    {
        logger()->debug("INVALID: too many packets until a marker bit was detected");
        status_ = detector::status::invalid;
        return status_;
    }

    if (packet.info.rtp().marker())
    {
        last_frame_was_marked_ = true;

        if (current_frame_ == rate_calculator::minimum_number_of_frames)
        {
            const auto packets = packets_per_frame_.count();

            if (packets < settings_.minimum_packets_per_frame)
            {
                status_ = detector::status::invalid;
                return status_;
            }

            const auto r = rate_.rate();
            if (!r.has_value())
            {
                // There is a problem with this stream: the timestamps are repeated.
                status_ = detector::status::invalid;
                return status_;
            }

            status_ = detector::status::valid;
            logger()->debug("Rate: {}", to_string(r.value()));

            if (packets)
            {
                logger()->debug("Packets per frame: {}", packets.value());
            }
            return status_;
        }
    }

    return status_;
}

int common_video_detector::packets_pre_frame() const
{
    return packets_per_frame_.count().value_or(0);
}

media::video::Rate common_video_detector::rate() const
{
    assert(rate_.rate().has_value());
    return rate_.rate().value();
}
