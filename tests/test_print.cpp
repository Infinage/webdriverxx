#include "webdriverxx/webdriver.hpp"

#include <filesystem>

int main() {
    webdriverxx::Driver driver{webdriverxx::Capabilities{}};
    driver.navigateTo("https://github.com/Infinage");
    webdriverxx::PageOptions pageopts;
    driver.print("infinage.pdf");

    int status {std::filesystem::exists("infinage.pdf")};
    return !status;
}
