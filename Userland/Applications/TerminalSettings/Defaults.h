/*
 * Copyright (c) 2025, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibVT/TerminalWidget.h>

namespace Terminal {

static constexpr VT::TerminalWidget::BellMode default_bell_mode = VT::TerminalWidget::BellMode::Visible;
static constexpr VT::TerminalWidget::AutoMarkMode default_automark_mode = VT::TerminalWidget::AutoMarkMode::MarkInteractiveShellPrompt;
static constexpr bool default_confirm_close = true;

static constexpr int default_opacity = 255;
static constexpr VT::CursorShape default_cursor_shape = VT::CursorShape::Block;
static constexpr bool default_cursor_blinking = true;
static constexpr int default_max_history_size = 1024;
static constexpr bool default_show_scrollbar = true;

}
