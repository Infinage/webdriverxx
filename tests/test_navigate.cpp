#include "webdriverxx/webdriver.hpp"

int main() {
    webdriverxx::Driver driver{webdriverxx::Capabilities{}};
    int status {driver.navigateTo("https://google.com").getTitle() == "Google"};
    return !status;
}
