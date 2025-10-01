#include "Weather.h"
#include "pictures.h"

bool WeatherForecast::GetWeatherForecast(char* filename) {
    std::ifstream json_file_stream(filename, std::ifstream::binary);
    if (!json_file_stream) {
        std::cerr << "Error: no file " << filename << '\n';
        return false;
    }
    nlohmann::json json_data = nlohmann::json::parse(json_file_stream, nullptr, false);

    int64_t count_of_cities = 0;
    for (int i = 0; i < json_data["cities"].size(); ++i) {
        City city(json_data["cities"][i]["name"], json_data["cities"][i]["days"].get<int>());
        if (!city.GetCordinates()) {
            std::cout << "Warning: can't find city " << city.name_ << '\n';
            continue;
        }
        cities_.push_back(city);
        ++count_of_cities;
    }
    freq_ = json_data["frequency"].get<int>();

    if (!count_of_cities) {
        std::cerr << "Error: no cities in file " << filename << '\n';
        return false;
    }

    if (freq_ <= 0) {
        std::cerr << "Error: freq <= 0\n";
        return false;
    }

    return true;
}

bool WeatherForecast::Work() {

    // Установка шрифта, поддерживающего Unicode
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof(fontInfo);
    GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
    wcscpy_s(fontInfo.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
    for (int i = 0; i < cities_.size(); ++i) {
        cities_[i].GetWeather();
    }
    current_city_ = 0;

    bool was_error = false;
    time_t start, end;
    time(&start);
    system("cls");
    ShowWeatherForecast(current_city_);
    for (;;) {
        if (!_kbhit()) {
            time(&end);
            if (difftime(end, start) >= freq_) {
                start = end;
                system("cls");
                ShowWeatherForecast(current_city_);
            }
            continue;
        }

        char input_symbol = _getch();
        if (input_symbol == 'n') {
            current_city_ = (current_city_ + 1) % cities_.size();
            cities_[current_city_].GetWeather();
        }
        else if (input_symbol == 'p') {
            current_city_ = (current_city_ == 0) ?
                (cities_.size() - 1) : (current_city_ - 1) % cities_.size();
        }
        else if (input_symbol == '+') {
            ++cities_[current_city_].days_;
            if (cities_[current_city_].days_ == 14) {
                --cities_[current_city_].days_;
            }
            if (cities_[current_city_].days_ > cities_[current_city_].max_days_) {
                ++cities_[current_city_].max_days_;
                cities_[current_city_].GetWeather();
            }
        }
        else if (input_symbol == '-') {
            cities_[current_city_].days_ = 1 < (cities_[current_city_].days_ - 1) ?
                cities_[current_city_].days_ - 1 : 1;
        }
        else if (input_symbol == kEscapeCode) {
            return true;
        }
        else {
            continue;
        }
        system("cls");
        ShowWeatherForecast(current_city_);
    }
    return true;
}

std::string GetDayOfWeekEn(struct tm* now) {
    switch (now->tm_wday) {
    case 0: return "Sun";
    case 1: return "Mon";
    case 2: return "Tue";
    case 3: return "Wed";
    case 4: return "Thu";
    case 5: return "Fri";
    case 6: return "Sat";
    default: return "";
    }
}

std::string GetMonthEn(struct tm* now) {
    switch (now->tm_mon) {
    case 0: return "Jan";
    case 1: return "Feb";
    case 2: return "Mar";
    case 3: return "Apr";
    case 4: return "May";
    case 5: return "Jun";
    case 6: return "Jul";
    case 7: return "Aug";
    case 8: return "Sep";
    case 9: return "Oct";
    case 10: return "Nov";
    case 11: return "Dec";
    default: return "";
    }
}

std::string GetWeatherIcon(int32_t code) {
    if (code == 0) {
        std::string result;
        for (const auto& line : Images::sunny) {
            result += line + "\n";
        }
        return result;
    }
    else if (code == 1 || code == 2) {
        std::string result;
        for (const auto& line : Images::partly_cloudy) {
            result += line + "\n";
        }
        return result;
    }
    else if (code == 3) {
        std::string result;
        for (const auto& line : Images::cloudy) {
            result += line + "\n";
        }
        return result;
    }
    else if (code == 45 || code == 48) {
        std::string result;
        for (const auto& line : Images::freezing_fog) {
            result += line + "\n";
        }
        return result;
    }
    else if (code == 51 || code == 53 || code == 55) {
        std::string result;
        for (const auto& line : Images::light_rain) {
            result += line + "\n";
        }
        return result;
    }
    else if (code == 61 || code == 63 || code == 65) {
        std::string result;
        for (const auto& line : Images::heavy_rain) {
            result += line + "\n";
        }
        return result;
    }
    else if (code == 71 || code == 73 || code == 75 || code == 77 || code == 85 || code == 86) {
        std::string result;
        for (const auto& line : Images::snow) {
            result += line + "\n";
        }
        return result;
    }
    else if (code == 80 || code == 81 || code == 82) {
        std::string result;
        for (const auto& line : Images::light_rain) {
            result += line + "\n";
        }
        return result;
    }
    else if (code == 95 || code == 96 || code == 99) {
        std::string result;
        for (const auto& line : Images::heavy_rain) {
            result += line + "\n";
        }
        return result;
    }
    else {
        std::string result;
        for (const auto& line : Images::unluck) {
            result += line + "\n";
        }
        return result;
    }
}

std::string GetWeatherText(int32_t code) {
    if (code == 0) return "Clear";
    else if (code == 1) return "Mainly clear";
    else if (code == 2) return "Partly cloudy";
    else if (code == 3) return "Overcast";
    else if (code == 45 || code == 48) return "Fog";
    else if (code == 51) return "Light drizzle";
    else if (code == 53) return "Moderate drizzle";
    else if (code == 55) return "Heavy drizzle";
    else if (code == 61) return "Light rain";
    else if (code == 63) return "Moderate rain";
    else if (code == 65) return "Heavy rain";
    else if (code == 71) return "Light snow";
    else if (code == 73) return "Moderate snow";
    else if (code == 75) return "Heavy snow";
    else if (code == 77) return "Snow grains";
    else if (code == 80) return "Light showers";
    else if (code == 81) return "Moderate showers";
    else if (code == 82) return "Heavy showers";
    else if (code == 85) return "Light snow";
    else if (code == 86) return "Heavy snow";
    else if (code == 95) return "Thunderstorm";
    else if (code == 96 || code == 99) return "Thunderstorm";
    else return "Unknown";
}

std::string FormatCell(const std::string& content, int width) {
    std::string result = content;
    if (result.length() > width) {
        result = result.substr(0, width);
    }
    while (result.length() < width) {
        result += " ";
    }
    return result;
}

std::string WeatherForecast::Date(int day_offset) const {
    time_t time_ = time(NULL);
    time_ += day_offset * kSecondsInDay;
    struct tm* now = localtime(&time_);
    return GetDayOfWeekEn(now) + " " + std::to_string(now->tm_mday) + " " + GetMonthEn(now);
}

std::string WeatherForecast::TableHeading(int day_number) const {
    std::string date = Date(day_number);
    if (date.size() == 9) {

        return "\n"
            "                                                          +───────────+\n"
            "────────────────────────────────┬─────────────────────────┤ " + date + " ├─────────────────────────┬────────────────────────────────\n"
            "            Night               │            Morning      +───────────+      Noon               │            Evening            |\n"
            "────────────────────────────────┼───────────────────────────────┼───────────────────────────────┼────────────────────────────────\n";
    }
    else {
        return "\n"
            "                                                          +────────────+\n"
            "────────────────────────────────┬─────────────────────────┤ " + date + " ├────────────────────────┬────────────────────────────────\n"
            "            Night               │            Morning      +────────────+      Noon              │            Evening            |\n"
            "────────────────────────────────┼───────────────────────────────┼───────────────────────────────┼────────────────────────────────\n";
    }
}

std::string WeatherForecast::TableBottom() const {
    return "────────────────────────────────┴───────────────────────────────┴───────────────────────────────┴───────────────────────────────┴\n";
}

void WeatherForecast::ShowWeatherForecast(int32_t curr_city) {
    City& city = cities_[curr_city];

    // Header
    std::cout << "Weather Forecast: " << city.name_ << ", Russia\n\n";

    // Current weather with icon on the left
    if (!city.weather_.empty()) {
        Weather& current = city.cur_weather;
        std::string current_icon = GetWeatherIcon(current.weather_code);

        // Split icon into lines
        std::vector<std::string> icon_lines;
        std::string line;
        for (char c : current_icon) {
            if (c == '\n') {
                if (!line.empty()) {
                    icon_lines.push_back(line);
                    line.clear();
                }
            }
            else {
                line += c;
            }
        }
        if (!line.empty()) icon_lines.push_back(line);

        // Display current weather info on the left
        std::cout << "Current Weather:\n";
        for (const auto& icon_line : icon_lines) {
            std::cout << icon_line << "\n";
        }
        std::cout << GetWeatherText(current.weather_code) << "\n";
        std::cout << "Temperature: " << current.temperature << "°C (feels like " << current.apparent_temperature << "°C)\n";
        std::cout << "Wind: " << current.wind_speed_min << "-" << current.wind_speed_max << " km/h\n";
        std::cout << "Humidity: " << current.relativehumidity << "%\n\n";
    }

    // Daily forecast
    for (int day = 0; day < city.days_; ++day) {
        std::cout << TableHeading(day);

        // Get weather data for all parts of the day
        std::vector<Weather*> day_parts;
        for (int part = 0; part < 4; ++part) {
            int index = day * 4 + part;
            if (index < city.weather_.size()) {
                day_parts.push_back(&city.weather_[index]);
            }
            else {
                day_parts.push_back(nullptr);
            }
        }

        // Display weather icons
        std::vector<std::vector<std::string>> weather_icons;
        int max_icon_height = 0;

        for (int part = 0; part < 4; ++part) {
            std::vector<std::string> icon_lines;
            if (day_parts[part] != nullptr) {
                std::string icon = GetWeatherIcon(day_parts[part]->weather_code);
                std::string line;
                for (char c : icon) {
                    if (c == '\n') {
                        if (!line.empty()) {
                            icon_lines.push_back(line);
                            line.clear();
                        }
                    }
                    else {
                        line += c;
                    }
                }
                if (!line.empty()) icon_lines.push_back(line);
            }
            else {
                // Empty space for missing data
                icon_lines = std::vector<std::string>(5, "");
            }
            weather_icons.push_back(icon_lines);

            if (icon_lines.size() > max_icon_height) {
                max_icon_height = icon_lines.size();
            }
        }

        // Ensure all icons have the same number of lines
        for (auto& icon : weather_icons) {
            while (icon.size() < max_icon_height) {
                icon.push_back("");
            }
        }

        // Display icons line by line
        for (int line = 0; line < max_icon_height; ++line) {
            std::cout << "| ";
            for (int part = 0; part < 4; ++part) {
                std::string icon_line = weather_icons[part][line];
                std::cout << FormatCell(icon_line, 29) << " | ";
            }
            std::cout << "\n";
        }

        // Weather conditions
        std::cout << "| ";
        for (int part = 0; part < 4; ++part) {
            std::string text;
            if (day_parts[part] != nullptr) {
                text = GetWeatherText(day_parts[part]->weather_code);
            }
            else {
                text = "-";
            }
            std::cout << FormatCell(text, 29) << " | ";
        }
        std::cout << "\n";

        // Temperature
        std::cout << "| ";
        for (int part = 0; part < 4; ++part) {
            std::string temp;
            if (day_parts[part] != nullptr) {
                temp = std::to_string(day_parts[part]->temperature) +
                    "(" + std::to_string(day_parts[part]->apparent_temperature) + ") C";
            }
            else {
                temp = "(-)";
            }
            std::cout << FormatCell(temp, 29) << " | ";
        }
        std::cout << "\n";

        // Wind
        std::cout << "| ";
        for (int part = 0; part < 4; ++part) {
            std::string wind;
            if (day_parts[part] != nullptr) {
                wind = std::to_string(day_parts[part]->wind_speed_min) +
                    "-" + std::to_string(day_parts[part]->wind_speed_max) + " km/h";
            }
            else {
                wind = "(-)";
            }
            std::cout << FormatCell(wind, 29) << " | ";
        }
        std::cout << "\n";

        // Humidity
        std::cout << "| ";
        for (int part = 0; part < 4; ++part) {
            std::string humidity;
            if (day_parts[part] != nullptr) {
                humidity = std::to_string(day_parts[part]->relativehumidity) + "%";
            }
            else {
                humidity = "(-)";
            }
            std::cout << FormatCell(humidity, 29) << " | ";
        }
        std::cout << "\n";

        std::cout << TableBottom() << "\n";
    }

    // Controls legend
    std::cout << "Controls: n - next city, p - previous city\n";
    std::cout << "          + - increase days, - - decrease days\n";
    std::cout << "          ESC - exit\n";
}