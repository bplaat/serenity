/*
 * Copyright (c) 2020-2021, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <ImageDecoder/ConnectionFromClient.h>
#include <LibCore/EventLoop.h>
#include <LibCore/System.h>
#include <LibGfx/Palette.h>
#include <LibGfx/SystemTheme.h>
#include <LibIPC/SingleServer.h>
#include <LibMain/Main.h>
#include <LibWeb/Bindings/MainThreadVM.h>
#include <LibWeb/HTML/EventLoop/EventLoop.h>
#include <LibWeb/Platform/EventLoopPlugin.h>
#include <LibWeb/Platform/EventLoopPluginSerenity.h>
#include <LibWeb/Platform/FontPlugin.h>
#include <LibWeb/Platform/FontPluginSerenity.h>

ErrorOr<int> serenity_main(Main::Arguments)
{
    Core::EventLoop event_loop;
    TRY(Core::System::pledge("stdio recvfd sendfd thread unix rpath map_fixed"));
    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil(nullptr, nullptr));

    auto client = TRY(IPC::take_over_accepted_client_from_system_server<ImageDecoder::ConnectionFromClient>());

    // Initialise LibWeb on the main thread for SVG rendering.
    Web::Platform::EventLoopPlugin::install(*new Web::Platform::EventLoopPluginSerenity);
    Web::Platform::FontPlugin::install(*new Web::Platform::FontPluginSerenity);
    TRY(Web::Bindings::initialize_main_thread_vm(Web::HTML::EventLoop::Type::Window));

    // Load default theme palette for SVG rendering.
    auto theme_buffer = TRY(Gfx::load_system_theme("/res/themes/Default.ini"));
    ImageDecoder::g_svg_palette = Gfx::PaletteImpl::create_with_anonymous_buffer(theme_buffer);

    ImageDecoder::g_main_event_loop = &event_loop;

    TRY(Core::System::pledge("stdio recvfd sendfd thread rpath map_fixed"));
    return event_loop.exec();
}
