#pragma once
#ifdef __cplusplus

namespace AP_Text {
enum class TextColor : char {
    COLOR_DEFAULT = 0,
    COLOR_ERROR,
    COLOR_LOG,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_CYAN,
    COLOR_MAGENTA,
    COLOR_SLATEBLUE,
    COLOR_PLUM,
    COLOR_SALMON,
    COLOR_WHITE,
    COLOR_ORANGE,
    COLOR_GRAY
};

struct ColoredTextNode {
    std::string text;
    AP_Text::TextColor color;
};
} // namespace AP_Text

#endif