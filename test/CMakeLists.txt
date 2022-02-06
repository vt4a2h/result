find_package(Catch2 CONFIG REQUIRED)

set(SOURCE_LIST CreateResultTest.cpp CheckStateTest.cpp GetDataTest.cpp)

add_executable(tests
    main.cpp
    ${SOURCE_LIST}
)

target_compile_features(tests PRIVATE cxx_std_20)

target_link_libraries(tests PRIVATE Catch2::Catch2)

include(Catch)
catch_discover_tests(tests)
