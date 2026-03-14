#include "webdriverxx/webdriver.hpp"

int main() {
    webdriverxx::Driver driver{webdriverxx::Capabilities{}};
    driver.navigateTo("https://duckduckgo.com");
    webdriverxx::Element element {driver.findElement(webdriverxx::LocationStrategy::CSS, "input[name='q']")};
    element.clear().sendKeys("Hello world").submit();
    int status {webdriverxx::waitUntil([&driver]{ return driver.getTitle() == "Hello world at DuckDuckGo"; }, 5000)};
    return !status;
}
