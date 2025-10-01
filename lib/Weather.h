#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <ctime>
#include <conio.h>

#include "City.h"

const int kEscapeCode = 0x1b;
const int kSecondsInDay = 24 * 60 * 60;

std::string ShowMonth(struct tm* now);
std::string ShowDayOfWeek(struct tm* now);
void ShowWeather(int32_t index, City& curr_city);
void ShowDate(int32_t index);

class WeatherForecast {
public:
    WeatherForecast() = default;
    ~WeatherForecast() = default;

    bool GetWeatherForecast(char* filename);
    bool Work();
    void ShowWeatherForecast(int32_t curr_city);

    std::string Date(int day_offset) const;
    std::string TableHeading(int day_number) const;
    std::string TableBottom() const;

    std::vector<City> cities_;
    int current_city_;

private:
    uint16_t freq_;
    time_t start;
    time_t cur_time;
};
