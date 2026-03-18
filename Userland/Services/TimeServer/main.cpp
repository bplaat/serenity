/*
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "TimeServer.h"
#include <LibConfig/Client.h>
#include <LibCore/EventLoop.h>
#include <LibCore/System.h>
#include <LibMain/Main.h>

ErrorOr<int> serenity_main(Main::Arguments)
{
    TRY(Core::System::pledge("stdio proc exec unix rpath"));
    Config::pledge_domain("TimeServer");

    Core::EventLoop event_loop;

    TRY(Core::System::unveil("/tmp/portal/config", "rw"));
    TRY(Core::System::unveil("/bin/ntpquery", "x"));
    TRY(Core::System::unveil(nullptr, nullptr));

    TimeServer time_server;

    return event_loop.exec();
}
