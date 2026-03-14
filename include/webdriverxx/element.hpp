#pragma once

#include "utils.hpp"
#include "rect.hpp"
#include "base64.hpp"
#include <fstream>
#include <stdexcept>

namespace webdriverxx {
    using Json = nlohmann::json;

    class Element {
        private:
            const std::string elementRef, elementId, sessionURL, elementURL;
            friend class Driver;

        public:
            explicit operator bool() const { 
                return !elementId.empty(); 
            }

            operator Json() const {
                return Json{{elementRef, elementId}};
            }

            Element(const std::string &elementRef, const std::string &elementId, const std::string &sessionURL): 
                elementRef(elementRef), 
                elementId(elementId), 
                sessionURL(sessionURL),
                elementURL(sessionURL + "/element/" + elementId) 
            { }

            Element &clickJS() {
                Json payload = {
                    { "script", "arguments[0].click();" }, 
                    { "args", Json::array({{{elementRef, elementId}}}) }
                };
                sendRequest(ApiMethod::Post, sessionURL + "/execute/sync", payload.dump());
                return *this;
            }

            Element &click() {
                sendRequest(ApiMethod::Post, elementURL + "/click");
                return *this;
            }

            Element &sendKeys(const std::string &text) {
                Json payload {{ "text", text }};
                sendRequest(ApiMethod::Post, elementURL + "/value", payload.dump());
                return *this;
            }

            Element &submit() { 
                sendKeys("" + Keys::Enter); 
                return *this;
            }

            Element &clear() {
                sendRequest(ApiMethod::Post, elementURL + "/clear");
                return *this;
            }

            Element &scrollIntoView(unsigned short pauseMS = 0) {
                Json payload = {
                    { "script", "arguments[0].scrollIntoView({behavior: 'smooth', block: 'center'});" }, 
                    { "args", Json::array({{{elementRef, elementId}}}) }
                };
                sendRequest(ApiMethod::Post, sessionURL + "/execute/sync", payload.dump());
                std::this_thread::sleep_for(std::chrono::milliseconds(pauseMS));
                return *this;
            }

            std::string getElementAttribute(const std::string &name) const {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/attribute/" + name);
                return response["value"];
            }

            std::string getElementProperty(const std::string &name) const {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/property/" + name);
                return response["value"];
            }

            std::string getElementCSSValue(const std::string &name) const {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/css/" + name);
                return response["value"];
            }

            std::string getElementText() const {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/text");
                return response["value"];
            }

            std::string getElementTagName() const {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/name");
                return response["value"];
            }

            bool isEnabled() const {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/enabled");
                return response["value"];
            }

            bool isSelected() const {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/selected");
                return response["value"];
            }

            Element &save_screenshot(const std::string &ofile) {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/screenshot");
                std::ofstream imageFS {ofile, std::ios::binary};
                if (!imageFS) throw std::runtime_error("Failed to open file for writing: " + ofile);
                std::string decoded {Base64::base64Decode(response["value"])};
                imageFS.write(decoded.data(), static_cast<long>(decoded.size()));
                return *this;
            }

            Element getActiveElement() const {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/element/active");
                return Element(
                    response["value"].begin().key(), 
                    response["value"].begin().value(), 
                    sessionURL
                );
            }

            Element findElement(const LocationStrategy &strategy, const std::string &criteria) const {
                std::string strategyKeyword;
                switch (strategy) {
                    case LocationStrategy::CSS: strategyKeyword = "css selector"; break;
                    case LocationStrategy::TagName: strategyKeyword = "tag name"; break;
                    case LocationStrategy::Xpath: strategyKeyword = "xpath"; break;
                }
                Json payload {{"using", strategyKeyword}, {"value", criteria}};
                Json response = sendRequest(ApiMethod::Post, elementURL + "/element", payload.dump());
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
                Json response = sendRequest(ApiMethod::Post, elementURL + "/elements", payload.dump());
                std::vector<Element> elements;
                std::transform(response["value"].begin(), response["value"].end(), std::back_inserter(elements), 
                    [&](const Json::value_type &ele) {
                        return Element{ele.begin().key(), ele.begin().value(), sessionURL};
                    }
                );
                return elements;
            }

            Rect getElementRect() const {
                Json response = sendRequest(ApiMethod::Get, elementURL + "/rect");
                return Rect{response["value"]};
            }
    };
}
