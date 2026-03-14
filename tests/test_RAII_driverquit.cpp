#include "webdriverxx/webdriver.hpp"
#include <exception>

int main() {
    // Expected to fail, see if browser still closes correctly
    int status {true};
    try {
        webdriverxx::Driver driver{webdriverxx::Capabilities{}};
        driver.findElement(webdriverxx::LocationStrategy::CSS, "#404");
    } catch(std::exception &ex) { 
        status &= std::strcmp(ex.what(), "Webdriver not in ready state") != 0;
    }

    return !status;
}
