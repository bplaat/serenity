/*
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "TimeServer.h"
#include <LibConfig/Client.h>
#include <LibCore/Command.h>

TimeServer::TimeServer()
{
    m_timer = Core::Timer::create_single_shot(sync_interval_ms, [this]() {
        if (!Config::read_bool("TimeServer"sv, "Client"sv, "Enabled"sv, false))
            return;
        sync_time();
        m_timer->restart(sync_interval_ms);
    });

    if (Config::read_bool("TimeServer"sv, "Client"sv, "Enabled"sv, false)) {
        sync_time();
        m_timer->start();
    }
}

void TimeServer::sync_time()
{
    auto host = Config::read_string("TimeServer"sv, "Client"sv, "ServerAddress"sv, ""sv);
    dbgln("TimeServer: Syncing time with {}", host.is_empty() ? "default server"sv : host.view());
    Vector<ByteString> args = { "--set" };
    if (!host.is_empty())
        args.append(host);
    if (auto result = Core::command("/bin/ntpquery", args, {}); result.is_error())
        dbgln("TimeServer: Failed to sync time: {}", result.error());
    else
        dbgln("TimeServer: Time synced successfully");
}

void TimeServer::config_bool_did_change(StringView domain, StringView, StringView key, bool value)
{
    if (domain != "TimeServer"sv)
        return;
    if (key == "Enabled"sv) {
        if (value) {
            sync_time();
            m_timer->restart(sync_interval_ms);
        } else {
            m_timer->stop();
        }
    }
}

void TimeServer::config_string_did_change(StringView domain, StringView, StringView key, StringView)
{
    if (domain != "TimeServer"sv || key != "ServerAddress"sv)
        return;
    if (Config::read_bool("TimeServer"sv, "Client"sv, "Enabled"sv, false)) {
        sync_time();
        m_timer->restart(sync_interval_ms);
    }
}
