/*
 * Copyright (c) 2023, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGfx/Palette.h>
#include <LibWeb/DOM/Document.h>
#include <LibWeb/HTML/DecodedImageData.h>
#include <LibWeb/Page/Page.h>

namespace Web::SVG {

class SVGDecodedImageData final : public HTML::DecodedImageData {
    JS_CELL(SVGDecodedImageData, HTML::DecodedImageData);
    JS_DECLARE_ALLOCATOR(SVGDecodedImageData);

public:
    class SVGPageClient;
    class SVGStandalonePageClient;

    static ErrorOr<JS::NonnullGCPtr<SVGDecodedImageData>> create(JS::Realm&, JS::NonnullGCPtr<Page>, URL::URL const&, ByteBuffer encoded_svg);

    // For embedding SVG rendering without a host browser page, JS execution is disabled and rendering uses defaults from palette.
    static ErrorOr<JS::NonnullGCPtr<SVGDecodedImageData>> create_standalone(URL::URL const&, ByteBuffer encoded_svg, Gfx::Palette);

    static bool sniff(ReadonlyBytes);

    virtual ~SVGDecodedImageData() override;

    virtual RefPtr<Gfx::ImmutableBitmap> bitmap(size_t frame_index, Gfx::IntSize) const override;

    virtual Optional<CSSPixels> intrinsic_width() const override;
    virtual Optional<CSSPixels> intrinsic_height() const override;
    virtual Optional<CSSPixelFraction> intrinsic_aspect_ratio() const override;

    // FIXME: Support SVG animations. :^)
    virtual int frame_duration(size_t) const override { return 0; }
    virtual size_t frame_count() const override { return 1; }
    virtual size_t loop_count() const override { return 0; }
    virtual bool is_animated() const override { return false; }

    DOM::Document const& svg_document() const { return *m_document; }

    RefPtr<Gfx::Bitmap> render(Gfx::IntSize) const;

    virtual void visit_edges(Cell::Visitor& visitor) override;

private:
    SVGDecodedImageData(JS::NonnullGCPtr<Page>, JS::NonnullGCPtr<PageClient>, JS::NonnullGCPtr<DOM::Document>, JS::NonnullGCPtr<SVG::SVGSVGElement>, bool fill_viewport = false);

    mutable HashMap<Gfx::IntSize, NonnullRefPtr<Gfx::ImmutableBitmap>> m_cached_rendered_bitmaps;

    bool m_fill_viewport { false };

    JS::NonnullGCPtr<Page> m_page;
    JS::NonnullGCPtr<PageClient> m_page_client;

    JS::NonnullGCPtr<DOM::Document> m_document;
    JS::NonnullGCPtr<SVG::SVGSVGElement> m_root_element;
};

class SVGDecodedImageData::SVGPageClient final : public PageClient {
    JS_CELL(SVGDecodedImageData::SVGPageClient, PageClient);
    JS_DECLARE_ALLOCATOR(SVGDecodedImageData::SVGPageClient);

public:
    static JS::NonnullGCPtr<SVGPageClient> create(JS::VM& vm, Page& page)
    {
        return vm.heap().allocate_without_realm<SVGPageClient>(page);
    }

    virtual ~SVGPageClient() override = default;

    JS::NonnullGCPtr<Page> m_host_page;
    JS::GCPtr<Page> m_svg_page;

    virtual Page& page() override { return *m_svg_page; }
    virtual Page const& page() const override { return *m_svg_page; }
    virtual bool is_connection_open() const override { return false; }
    virtual Gfx::Palette palette() const override { return m_host_page->client().palette(); }
    virtual DevicePixelRect screen_rect() const override { return {}; }
    virtual double device_pixels_per_css_pixel() const override { return 1.0; }
    virtual CSS::PreferredColorScheme preferred_color_scheme() const override { return m_host_page->client().preferred_color_scheme(); }
    virtual CSS::PreferredContrast preferred_contrast() const override { return m_host_page->client().preferred_contrast(); }
    virtual CSS::PreferredMotion preferred_motion() const override { return m_host_page->client().preferred_motion(); }
    virtual void request_file(FileRequest) override { }
    virtual void paint_next_frame() override { }
    virtual void paint(DevicePixelRect const&, Gfx::Bitmap&, Web::PaintOptions = {}) override { }
    virtual void schedule_repaint() override { }
    virtual bool is_ready_to_paint() const override { return true; }

    virtual DisplayListPlayerType display_list_player_type() const override { return m_host_page->client().display_list_player_type(); }

private:
    explicit SVGPageClient(Page& host_page)
        : m_host_page(host_page)
    {
    }

    virtual void visit_edges(Visitor&) override;
};

// A minimal PageClient for embedding SVG rendering without a host browser page.
class SVGDecodedImageData::SVGStandalonePageClient final : public PageClient {
    JS_CELL(SVGDecodedImageData::SVGStandalonePageClient, PageClient);
    JS_DECLARE_ALLOCATOR(SVGDecodedImageData::SVGStandalonePageClient);

public:
    static JS::NonnullGCPtr<SVGStandalonePageClient> create(JS::VM& vm, Gfx::Palette palette)
    {
        return vm.heap().allocate_without_realm<SVGStandalonePageClient>(move(palette));
    }

    virtual ~SVGStandalonePageClient() override = default;

    JS::GCPtr<Page> m_svg_page;

    virtual Page& page() override { return *m_svg_page; }
    virtual Page const& page() const override { return *m_svg_page; }
    virtual bool is_connection_open() const override { return false; }
    virtual Gfx::Palette palette() const override { return m_palette; }
    virtual DevicePixelRect screen_rect() const override { return {}; }
    virtual double device_pixels_per_css_pixel() const override { return 1.0; }
    virtual CSS::PreferredColorScheme preferred_color_scheme() const override { return CSS::PreferredColorScheme::Light; }
    virtual CSS::PreferredContrast preferred_contrast() const override { return CSS::PreferredContrast::NoPreference; }
    virtual CSS::PreferredMotion preferred_motion() const override { return CSS::PreferredMotion::NoPreference; }
    virtual void request_file(FileRequest) override { }
    virtual void paint_next_frame() override { }
    virtual void paint(DevicePixelRect const&, Gfx::Bitmap&, Web::PaintOptions = {}) override { }
    virtual void schedule_repaint() override { }
    virtual bool is_ready_to_paint() const override { return true; }
    virtual DisplayListPlayerType display_list_player_type() const override { return DisplayListPlayerType::CPU; }

private:
    explicit SVGStandalonePageClient(Gfx::Palette palette)
        : m_palette(move(palette))
    {
    }

    virtual void visit_edges(Visitor&) override;

    Gfx::Palette m_palette;
};

}
