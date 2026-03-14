cmake_minimum_required(VERSION 3.30)
project(webdriverxx VERSION 0.0.1 DESCRIPTION "Selenium driver bindings for C++" 
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options for consumers
option(WEBDRIVERXX_BUILD_TESTS "Build webdriverxx tests" OFF)
option(WEBDRIVERXX_BUILD_EXAMPLES "Build webdriverxx examples" OFF)

# Fetch external dependencies
include(FetchContent)
FetchContent_Declare(json 
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG f534f4f75e12893716ea688679aeb768bff426c4)
set(JSON_Install ON CACHE BOOL "Install JSON" FORCE)
FetchContent_MakeAvailable(json)
FetchContent_Declare(httplib 
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG 68fa9bce0f1abb27fc7507c372c6ac0b75f8a878)
FetchContent_MakeAvailable(httplib)

# Define `webdriverxx` as an INTERFACE library
add_library(webdriverxx INTERFACE)
add_library(webdriverxx::webdriverxx ALIAS webdriverxx)
target_sources(webdriverxx INTERFACE 
    FILE_SET HEADERS BASE_DIRS include FILES
    include/webdriverxx/apierror.hpp
    include/webdriverxx/base64.hpp
    include/webdriverxx/capabilities.hpp
    include/webdriverxx/cookie.hpp
    include/webdriverxx/element.hpp
    include/webdriverxx/pageoptions.hpp
    include/webdriverxx/rect.hpp
    include/webdriverxx/timeout.hpp
    include/webdriverxx/utils.hpp
    include/webdriverxx/webdriver.hpp
)
target_link_libraries(webdriverxx INTERFACE 
    httplib::httplib nlohmann_json::nlohmann_json)

# Include tests
if (WEBDRIVERXX_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

# Include examples
if (WEBDRIVERXX_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Create install targets
include(GNUInstallDirs)
install(TARGETS webdriverxx EXPORT Webdriverxx-Targets FILE_SET HEADERS)
install(EXPORT Webdriverxx-Targets NAMESPACE webdriverxx::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/webdriverxx)

# Create Webdriverxx-Config.cmake file
include(CMakePackageConfigHelpers)
set(WEBDRIVERXX_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/webdriverxx)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Webdriverxx-Config.cmake.in"
    Webdriverxx-Config.cmake
    INSTALL_DESTINATION ${WEBDRIVERXX_CMAKE_DIR}
    PATH_VARS WEBDRIVERXX_CMAKE_DIR)

# Create a CalcConfigVersion.cmake file
write_basic_package_version_file(
    WebdriverxxConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

# Install the config file to required path
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Webdriverxx-Config.cmake" 
    "${CMAKE_CURRENT_BINARY_DIR}/WebdriverxxConfigVersion.cmake"
    DESTINATION ${WEBDRIVERXX_CMAKE_DIR})

# CPack config
set(CPACK_PACKAGE_NAME "Webdriverxx")
set(CPACK_PACKAGE_VENDOR "Naresh Jagadeesan")
set(CPACK_PACKAGE_DESCRIPTION "Selenium bindings for C++")
set(CPACK_PACKAGE_CONTACT "37257700+Infinage@users.noreply.github.com")
set(CPACK_GENERATOR TGZ)
include(CPack)
