foreach(EXAMPLE IN ITEMS github screener)
    add_executable(${EXAMPLE} "${EXAMPLE}.cpp")
    target_link_libraries(${EXAMPLE} PRIVATE webdriverxx::webdriverxx)
endforeach()
