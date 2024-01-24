/*
 * Copyright (c) 2023, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <AK/StringView.h>
#include <LibGfx/Color.h>
#include <LibWeb/PixelUnits.h>

#import <System/Cocoa.h>

namespace Ladybird {

String ns_string_to_string(NSString*);
NSString* string_to_ns_string(StringView);

NSData* string_to_ns_data(StringView);

NSDictionary* deserialize_json_to_dictionary(StringView);

Web::DevicePixelRect ns_rect_to_gfx_rect(NSRect, float device_pixel_ratio);
NSRect gfx_rect_to_ns_rect(Web::DevicePixelRect, float device_pixel_ratio);

Web::DevicePixelSize ns_size_to_gfx_size(NSSize, float device_pixel_ratio);
NSSize gfx_size_to_ns_size(Web::DevicePixelSize, float device_pixel_ratio);

Web::DevicePixelPoint ns_point_to_gfx_point(NSPoint, float device_pixel_ratio);
NSPoint gfx_point_to_ns_point(Web::DevicePixelPoint, float device_pixel_ratio);

Gfx::Color ns_color_to_gfx_color(NSColor*);
NSColor* gfx_color_to_ns_color(Gfx::Color);

}
