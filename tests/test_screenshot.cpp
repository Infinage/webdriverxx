#include "webdriverxx/webdriver.hpp"

#include <filesystem>

int main() {
    webdriverxx::Driver driver{webdriverxx::Capabilities{}};
    driver.navigateTo("https://github.com/Infinage");
    driver.save_screenshot("screenshot-full.png");
    driver.setTimeouts({.implicit=30 * 1000});
    webdriverxx::Element element {driver.findElement(webdriverxx::LocationStrategy::CSS, "img.avatar")};
    element.save_screenshot("screenshot.png");

    int status {std::filesystem::exists("screenshot.png") && std::filesystem::exists("screenshot-full.png")};
    return !status;
}
