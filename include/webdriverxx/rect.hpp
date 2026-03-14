#pragma once

#include <optional>

#include "nlohmann/json.hpp"

namespace webdriverxx {
    using Json = nlohmann::json;

    struct Rect {
        std::optional<int> x, y, width, height; 

        Rect(const Json &json_) {
            if (json_.contains("x")) x = json_.at("x");
            if (json_.contains("y")) y = json_.at("y");
            if (json_.contains( "width"))  width = json_.at( "width");
            if (json_.contains("height")) height = json_.at("height");
        }

        operator Json() const {
            Json object = Json::object();
            if (x) object["x"] = *x;
            if (y) object["y"] = *y;
            if (width)  object[ "width"] = *width;
            if (height) object["height"] = *height;
            return object;
        }
    };
}
