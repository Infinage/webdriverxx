#pragma once

#include "capabilities.hpp"
#include "pageoptions.hpp"
#include "rect.hpp"
#include "cookie.hpp"
#include "timeout.hpp"
#include "element.hpp"
#include <stdexcept>

namespace webdriverxx {

    // Webdriver constants
    enum class WindowType {Tab, Window};
    namespace enums { using enum WindowType; }

    class Driver {
        private:
            const Capabilities capabilities;
            const std::string port, baseURL; 
            const std::string sessionId, sessionURL;
            bool running {false};

        private:
            std::string startSession() {
                if (!status()) throw std::runtime_error("Webdriver not in ready state");
                Json response = sendRequest(ApiMethod::Post, baseURL + "/session", static_cast<Json>(capabilities).dump());
                return response["value"]["sessionId"];
            }

        public:
            Driver(
                const Capabilities &cap_,
                const std::string  &port_,
                const std::string  &sessionId_  = ""
            ):
                capabilities(cap_), port(port_),
                baseURL("http://127.0.0.1:" + port), 
                sessionId(sessionId_.empty()? startSession(): sessionId_), 
                sessionURL(baseURL + "/session/" + sessionId) 
            { running = true; }

            ~Driver() { if (running) quit(); }

            bool status() {
                // Ignore errors and parse the errors as JSON
                Json response = sendRequest(ApiMethod::Get, baseURL + "/status", "{}", 200, true);
                if (response.value("status_code", 200) != 200) return false;
                else return response["value"]["ready"];
            }

            void quit() {
                sendRequest(ApiMethod::Delete, sessionURL);
                running = false;
            }

            Rect minimize() {
                Json response = sendRequest(ApiMethod::Post, sessionURL + "/window/minimize");
                return Rect{response["value"]};
            }

            Rect maximize() {
                Json response = sendRequest(ApiMethod::Post, sessionURL + "/window/maximize");
                return Rect{response["value"]};
            }

            Rect fullscreen() {
                Json response = sendRequest(ApiMethod::Post, sessionURL + "/window/fullscreen");
                return Rect{response["value"]};
            }

            Driver& navigateTo(const std::string &url) {
                Json payload {{ "url", url }};
                sendRequest(ApiMethod::Post, sessionURL + "/url", payload.dump());
                return *this;
            }

            Driver& back() {
                sendRequest(ApiMethod::Post, sessionURL + "/back");
                return *this;
            }

            Driver& forward() {
                sendRequest(ApiMethod::Post, sessionURL + "/forward");
                return *this;
            }

            Driver& refresh() {
                sendRequest(ApiMethod::Post, sessionURL + "/refresh");
                return *this;
            }

            Timeout getTimeouts() const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/timeouts");
                return { 
                    .script=response["value"]["script"], 
                    .pageLoad=response["value"]["pageLoad"], 
                    .implicit=response["value"]["implicit"] 
                };
            }
            
            Driver &setTimeouts(const Timeout &timeouts) {
                if (!timeouts.script && !timeouts.pageLoad && !timeouts.implicit)
                    throw std::invalid_argument("Atleast one timeout must be set.");

                Json payload = Json::object();
                if (timeouts.script) payload["script"] = *timeouts.script;
                if (timeouts.pageLoad) payload["pageLoad"] = *timeouts.pageLoad;
                if (timeouts.implicit) payload["implicit"] = *timeouts.implicit;

                sendRequest(ApiMethod::Post, sessionURL + "/timeouts", payload.dump());
                return *this;
            }

            Driver &save_screenshot(const std::string &ofile) {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/screenshot");
                std::ofstream imageFS {ofile, std::ios::binary};
                if (!imageFS) throw std::runtime_error("Failed to open file for writing: " + ofile);
                std::string decoded {Base64::base64Decode(response["value"])};
                imageFS.write(decoded.data(), static_cast<long>(decoded.size()));
                return *this;
            }

