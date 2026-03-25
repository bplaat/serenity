/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2021, Linus Groh <linusg@serenityos.org>
 * Copyright (c) 2021, Mohsan Ali <mohsan0073@gmail.com>
 * Copyright (c) 2022, Mustafa Quraish <mustafa@serenityos.org>
 * Copyright (c) 2022, the SerenityOS developers.
 * Copyright (c) 2023, Caoimhe Byrne <caoimhebyrne06@gmail.com>
 * Copyright (c) 2023, MacDue <macdue@dueutil.tech>
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibCore/Timer.h>
#include <LibGUI/AbstractZoomPanWidget.h>
#include <LibGUI/Painter.h>
#include <LibImageDecoderClient/Client.h>

namespace ImageViewer {

class Image : public RefCounted<Image> {
public:
    static NonnullRefPtr<Image> create(Gfx::Bitmap& bitmap, Gfx::FloatPoint scale) { return adopt_ref(*new Image(bitmap, scale)); }

    Gfx::IntSize size() const { return { round(m_bitmap->size().width() * m_scale.x()), round(m_bitmap->size().height() * m_scale.y()) }; }
    Gfx::IntRect rect() const { return { {}, size() }; }

    void flip(Gfx::Orientation);
    void rotate(Gfx::RotationDirection);

    void draw_into(Gfx::Painter&, Gfx::IntRect const& dest, Gfx::ScalingMode) const;

    ErrorOr<NonnullRefPtr<Gfx::Bitmap>> bitmap(Optional<Gfx::IntSize>) const { return m_bitmap; }

private:
    Image(Gfx::Bitmap& bitmap, Gfx::FloatPoint scale)
        : m_bitmap(bitmap)
        , m_scale(scale)
    {
    }

    NonnullRefPtr<Gfx::Bitmap> m_bitmap;
    Gfx::FloatPoint m_scale;
};

class ViewWidget final : public GUI::AbstractZoomPanWidget {
    C_OBJECT(ViewWidget)
public:
    enum Directions {
        First,
        Back,
        Forward,
        Last
    };

    virtual ~ViewWidget() override = default;

    Image const* image() const { return m_image.ptr(); }
    bool is_vector_image() const { return m_is_vector_image; }
    String const& path() const { return m_path; }
    void set_toolbar_height(int height) { m_toolbar_height = height; }
    int toolbar_height() { return m_toolbar_height; }
    bool scaled_for_first_image() { return m_scaled_for_first_image; }
    void set_scaled_for_first_image(bool val) { m_scaled_for_first_image = val; }
    void set_path(String const& path);
    void resize_window();
    void scale_image_for_window();
    void set_scaling_mode(Gfx::ScalingMode);

    bool is_next_available() const;
    bool is_previous_available() const;

    void clear();
    void flip(Gfx::Orientation);
    void rotate(Gfx::RotationDirection);
    void navigate(Directions);
    void open_file(String const&, Core::File&);

    Function<void()> on_doubleclick;
    Function<void(const GUI::DropEvent&)> on_drop;

    Function<void(Image*)> on_image_change;

private:
    ViewWidget();
    virtual void doubleclick_event(GUI::MouseEvent&) override;
    virtual void paint_event(GUI::PaintEvent&) override;
    virtual void mousedown_event(GUI::MouseEvent&) override;
    virtual void mouseup_event(GUI::MouseEvent&) override;
    virtual void drag_enter_event(GUI::DragEvent&) override;
    virtual void drop_event(GUI::DropEvent&) override;
    virtual void resize_event(GUI::ResizeEvent&) override;
    virtual void handle_relayout(Gfx::IntRect const&) override;

    void set_image(Image const* image);
    void animate();
    void clear_without_client();
    void do_vector_redecode();
    Vector<ByteString> load_files_from_directory(ByteString const& path) const;
    ErrorOr<void> try_open_file(String const&, Core::File&);

    String m_path;
    RefPtr<Image> m_image;
    ByteBuffer m_encoded_data;
    Optional<ByteString> m_mime_type;
    bool m_is_vector_image { false };

    struct Animation {
        struct Frame {
            RefPtr<Image> image;
            int duration { 0 };
        };

        size_t loop_count { 0 };
        Vector<Frame> frames;
    };

    Optional<Animation> m_animation;

    size_t m_current_frame_index { 0 };
    size_t m_loops_completed { 0 };
    NonnullRefPtr<Core::Timer> m_timer;
    NonnullRefPtr<ImageDecoderClient::Client> m_image_decoder_client;
    NonnullRefPtr<Core::Timer> m_vector_redecode_timer;
    u32 m_decode_generation { 0 };

    int m_toolbar_height { 28 };
    bool m_scaled_for_first_image { false };
    Vector<ByteString> m_files_in_same_dir;
    Optional<size_t> m_current_index;
    Gfx::ScalingMode m_scaling_mode { Gfx::ScalingMode::BoxSampling };
};

}
