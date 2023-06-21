#include "media_server.h"

#include <unistd.h>

namespace media {

static void on_h264_frame(void *user_data, mk_h264_splitter splitter, const char *data, int size) {
    usleep(20 * 1000);
    static int dts = 0;
    mk_frame frame = mk_frame_create(MKCodecH264, dts, dts, data, size, NULL, NULL);
    dts += 20;
    mk_media_input_frame((mk_media)user_data, frame);
    mk_frame_unref(frame);
}

MediaServer::MediaServer(const char *media_file_path, const char *stream_id, bool hevc)
    : _media_file_path(media_file_path), _stream_id(stream_id), _hevc(hevc) {}

MediaServer::~MediaServer() {
    stop();
}

bool MediaServer::start() {
    _exit = false;
    _media = mk_media_create("__defaultVhost__", "live", _stream_id.c_str(), 0, 0, 0);

    codec_args v_args = {0};
    int codec_id = MKCodecH264;
    if (_hevc) {
        codec_id = MKCodecH265;
    }
    mk_track v_track = mk_track_create(codec_id, &v_args);
    mk_media_init_track(_media, v_track);
    mk_media_init_complete(_media);
    mk_track_unref(v_track);

    // 创建h264分帧器
    mk_h264_splitter splitter = mk_h264_splitter_create(on_h264_frame, _media, _hevc ? 1 : 0);

    FILE *fp = fopen(_media_file_path.c_str(), "rb");
    if (fp == nullptr) {
        log_error("打开文件失败 %d", errno);
        return false;
    }

    char buf[1024];
    while (!_exit) {
        int size = fread(buf, 1, sizeof(buf) - 1, fp);
        if (size > 0) {
            mk_h264_splitter_input_data(splitter, buf, size);
        } else {
            // 文件读完了，重新开始
            fseek(fp, 0, SEEK_SET);
        }
    }

    fclose(fp);

    if (_splitter != nullptr) {
        mk_h264_splitter_release(_splitter);
    }
    if (_media != nullptr) {
        mk_media_release(_media);
    }

    return true;
}

void MediaServer::stop() {
    _exit = true;
}

}  // namespace media