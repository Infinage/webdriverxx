#include "webdriverxx/webdriver.hpp"
#include <iostream>

int main() {
    // Set the capabilities
    webdriverxx::Capabilities caps{};
    caps.userAgent("Webdriverxx").headless(true);

    webdriverxx::Driver driver{caps};
    int status {true};

    // Check useragent
    const std::string userAgent {driver.execute<std::string>("return navigator.userAgent;")};
    std::cerr << "User Agent: " << userAgent << '\n';
    status &= userAgent == "Webdriverxx";

    // Check running headless
    const bool headlessMode {driver.execute<bool>("return navigator.webdriver || window.innerWidth === 0;")};
    std::cerr << "Headless: " << headlessMode << '\n';
    status &= headlessMode;

    return !status;
}
