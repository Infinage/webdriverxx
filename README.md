# WebDriver++

**WebDriver++** is a modern, lightweight C++ client for the Selenium WebDriver protocol.
It provides a clean, RAII-driven API for browser automation with minimal dependencies and a strong focus on correctness and ergonomics.

---

## Features

* **Modern C++ (C++23)** API design
* **Lightweight**: built on `cpp-httplib` and `nlohmann/json`
* **RAII-based session management**
* **Fluent, expressive API**
* **Cross-browser support**: Chrome, Firefox, Edge
* **Protocol-faithful** implementation of W3C WebDriver

---

## Installation

Using CPM:

```cmake
include(cmake/CPM.cmake)
CPMAddPackage("gh:infinage/webdriverxx@0.0.1")
```

Or simply include the headers directly.

---

## Quick Example

```cpp
#include "webdriver.hpp"

using namespace webdriverxx;
using namespace webdriverxx::enums;

int main() {
    Driver driver{
        Capabilities{Browsers::Chrome, "/usr/bin/google-chrome"}
            .headless(true)
            .startMaximized(true),
        "9515"
    };

    driver.navigateTo("https://duckduckgo.com");

    auto search = driver.findElement(LocationStrategy::CSS, "#searchbox_input");
    search.sendKeys("WebDriver++ C++").submit();
}
```

---

## Design Overview

### RAII Session Management

* A WebDriver **session is created in the `Driver` constructor**
* The session is **automatically closed in the destructor**
* You may also explicitly call `quit()`

```cpp
Driver driver{caps, "9515"};  // POST /session
// ...
driver.quit();               // DELETE /session
```

This guarantees correct cleanup even in the presence of exceptions.

---

## Runtime Assumptions

WebDriver++ is a **pure WebDriver client**, not a browser or driver manager.

### Assumptions

* A compatible WebDriver **(chromedriver, geckodriver, msedgedriver)** is already running
* A runnable browser binary is present on the system
* The user is responsible for:
  * Installing the browser
  * Installing the corresponding driver
  * Ensuring version compatibility

The library connects to an **existing WebDriver endpoint** via HTTP and does not attempt to manage external processes.

---

### Browser Capabilities

Capabilities are built fluently and converted internally to WebDriver JSON.

```cpp
Capabilities caps{Browsers::Firefox, "/usr/bin/firefox"};

caps.headless(true)
    .windowSize(900, 1400)
    .disableExtensions(true);
```

Browser-specific behavior is handled internally.

---

## User-Facing API

### Driver

| Function                             | Description               |
| ------------------------------------ | ------------------------- |
| `Driver(...)`                        | Create a session          |
| `status()`                           | Check WebDriver readiness |
| `quit()`                             | End session               |
| `navigateTo(url)`                    | Navigate to URL           |
| `back()` / `forward()` / `refresh()` | Navigation                |
| `getCurrentURL()`                    | Current URL               |
| `getTitle()`                         | Page title                |
| `getPageSource()`                    | HTML source               |

---

### Window & Frame Management

| Function                                     | Description             |
| -------------------------------------------- | ----------------------- |
| `getWindowHandle()`                          | Current window          |
| `getWindowHandles()`                         | All windows             |
| `newWindow(WindowType)`                      | Open tab/window         |
| `switchWindow(handle)`                       | Switch window           |
| `closeWindow()`                              | Close window            |
| `maximize()` / `minimize()` / `fullscreen()` | Window state            |
| `getWindowRect()` / `setWindowRect()`        | Geometry                |
| `switchFrame(index)`                         | Switch frame            |
| `switchFrame(element)`                       | Switch frame by element |
| `switchToParentFrame()`                      | Parent frame            |

---

### Element Interaction

| Function                        | Description            |
| ------------------------------- | ---------------------- |
| `findElement(strategy, value)`  | Find single element    |
| `findElements(strategy, value)` | Find multiple elements |
| `Element::click()`              | Click                  |
| `Element::sendKeys()`           | Send keys              |
| `Element::clear()`              | Clear                  |
| `Element::submit()`             | Submit                 |
| `Element::text()`               | Get text               |
| `Element::attribute(name)`      | Attribute              |
| `Element::css(property)`        | CSS value              |
| `Element::rect()`               | Element bounds         |

---

### Timeouts

```cpp
driver.setTimeouts({ .implicit = 5000, .pageLoad = 10000 });
```

| Function        | Description    |
| --------------- | -------------- |
| `getTimeouts()` | Fetch timeouts |
| `setTimeouts()` | Set timeouts   |

---

### Script Execution

```cpp
int result = driver.execute<int>("return 2 + 2;");
```

| Function                   | Description         |
| -------------------------- | ------------------- |
| `execute<T>(script, args)` | Execute sync script |

---

### Cookies

| Function             | Description   |
| -------------------- | ------------- |
| `getAllCookies()`    | Fetch cookies |
| `getCookie(name)`    | Get cookie    |
| `addCookie(cookie)`  | Add cookie    |
| `deleteCookie(name)` | Delete cookie |
| `deleteAllCookies()` | Clear cookies |

---

### Alerts

| Function                 | Description   |
| ------------------------ | ------------- |
| `dismissAlert(true)`     | Accept alert  |
| `dismissAlert(false)`    | Dismiss alert |
| `getAlertText()`         | Alert text    |
| `setAlertResponse(text)` | Send text     |

---

### Screenshots & Printing

| Function                | Description       |
| ----------------------- | ----------------- |
| `save_screenshot(file)` | Screenshot        |
| `print(file, options)`  | Print page to PDF |

---

## WebDriver Protocol Coverage

This library implements the **W3C WebDriver specification**.

| Area             | Status |
| ---------------- | ------ |
| Sessions         | ✅      |
| Navigation       | ✅      |
| Windows & Frames | ✅      |
| Elements         | ✅      |
| Cookies          | ✅      |
| Alerts           | ✅      |
| Screenshots      | ✅      |
| Printing         | ✅      |
| Execute Async    | ⏳      |
| Actions API      | ⏳      |

---

## Testing

Tests are browser backed and run against real WebDriver instances.

```sh
FIREFOX_PORT=1001 CHROME_PORT=1002 MSEDGE_PORT=1003 \
ctest --output-on-failure
```

Run a single browser:

```sh
CHROME_PORT=1002 ctest -L chrome
```

---

## Roadmap

* [ ] Actions API
* [ ] Execute Async

---

## License

This project is licensed under the MIT License.
