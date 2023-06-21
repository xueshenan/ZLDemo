/*
 * Copyright (c) 2016 The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/xia-chu/ZLMediaKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#include <mkapi/mk_mediakit.h>
#include <signal.h>

#include <sstream>
#include <thread>

#include "media_server.h"

int main(int argc, char *argv[]) {
    char *ini_path = mk_util_get_exe_dir("config.ini");
    mk_config config = {.ini = ini_path,
                        .ini_is_path = 1,
                        .log_level = 0,
                        .log_mask = LOG_CONSOLE,
                        .log_file_path = NULL,
                        .log_file_days = 0,
                        .ssl = NULL,
                        .ssl_is_path = 1,
                        .ssl_pwd = NULL,
                        .thread_num = 3};
    mk_env_init(&config);
    mk_free(ini_path);

    mk_rtsp_server_start(554, 0);
    mk_rtmp_server_start(1935, 0);

    mk_events events = {.on_mk_media_changed = NULL,
                        .on_mk_media_publish = NULL,
                        .on_mk_media_play = NULL,
                        .on_mk_media_not_found = NULL,
                        .on_mk_media_no_reader = NULL,
                        .on_mk_http_request = NULL,
                        .on_mk_http_access = NULL,
                        .on_mk_http_before_access = NULL,
                        .on_mk_rtsp_get_realm = NULL,
                        .on_mk_rtsp_auth = NULL,
                        .on_mk_record_mp4 = NULL,
                        .on_mk_shell_login = NULL,
                        .on_mk_flow_report = NULL};
    mk_events_listen(&events);

    const int max_count = 4;
    std::thread *t_buf[max_count] = {nullptr};
    media::MediaServer *media_buf[max_count] = {nullptr};
    for (int i = 1; i < argc; i++) {
        std::stringstream ss;
        ss << "stream" << i;
        bool hevc = false;
        std::string str_file(argv[i]);
        if (str_file.find("h265") != std::string::npos) {
            hevc = true;
        }
        media_buf[i] = new media::MediaServer(argv[i], ss.str().c_str(), hevc);
        t_buf[i] = new std::thread(&media::MediaServer::start, media_buf[i]);
    }

    for (int i = 0; i < max_count; i++) {
        if (t_buf[i] != nullptr) {
            t_buf[i]->join();
        }
    }
    mk_stop_all_server();
    return 0;
}