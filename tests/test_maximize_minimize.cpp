#include "webdriverxx/webdriver.hpp"

int main() {
    webdriverxx::Driver driver{webdriverxx::Capabilities{}};
    driver.maximize();
    driver.minimize();
    return 0;
}
