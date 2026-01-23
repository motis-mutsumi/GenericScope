#ifndef SCOPEIMAGETRANSFER_H
#define SCOPEIMAGETRANSFER_H

#include "scopetransferbasic.h"
#include <functional>

#pragma pack(push, 1)
struct ImageInfo
{
    uint32_t width = 640;
    uint32_t height = 512;
    uint8_t bit_depth = 2;
    float fps = 50;
};
#pragma pack(pop)

class TRANSFER_EXPORT ScopeImageTransfer : virtual public ScopeTransferBasic
{
public:
    using FrameCallback = std::function<void(uint8_t *frame_data_, ImageInfo *image_info_)>;
    void setFrameCallback(FrameCallback cb) { m_frameCallback = cb; }
    void setImageInfo(const ImageInfo &image_info_) noexcept { m_imageInfo = image_info_; }
    bool isRunning() const { return m_running.load(); }

protected:
    ScopeImageTransfer() = default;
    ~ScopeImageTransfer();

    virtual ScopeTransferStatus startStream() = 0;
    virtual ScopeTransferStatus stopStream() = 0;

protected:
    std::atomic<bool> m_running = {false};
    FrameCallback m_frameCallback;
    ImageInfo m_imageInfo;
};

#endif // SCOPEIMAGETRANSFER_H
