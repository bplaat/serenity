/*
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefPtr.h>
#include <LibConfig/Listener.h>
#include <LibCore/Timer.h>

class TimeServer final : public Config::Listener {
public:
    TimeServer();

private:
    void sync_time();

    static constexpr i32 sync_interval_ms = 3600 * 1000;

    // ^Config::Listener
    virtual void config_bool_did_change(StringView domain, StringView, StringView key, bool value) override;
    virtual void config_string_did_change(StringView domain, StringView, StringView key, StringView value) override;

    RefPtr<Core::Timer> m_timer;
};
