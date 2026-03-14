#include "webdriverxx/webdriver.hpp"

int main() {
    webdriverxx::Driver driver{webdriverxx::Capabilities{}};
    try {
        driver.findElement(webdriverxx::LocationStrategy::CSS, "#404");
    } catch (webdriverxx::APIError &ex) {
        return 0;
    }
    return 1;
}
