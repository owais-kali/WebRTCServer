#include "pch.h"

#include <api/make_ref_counted.h>

#include "VideoFrame.h"

namespace unity
{
    namespace webrtc
    {
        bool VideoFrame::HasGpuMemoryBuffer() const
        {
            return false;
        }

        VideoFrame::VideoFrame(
            const Size& size,
            TimeDelta timestamp)
            : size_(size)
            , timestamp_(timestamp)
        {
        }

        VideoFrame::~VideoFrame()
        {
            
        }

    } // end namespace webrtc
} // end namespace unity
