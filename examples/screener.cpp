#include "webdriverxx/webdriver.hpp"

int main() {
    namespace fs = std::filesystem;
    using namespace webdriverxx;
    using namespace webdriverxx::enums;

    // Remove screens if already exists, create new
    if (fs::exists("screens")) fs::remove_all("screens");
    fs::create_directory("screens");

    // Create a new webdriver
    // Ensure ENV: "BROWSER_TYPE", "BROWSER_PATH" and "DRIVER_PORT"
    auto caps = Capabilities{}.startMaximized(true).headless(false);
    Driver driver{caps};

    // Navigate to URL
    driver.navigateTo("https://www.screener.in");

    // Click on 'screens' (clickJS work around for element not interactable)
    driver.findElement(CSS, "a[href='/explore/']").clickJS();

    // Wait until loaded
    waitUntil([&]{ return driver.getTitle() == "Explore stock screens - Screener"; });

    // Iterate through all screens
    auto screenLinks {driver.findElements(CSS, ".screen-items a")};
    for (auto screenLink: screenLinks) {
        // Extract screen name from url
        auto screenName {screenLink.getElementAttribute("href")};
        screenName = screenName.substr(screenName.rfind('/', screenName.size() - 2) + 1);
        screenName.pop_back();

        // Click the link
        screenLink.click();

        // Print the page result
        auto pageopts = PageOptions().orientation(Landscape).shrinkToFit(false).pageScale(0.75);
        driver.print("screens/" + screenName + ".pdf", pageopts);
        
        // Navigate back
        driver.back();
    }
}
