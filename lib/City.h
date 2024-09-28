#pragma once

#include <string>
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <ctime>

const int kNoErrorCode = 200;

struct Weather{
    Weather() = default;
    ~Weather() = default;

    int temperature = 0;
    int apparent_temperature = 0;
    int weather_code = 0;
    int wind_speed_min = 0;
    int wind_speed_max = 100;
    int relativehumidity = 0;

    void SetValue(nlohmann::json& data, int part_of_day);
    void SetValue(nlohmann::json& data);
};

class City {
public:

    City(const std::string& name,const int32_t days);
    bool GetCordinates();
    bool GetWeather();
    void SetWeather(nlohmann::json& data);
    std::vector<Weather> weather_;
    Weather cur_weather;
    std::string name_;
    int32_t max_days_;
    int32_t days_;

private:
    std::string latitude_;
    std::string longitude_;
};
