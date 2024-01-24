/*
 * Copyright (c) 2022, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2024, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteString.h>
#include <AK/Error.h>
#include <AK/String.h>
#include <AK/StringView.h>
#include <LibWeb/PixelUnits.h>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>

namespace Ladybird {

AK::ByteString ak_byte_string_from_qstring(QString const&);
String ak_string_from_qstring(QString const&);
QString qstring_from_ak_string(StringView);

Web::DevicePixelRect qrect_to_device_pixel_rect(QRect, float device_pixel_ratio);
QRect device_pixel_rect_to_qrect(Web::DevicePixelRect, float device_pixel_ratio);

Web::DevicePixelSize qsize_to_device_pixel_size(QSize, float device_pixel_ratio);
QSize device_pixel_size_to_qsize(Web::DevicePixelSize, float device_pixel_ratio);

Web::DevicePixelPoint qpoint_to_device_pixel_point(QPointF, float device_pixel_ratio);
QPointF device_pixel_point_to_qpoint(Web::DevicePixelPoint, float device_pixel_ratio);

}
