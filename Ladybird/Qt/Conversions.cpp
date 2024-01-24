/*
 * Copyright (c) 2022, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2024, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Conversions.h"

namespace Ladybird {

AK::ByteString ak_byte_string_from_qstring(QString const& qstring)
{
    return AK::ByteString(qstring.toUtf8().data());
}

String ak_string_from_qstring(QString const& qstring)
{
    auto utf8_data = qstring.toUtf8();
    return MUST(String::from_utf8(StringView(utf8_data.data(), utf8_data.size())));
}

QString qstring_from_ak_string(StringView ak_string)
{
    return QString::fromUtf8(ak_string.characters_without_null_termination(), static_cast<qsizetype>(ak_string.length()));
}

Web::DevicePixelRect qrect_to_device_pixel_rect(QRect rect, float device_pixel_ratio) {
    return { rect.x() * device_pixel_ratio, rect.y() * device_pixel_ratio, rect.width() * device_pixel_ratio, rect.height() * device_pixel_ratio };
}

QRect device_pixel_rect_to_qrect(Web::DevicePixelRect rect, float device_pixel_ratio) {
    return QRect(QPoint(rect.x().value(), rect.y().value()) / device_pixel_ratio, QSize(rect.width().value(), rect.height().value()) / device_pixel_ratio);
}

Web::DevicePixelSize qsize_to_device_pixel_size(QSize size, float device_pixel_ratio) {
    return { size.width() * device_pixel_ratio, size.height() * device_pixel_ratio };
}

QSize device_pixel_size_to_qsize(Web::DevicePixelSize size, float device_pixel_ratio) {
    return QSize(size.width().value(), size.height().value()) / device_pixel_ratio;
}

Web::DevicePixelPoint qpoint_to_device_pixel_point(QPointF point, float device_pixel_ratio) {
    return { point.x() * device_pixel_ratio, point.y() * device_pixel_ratio };
}

QPointF device_pixel_point_to_qpoint(Web::DevicePixelPoint point, float device_pixel_ratio) {
    return QPointF(point.x().value(), point.y().value()) / device_pixel_ratio;
}

}
