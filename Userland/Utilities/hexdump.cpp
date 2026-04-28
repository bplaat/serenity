/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, Eli Youngs <eli.m.youngs@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Array.h>
#include <AK/CharacterTypes.h>
#include <LibCore/ArgsParser.h>
#include <LibCore/File.h>
#include <LibCore/System.h>
#include <LibMain/Main.h>
#include <string.h>

static constexpr size_t LINE_LENGTH_BYTES = 16;

enum class State {
    Print,
    PrintFiller,
    SkipPrint
};

enum class ColoringMode {
    None,
    Basic,
    Full,
};

static inline StringView get_ansi_color_for_byte_basic(u8 byte)
{
    if (byte == 0x00)
        return "\033[38;5;240m"sv;
    if (byte == 0x09 || byte == 0x0A || byte == 0x0B || byte == 0x0C || byte == 0x0D || byte == 0x20)
        return "\033[38;5;51m"sv;
    if (byte >= 0x20 && byte <= 0x7E)
        return "\033[38;5;32m"sv;
    if ((byte >= 0x01 && byte <= 0x08) || (byte >= 0x0E && byte <= 0x1F) || byte == 0x7F)
        return "\033[38;5;200m"sv;
    return "\033[38;5;208m"sv;
}

static inline StringView get_ansi_color_for_byte_full(u8 byte)
{
    // 16 colors, one per leading nibble (0x0_-0xF_)
    static constexpr StringView COLORS[] {
        "\033[38;5;240m"sv, // 0x00-0x0F - gray (null region)
        "\033[38;5;167m"sv, // 0x10-0x1F - red-orange
        "\033[38;5;179m"sv, // 0x20-0x2F - orange
        "\033[38;5;184m"sv, // 0x30-0x3F - yellow-orange
        "\033[38;5;185m"sv, // 0x40-0x4F - yellow
        "\033[38;5;184m"sv, // 0x50-0x5F - yellow-green
        "\033[38;5;142m"sv, // 0x60-0x6F - green
        "\033[38;5;108m"sv, // 0x70-0x7F - green-cyan
        "\033[38;5;87m"sv,  // 0x80-0x8F - cyan
        "\033[38;5;50m"sv,  // 0x90-0x9F - cyan-blue
        "\033[38;5;38m"sv,  // 0xA0-0xAF - blue
        "\033[38;5;32m"sv,  // 0xB0-0xBF - blue-purple
        "\033[38;5;105m"sv, // 0xC0-0xCF - purple
        "\033[38;5;135m"sv, // 0xD0-0xDF - purple-magenta
        "\033[38;5;168m"sv, // 0xE0-0xEF - magenta
        "\033[38;5;175m"sv, // 0xF0-0xFF - red-magenta
    };
    static_assert(AK::array_size(COLORS) == 16);
    return COLORS[byte >> 4];
}

static inline StringView get_ansi_reset() { return "\033[0m"sv; }

ErrorOr<int> serenity_main(Main::Arguments args)
{
    TRY(Core::System::pledge("stdio rpath"));

    Core::ArgsParser args_parser;
    StringView path;
    bool verbose = false;
    Optional<size_t> max_bytes;
    Optional<size_t> seek_to;
    StringView coloring_mode_str = "basic"sv;

    args_parser.add_positional_argument(path, "Input", "input", Core::ArgsParser::Required::No);
    args_parser.add_option(verbose, "Display all input data", "verbose", 'v');
    args_parser.add_option(max_bytes, "Truncate to a fixed number of bytes", nullptr, 'n', "bytes");
    args_parser.add_option(seek_to, "Seek to a byte offset", "seek", 's', "offset");
    args_parser.add_option(coloring_mode_str, "Coloring mode (none, basic, full)", "color", 'c', "mode");
    args_parser.parse(args);

    auto coloring_mode = ColoringMode::Basic;
    if (coloring_mode_str == "none"sv) {
        coloring_mode = ColoringMode::None;
    } else if (coloring_mode_str == "full"sv) {
        coloring_mode = ColoringMode::Full;
    }

    auto file = TRY(Core::File::open_file_or_standard_stream(path, Core::File::OpenMode::Read));
    if (seek_to.has_value())
        TRY(file->seek(seek_to.value(), SeekMode::SetPosition));

    auto print_line = [coloring_mode](Bytes line) {
        VERIFY(line.size() <= LINE_LENGTH_BYTES);
        for (size_t i = 0; i < LINE_LENGTH_BYTES; ++i) {
            if (i < line.size()) {
                auto byte = line[i];
                if (coloring_mode == ColoringMode::Basic) {
                    out("{}{:02x}{}", get_ansi_color_for_byte_basic(byte), byte, get_ansi_reset());
                } else if (coloring_mode == ColoringMode::Full) {
                    out("{}{:02x}{}", get_ansi_color_for_byte_full(byte), byte, get_ansi_reset());
                } else {
                    out("{:02x}", byte);
                }
                out(" ");
            } else {
                out("   ");
            }

            if (i == 7)
                out("  ");
        }

        out("  |");

        for (auto const& byte : line) {
            if (is_ascii_printable(byte))
                putchar(byte);
            else
                putchar('.');
        }

        putchar('|');
        putchar('\n');
    };

    Array<u8, BUFSIZ> contents;
    Bytes bytes;
    Bytes previous_line;
    static_assert(LINE_LENGTH_BYTES * 2 <= contents.size(), "Buffer is too small?!");
    size_t total_bytes_read = 0;

    auto state = State::Print;
    bool is_input_remaining = true;
    while (is_input_remaining) {
        auto bytes_to_read = contents.size() - bytes.size();

        if (max_bytes.has_value()) {
            auto bytes_remaining = max_bytes.value() - total_bytes_read;
            if (bytes_remaining < bytes_to_read) {
                bytes_to_read = bytes_remaining;
                is_input_remaining = false;
            }
        }

        bytes = contents.span().slice(0, bytes_to_read);
        bytes = TRY(file->read_some(bytes));

        total_bytes_read += bytes.size();

        if (bytes.size() < bytes_to_read) {
            is_input_remaining = false;
        }

        while (bytes.size() > LINE_LENGTH_BYTES) {
            auto current_line = bytes.slice(0, LINE_LENGTH_BYTES);
            bytes = bytes.slice(LINE_LENGTH_BYTES);

            if (verbose) {
                print_line(current_line);
                continue;
            }

            bool is_same_contents = (current_line == previous_line);
            if (!is_same_contents)
                state = State::Print;
            else if (is_same_contents && (state != State::SkipPrint))
                state = State::PrintFiller;

            // Coalesce repeating lines
            switch (state) {
            case State::Print:
                print_line(current_line);
                break;
            case State::PrintFiller:
                outln("*");
                state = State::SkipPrint;
                break;
            case State::SkipPrint:
                break;
            }
            previous_line = current_line;
        }
    }

    if (bytes.size() > 0)
        print_line(bytes);

    return 0;
}
