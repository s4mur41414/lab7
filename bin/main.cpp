#include <iostream>
#include <fstream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "lib/Weather.h"

int main(int argc, char** argv) {
    WeatherForecast weather_forecast;
    if (!weather_forecast.GetWeatherForecast(argv[1])) {
        return 1;
    }
    weather_forecast.Work();
}