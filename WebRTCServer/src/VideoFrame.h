#pragma once

#include <api/units/time_delta.h>
#include <rtc_base/ref_counted_object.h>
#include <rtc_base/timestamp_aligner.h>

#include "Size.h"

namespace unity
{
    namespace webrtc
    {

        using namespace ::webrtc;

        class VideoFrame : public rtc::RefCountInterface
        {
        public:
            VideoFrame() = delete;
            VideoFrame(const VideoFrame&) = delete;
            VideoFrame& operator=(const VideoFrame&) = delete;

            Size size() const { return size_; }
            TimeDelta timestamp() const { return timestamp_; }
            void set_timestamp(TimeDelta timestamp) { timestamp_ = timestamp; }

            bool HasGpuMemoryBuffer() const;

        protected:
            VideoFrame(
                const Size& size,
                TimeDelta timestamp);
            virtual ~VideoFrame() override;

        private:
            Size size_;
            TimeDelta timestamp_;
        };

    } // end namespace webrtc
} // end namespace unity
