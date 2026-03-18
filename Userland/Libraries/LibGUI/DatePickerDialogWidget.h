/*
 * Copyright (c) 2023, David Ganz <david.g.ganz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGUI/Widget.h>

namespace GUI {

class DatePickerDialogWidget : public Widget {
    C_OBJECT_ABSTRACT(DatePickerDialogWidget)

public:
    static ErrorOr<NonnullRefPtr<DatePickerDialogWidget>> try_create();
    virtual ~DatePickerDialogWidget() override = default;

private:
    DatePickerDialogWidget() = default;
};

}