            std::string getCurrentURL() const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/url");
                return response["value"];
            }

            std::string getTitle() const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/title");
                return response["value"];
            }

            std::string getPageSource() const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/source");
                return response["value"];
            }

            Element findElement(const LocationStrategy &strategy, const std::string &criteria) const {
                std::string strategyKeyword;
                switch (strategy) {
                    case LocationStrategy::CSS: strategyKeyword = "css selector"; break;
                    case LocationStrategy::TagName: strategyKeyword = "tag name"; break;
                    case LocationStrategy::Xpath: strategyKeyword = "xpath"; break;
                }

                Json payload {{"using", strategyKeyword}, {"value", criteria}};
                Json response = sendRequest(ApiMethod::Post, sessionURL + "/element", payload.dump());
                return Element(
                    response["value"].begin().key(), 
                    response["value"].begin().value(), 
                    sessionURL
                );
            }

            std::vector<Element> findElements(const LocationStrategy &strategy, const std::string &criteria) const {
                std::string strategyKeyword;
                switch (strategy) {
                    case LocationStrategy::CSS: strategyKeyword = "css selector"; break;
                    case LocationStrategy::TagName: strategyKeyword = "tag name"; break;
                    case LocationStrategy::Xpath: strategyKeyword = "xpath"; break;
                }

                Json payload {{"using", strategyKeyword}, {"value", criteria}};
                Json response = sendRequest(ApiMethod::Post, sessionURL + "/elements", payload.dump());
                std::vector<Element> elements;
                std::transform(response["value"].begin(), response["value"].end(), std::back_inserter(elements), 
                    [&](const Json::value_type &ele) {
                        return Element{ele.begin().key(), ele.begin().value(), sessionURL};
                    }
                );
                return elements;
            }

            std::string getWindowHandle() const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/window");
                return response["value"];
            }

            Driver &closeWindow() {
                sendRequest(ApiMethod::Delete, sessionURL + "/window");
                return *this;
            }
            
            Driver &switchWindow(const std::string &windowId) {
                Json payload {{ "handle", windowId }};
                sendRequest(ApiMethod::Post, sessionURL + "/window", payload.dump());
                return *this;
            }

            std::vector<std::string> getWindowHandles() const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/window/handles");
                return {response["value"].begin(), response["value"].end()};
            }

            std::string newWindow(const WindowType &type) {
                Json payload {{ "type", type == WindowType::Window? "window": "tab" }};
                Json response = sendRequest(ApiMethod::Post, sessionURL + "/window/new", payload.dump());
                return response["value"]["handle"];
            }

            Driver &switchToParentFrame() {
                sendRequest(ApiMethod::Post, sessionURL + "/frame/parent");
                return *this;
            }

            Driver &switchFrame(const int index = -1) {
                Json payload;
                if (index < 0) payload = {{ "id", nullptr }};
                else payload = {{ "id", index }};
                sendRequest(ApiMethod::Post, sessionURL + "/frame", payload.dump());
                return *this;
            }

            Driver &switchFrame(const Element& element) {
                Json payload {{ 
                    "id", {{ element.elementRef, element.elementId }}
                }};
                sendRequest(ApiMethod::Post, sessionURL + "/frame", payload.dump());
                return *this;
            }

            Driver &dismissAlert(bool accept) {
                sendRequest(ApiMethod::Post, sessionURL + "/alert/" + (accept? "accept": "dismiss"));
                return *this;
            }

            std::string getAlertText() const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/alert/text");
                return response["value"];
            }

            Driver &setAlertResponse(const std::string &text) {
                Json payload = {{ "text", text }};
                sendRequest(ApiMethod::Post, sessionURL + "/alert/text", payload.dump());
                return *this;
            }

            template<typename T>
            T execute(const std::string &code, const Json &args = Json::array()) {
                Json payload = {{ "script", code }, { "args", args.is_array()? args: Json::array({args}) }};
                Json response = sendRequest(ApiMethod::Post, sessionURL + "/execute/sync", payload.dump());
                return response["value"].get<T>();
            }

            std::vector<Cookie> getAllCookies() const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/cookie");
                std::vector<Cookie> cookies;
                std::transform(response["value"].begin(), response["value"].end(), std::back_inserter(cookies), 
                    [&](const Json::value_type &cookieJson) { return Cookie{cookieJson}; }
                );
                return cookies;
            }

            Driver &deleteAllCookies() {
                sendRequest(ApiMethod::Delete, sessionURL + "/cookie");
                return *this;
            }

            Cookie getCookie(const std::string &name) const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/cookie/" + name);
                return Cookie{response["value"]};
            }

            Driver &addCookie(const Cookie &cookie) {
                Json payload = {{"cookie", static_cast<Json>(cookie) }};
                sendRequest(ApiMethod::Post, sessionURL + "/cookie", payload.dump());
                return *this;
            }

            Driver &deleteCookie(const std::string &name) {
                sendRequest(ApiMethod::Delete, sessionURL + "/cookie/" + name);
                return *this;
            }

            Rect getWindowRect() const {
                Json response = sendRequest(ApiMethod::Get, sessionURL + "/window/rect");
                return Rect{response["value"]};
            }

            Driver &setWindowRect(const Rect& rect) {
                sendRequest(ApiMethod::Post, sessionURL + "/window/rect", static_cast<Json>(rect).dump());
                return *this;
            }

            Driver &print(const std::string &ofile, const PageOptions &opts = PageOptions{}) {
                Json optsJson = static_cast<Json>(opts);
                Json response = sendRequest(ApiMethod::Post, sessionURL + "/print", optsJson.empty()? "{}": optsJson.dump());
                std::ofstream pdfFS {ofile, std::ios::binary};
                if (!pdfFS) throw std::runtime_error("Failed to open file for writing: " + ofile);
                std::string decoded {Base64::base64Decode(response["value"])};
                pdfFS.write(decoded.data(), static_cast<long>(decoded.size()));
                return *this;
            }
    };
}
