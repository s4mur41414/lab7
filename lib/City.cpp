#include "City.h"

void Weather::SetValue(nlohmann::json& json_data, int part_of_day) {
    for (int i = 0; i < 6; ++i) {
        apparent_temperature += json_data["hourly"]["apparent_temperature"][part_of_day + i].get<int>();
        temperature += json_data["hourly"]["temperature_2m"][part_of_day + i].get<int>();
        relativehumidity += json_data["hourly"]["relativehumidity_2m"][part_of_day + i].get<int>();
        wind_speed_min = json_data["hourly"]["windspeed_10m"][part_of_day + i].get<int>() > wind_speed_min ?
            json_data["hourly"]["windspeed_10m"][part_of_day + i].get<int>() : wind_speed_min;
        wind_speed_max = json_data["hourly"]["windspeed_10m"][part_of_day + i].get<int>() < wind_speed_max ?
            json_data["hourly"]["windspeed_10m"][part_of_day + i].get<int>() : wind_speed_max;
    }

    apparent_temperature /= 6;
    temperature /= 6;
    relativehumidity /= 6;
    weather_code = json_data["hourly"]["weathercode"][part_of_day + 3].get<int>();
}

void Weather::SetValue(nlohmann::json& json_data) {
    apparent_temperature = json_data["current"]["apparent_temperature"].get<int>();
    temperature = json_data["current"]["temperature_2m"].get<int>();
    wind_speed_min = json_data["current"]["windspeed_10m"].get<int>();
    wind_speed_max = json_data["current"]["windspeed_10m"].get<int>();
    relativehumidity = json_data["current"]["relativehumidity_2m"].get<int>();
    weather_code = json_data["current"]["weathercode"].get<int>();
}

City::City(const std::string& name, int32_t days) :
    name_(name),
    days_(days),
    max_days_(days)
{
}

bool City::GetCordinates() {
    cpr::Response resp = cpr::Get(cpr::Url{ "https://api.api-ninjas.com/v1/city" },
        cpr::Header{ {"X-Api-Key", "your_key"} },
        cpr::Parameters{ {"name", name_} });

    if (resp.status_code != kNoErrorCode) {
        std::cerr << resp.error.message << '\n';

        return false;
    }

    nlohmann::json json_data = nlohmann::json::parse(resp.text, nullptr, false);
    latitude_ = json_data[0]["latitude"].dump();
    longitude_ = json_data[0]["longitude"].dump();

    return true;
}

void City::SetWeather(nlohmann::json& data) {
    for (int i = 0; i < days_; i++) {
        for (int j = 0; j < 4; j++) {
            Weather values;
            values.SetValue(data, 24 * i + 4 * j);
            weather_.push_back(values);
        }
    }
}


bool City::GetWeather() {
    weather_.clear();

    cpr::Response resp = cpr::Get(
        cpr::Url{ "https://api.open-meteo.com/v1/forecast" },
        cpr::Parameters{
            {"latitude", latitude_},
            {"longitude", longitude_},
            {"current", "temperature_2m"},
            {"current", "relativehumidity_2m"},
            {"current", "apparent_temperature"},
            {"current", "weathercode"},
            {"current", "windspeed_10m"},
        });

    if (resp.status_code != kNoErrorCode) {
        std::cerr << resp.error.message << '\n';

        return false;
    }

    nlohmann::json json_data = nlohmann::json::parse(resp.text, nullptr, false);
    cur_weather.SetValue(json_data);

    resp = cpr::Get(
        cpr::Url{ "https://api.open-meteo.com/v1/forecast" },
        cpr::Parameters{
            {"latitude", latitude_},
            {"longitude", longitude_},
            {"hourly", "temperature_2m"},
            {"hourly", "relativehumidity_2m"},
            {"hourly", "apparent_temperature"},
            {"hourly", "weathercode"},
            {"hourly", "windspeed_10m"},
            {"forecast_days", std::to_string(days_)},
        });

    if (resp.status_code != kNoErrorCode) {
        std::cout << resp.error.message << '\n';

        return false;
    }

    json_data = nlohmann::json::parse(resp.text, nullptr, false);
    SetWeather(json_data);

    return true;
}