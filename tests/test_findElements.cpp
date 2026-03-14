#include "webdriverxx/webdriver.hpp"

int main() {
    webdriverxx::Driver driver{webdriverxx::Capabilities{}};
    driver.navigateTo("https://github.com/Infinage/cpp-experiments/tree/main/webdriverxx");
    auto xpathFiltered {driver.findElements(webdriverxx::LocationStrategy::Xpath, "//td[contains(text(), 'Done')]")};

    auto rows {driver.findElements(webdriverxx::LocationStrategy::CSS, "markdown-accessiblity-table tbody tr")};
    std::size_t completedEndpoints {0};
    for (const auto &row: rows) {
        std::string text {row.findElement(webdriverxx::LocationStrategy::CSS, "td:nth-child(4)").getElementText()};
        if (text == "Done") completedEndpoints++;
    }

    int status {xpathFiltered.size() == completedEndpoints};
    return !status;
}
