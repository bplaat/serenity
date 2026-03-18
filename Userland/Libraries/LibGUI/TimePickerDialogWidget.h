/*
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGUI/Widget.h>

namespace GUI {

class TimePickerDialogWidget : public Widget {
    C_OBJECT_ABSTRACT(TimePickerDialogWidget)

public:
    static ErrorOr<NonnullRefPtr<TimePickerDialogWidget>> try_create();
    virtual ~TimePickerDialogWidget() override = default;

private:
    TimePickerDialogWidget() = default;
};

}
