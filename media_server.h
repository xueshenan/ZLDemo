#pragma once

#include <mkapi/mk_mediakit.h>

#include <atomic>
#include <string>

namespace media {

class MediaServer {
public:
    MediaServer(const char *media_file_path, const char *stream_id, bool hevc);
    ~MediaServer();
public:
    bool start();
    void stop();
private:
    const std::string _media_file_path;
    const std::string _stream_id;
private:
    std::atomic<bool> _exit;
    mk_media _media{nullptr};
    mk_h264_splitter _splitter{nullptr};
    bool _hevc{false};
};

}  // namespace media