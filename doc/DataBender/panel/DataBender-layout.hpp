#pragma once

#include <rack.hpp>

// Measured from the supplied reference image, not manufacturer drill data.
// Coordinates use the top-left of the 71.12 mm x 128.5 mm panel.
namespace data_bender_panel {

inline constexpr float widthMm = 71.12f;
inline constexpr float heightMm = 128.5f;
inline constexpr int widthHp = 14;

struct Position {
    float xMm;
    float yMm;
    rack::math::Vec px() const {
        return rack::window::mm2px(rack::math::Vec{xMm, yMm});
    }
};

inline constexpr Position TIME_PARAM{13.919200f, 19.893517f};
inline constexpr Position REPEATS_PARAM{8.636000f, 45.896464f};
inline constexpr Position MIX_PARAM{8.636000f, 66.825049f};
inline constexpr Position BEND_PARAM{27.228800f, 60.841847f};
inline constexpr Position BREAK_PARAM{45.415200f, 60.791356f};
inline constexpr Position CORRUPT_PARAM{63.601600f, 60.589391f};
inline constexpr Position SHIFT_BUTTON_PARAM{41.046400f, 21.080059f};
inline constexpr Position CLOCK_BUTTON_PARAM{52.222400f, 21.080059f};
inline constexpr Position MODE_BUTTON_PARAM{63.398400f, 21.080059f};
inline constexpr Position BEND_BUTTON_PARAM{29.870400f, 40.039489f};
inline constexpr Position BREAK_BUTTON_PARAM{41.046400f, 40.039489f};
inline constexpr Position CORRUPT_BUTTON_PARAM{52.222400f, 40.039489f};
inline constexpr Position FREEZE_BUTTON_PARAM{63.398400f, 40.039489f};
inline constexpr Position SHIFT_LIGHT{40.970200f, 14.844401f};
inline constexpr Position CLOCK_LIGHT{52.146200f, 14.844401f};
inline constexpr Position MODE_LIGHT{63.322200f, 14.844401f};
inline constexpr Position BEND_LIGHT{29.794200f, 33.778585f};
inline constexpr Position BREAK_LIGHT{40.970200f, 33.778585f};
inline constexpr Position CORRUPT_LIGHT{52.146200f, 33.778585f};
inline constexpr Position FREEZE_LIGHT{63.322200f, 33.778585f};
inline constexpr Position CLOCK_INPUT{8.509000f, 82.881238f};
inline constexpr Position BEND_CV_INPUT{21.894800f, 82.881238f};
inline constexpr Position BREAK_CV_INPUT{35.712400f, 82.881238f};
inline constexpr Position CORRUPT_CV_INPUT{49.530000f, 82.881238f};
inline constexpr Position FREEZE_INPUT{63.449200f, 82.881238f};
inline constexpr Position LEFT_INPUT{8.509000f, 96.741061f};
inline constexpr Position BEND_GATE_INPUT{21.894800f, 96.741061f};
inline constexpr Position BREAK_GATE_INPUT{35.712400f, 96.741061f};
inline constexpr Position CORRUPT_GATE_INPUT{49.530000f, 96.741061f};
inline constexpr Position LEFT_OUTPUT{63.449200f, 96.741061f};
inline constexpr Position RIGHT_INPUT{8.509000f, 110.575639f};
inline constexpr Position MIX_CV_INPUT{21.894800f, 110.575639f};
inline constexpr Position REPEATS_CV_INPUT{35.712400f, 110.575639f};
inline constexpr Position TIME_CV_INPUT{49.530000f, 110.575639f};
inline constexpr Position RIGHT_OUTPUT{63.449200f, 110.575639f};

inline constexpr Position mountingHoles[] = {
    {7.696200f, 3.079961f},
    {63.627000f, 3.079961f},
    {7.696200f, 125.091847f},
    {63.627000f, 125.091847f},
};

} // namespace data_bender_panel
