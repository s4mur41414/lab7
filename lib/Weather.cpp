#include "Weather.h"

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

std::string ShowDayOfWeek(struct tm* now) {
    switch (now->tm_wday) {
    case 0: 
        return "Sunday";
    case 1:
        return "Monday";
    case 2:
        return "Tuesday";
    case 3:
        return "Wednesday";
    case 4:
        return "Thursday";
    case 5:
        return "Friday";
    case 6:
        return "Saturday";
    }
}

std::string ShowMonth(struct tm* now) {
    switch (now->tm_mon) {
    case 0:
        return "January";
    case 1:
        return "February";
    case 2:
        return "March";
    case 3:
        return "April";
    case 4:
        return "May";
    case 5:
        return "June";
    case 6:
        return "July";
    case 7:
        return "August";
    case 8:
        return "September";
    case 9:
        return "October";
    case 10:
        return "November";
    case 11:
        return "December";
    }
}

void ShowDate(int32_t index) {
    std::string ans;
    time_t time_ = time(NULL);
    time_+= index * kSecondsInDay;
    struct tm* now = localtime(&time_);
    std::cout << now->tm_mday 
        << ' ' 
        << ShowMonth(now) 
        << ' ' 
        << now->tm_year + 1900 
        << ", " 
        << ShowDayOfWeek(now)
        << ":\n";
}

void ShowWeatherCode(int32_t code) {
    if (code == 0) {
        std::cout << "Clear Sky" << '\n';
    }
    else if (code == 2) {
        std::cout << "Partly Cloudly" << '\n';
    }
    else if (code == 3 || code == 1) {
        std::cout << "Overcast" << '\n';
    }
    else if (code == 45 || code == 48) {
        std::cout << "Fog" << '\n';
    }
    else if (code == 51 || code == 53 || code == 55) {
        std::cout << "Drizzle" << '\n';
    }
    else if (code == 56 || code == 57) {
        std::cout << "Freezing Drizzle" << '\n';
    }
    else if (code == 61 || code == 63 || code == 65) {
        std::cout << "Rain" << '\n';
    }
    else if (code == 66 || code == 67) {
        std::cout << "Freezing Rain" << '\n';
    }
    else if (code == 71 || code == 73 || code == 75) {
        std::cout << "Snow fall" << '\n';
    }
    else if (code == 77) {
        std::cout << "Snow Grains" << '\n';
    }
    else if (code == 80 || code == 81 || code == 82) {
        std::cout << "Rain showers" << '\n';
    }
    else if (code == 85 || code == 86) {
        std::cout << "Snow showers" << '\n';
    }
    else if (code == 95 || code == 96 || code == 99) {
        std::cout << "ThunderStorm" << '\n';
    }
}

void ShowWeather(int32_t index, City& curr_city) {
    ShowWeatherCode(curr_city.weather_[index].weather_code);
    std::cout << "temperature: " 
        << curr_city.weather_[index].temperature
        << '\n';
    std::cout << "apparent_temperature: "
        << curr_city.weather_[index].apparent_temperature
        << '\n';
    std::cout << "wind_speed: "
        << curr_city.weather_[index].wind_speed_max
        << "-"
        << curr_city.weather_[index].wind_speed_min
        << '\n';
    std::cout << "relativehumidity "
        << curr_city.weather_[index].relativehumidity
        << '\n';
      
}

void WeatherForecast::ShowWeatherForecast(int32_t curr_city) {
    std::cout << "Weather in " + cities_[curr_city].name_ << ":\n";
    for (int i = 0; i < cities_[curr_city].days_; ++i) {
        ShowDate(i);
        std::cout << "\n       MORNING\n";
        ShowWeather(i * 4 + 0, cities_[curr_city]);
        std::cout << "\n       DAY\n";
        ShowWeather(i * 4 + 1, cities_[curr_city]);
        std::cout << "\n       EVENING\n";
        ShowWeather(i * 4 + 2, cities_[curr_city]);
        std::cout << "\n       NIGHT\n";
        ShowWeather(i * 4 + 3, cities_[curr_city]);
        std::cout << '\n';
    }
    
}
