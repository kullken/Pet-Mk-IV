#include "modules.h"

#include <timer.h>

#include "engine_module.h"
#include "line_sensor_module.h"
#include "ultrasound_module.h"

namespace pet
{

Timer<4> g_timer{};

ConfigResult configure_modules()
{
    static pet::EngineModule engine_module{};
    if (!g_timer.register_module(&engine_module)) {
        return ConfigResult::TimerRegistrationError;
    }

    static pet::LineSensorModule line_follower_module{};
    if (!g_timer.register_module(&line_follower_module)) {
        return ConfigResult::TimerRegistrationError;
    }

    constexpr int kSensorCount = 3;
    constexpr int ultrasound_pins[kSensorCount] = {
        A0, 
        A1, 
        A2
    };
    constexpr const char* frame_ids[kSensorCount] = {
        "dist_sensor_right",
        "dist_sensor_middle",
        "dist_sensor_left"
    };
    static pet::UltrasoundModule<kSensorCount> ultrasound_module{ultrasound_pins, frame_ids};
    if (!g_timer.register_module(&ultrasound_module)) {
        return ConfigResult::TimerRegistrationError;
    }

    return ConfigResult::Success;
}

} // namespace pet
