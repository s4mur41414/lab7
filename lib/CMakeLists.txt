add_library(City City.cpp City.h)
add_library(Weather Weather.cpp Weather.h)

target_link_libraries(City PRIVATE 
    cpr::cpr 
    nlohmann_json::nlohmann_json
)
target_link_libraries(Weather PRIVATE 
    City
    cpr::cpr 
    nlohmann_json::nlohmann_json
)
