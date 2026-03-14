#include "webdriverxx/webdriver.hpp"

int main() {
    webdriverxx::Driver driver{webdriverxx::Capabilities{}};
    driver.quit();
    return 0;
}
