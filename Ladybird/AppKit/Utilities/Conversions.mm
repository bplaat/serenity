/*
 * Copyright (c) 2023, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#import <Utilities/Conversions.h>

namespace Ladybird {

String ns_string_to_string(NSString* string)
{
    auto const* utf8 = [string UTF8String];
    return MUST(String::from_utf8({ utf8, strlen(utf8) }));
}

NSString* string_to_ns_string(StringView string)
{
    return [[NSString alloc] initWithData:string_to_ns_data(string) encoding:NSUTF8StringEncoding];
}

NSData* string_to_ns_data(StringView string)
{
    return [NSData dataWithBytes:string.characters_without_null_termination() length:string.length()];
}

NSDictionary* deserialize_json_to_dictionary(StringView json)
{
    auto* ns_json = string_to_ns_string(json);
    auto* json_data = [ns_json dataUsingEncoding:NSUTF8StringEncoding];

    NSError* error = nil;
    NSDictionary* dictionary = [NSJSONSerialization JSONObjectWithData:json_data
                                                               options:0
                                                                 error:&error];

    if (!dictionary) {
        NSLog(@"Error deserializing DOM tree: %@", error);
    }

    return dictionary;
}

Web::DevicePixelRect ns_rect_to_gfx_rect(NSRect rect, float device_pixel_ratio)
{
    return { rect.origin.x * device_pixel_ratio, rect.origin.y * device_pixel_ratio,
        rect.size.width * device_pixel_ratio, rect.size.height * device_pixel_ratio };
}

NSRect gfx_rect_to_ns_rect(Web::DevicePixelRect rect, float device_pixel_ratio)
{
    return NSMakeRect(rect.x().value() / device_pixel_ratio,
        rect.y().value() / device_pixel_ratio,
        rect.width().value() / device_pixel_ratio,
        rect.height().value() / device_pixel_ratio);
}

Web::DevicePixelSize ns_size_to_gfx_size(NSSize size, float device_pixel_ratio)
{
    return { size.width * device_pixel_ratio, size.height * device_pixel_ratio };
}

NSSize gfx_size_to_ns_size(Web::DevicePixelSize size, float device_pixel_ratio)
{
    return NSMakeSize(size.width().value() / device_pixel_ratio,
        size.height().value() / device_pixel_ratio);
}

Web::DevicePixelPoint ns_point_to_gfx_point(NSPoint point, float device_pixel_ratio)
{
    return { point.x * device_pixel_ratio, point.y * device_pixel_ratio };
}

NSPoint gfx_point_to_ns_point(Web::DevicePixelPoint point, float device_pixel_ratio)
{
    return NSMakePoint(point.x().value() / device_pixel_ratio,
        point.y().value() / device_pixel_ratio);
}

Gfx::Color ns_color_to_gfx_color(NSColor* color)
{
    auto rgb_color = [color colorUsingColorSpace:NSColorSpace.genericRGBColorSpace];
    if (rgb_color != nil)
        return {
            static_cast<u8>([rgb_color redComponent] * 255),
            static_cast<u8>([rgb_color greenComponent] * 255),
            static_cast<u8>([rgb_color blueComponent] * 255),
            static_cast<u8>([rgb_color alphaComponent] * 255)
        };
    return {};
}

NSColor* gfx_color_to_ns_color(Gfx::Color color)
{
    return [NSColor colorWithRed:(color.red() / 255.f)
                           green:(color.green() / 255.f)
                            blue:(color.blue() / 255.f)
                           alpha:(color.alpha() / 255.f)];
}

}
