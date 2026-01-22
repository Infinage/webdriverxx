#pragma once

#include "webdriver.hpp"

#include <cstdlib>
#include <stdexcept>

inline std::string getEnv(const std::string &key) {
    const char* envVal {std::getenv(key.c_str())};
    if (!envVal)
        throw std::runtime_error('`' + key + "` env variable not set.");
    return envVal;
}

inline webdriverxx::Browsers resolveBrowser(const std::string &browserName) {
    if (browserName == "FIREFOX") return webdriverxx::Browsers::Firefox;
    else if (browserName == "CHROME") return webdriverxx::Browsers::Chrome;
    else if (browserName == "MSEDGE") return webdriverxx::Browsers::MSEdge;
    else throw std::runtime_error('`' + browserName + "` is not supported.");
}

const std::string BROWSER_NAME {getEnv({"BROWSER"})};
const webdriverxx::Browsers BROWSER_TYPE {resolveBrowser(BROWSER_NAME)};
const std::string BROWSER_BINARY {getEnv("BROWSER_BINARY")};
const std::string PORT {getEnv(BROWSER_NAME + "_PORT")};
