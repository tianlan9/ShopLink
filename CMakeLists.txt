cmake_minimum_required(VERSION 3.21)

project(ShopLink LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

qt_standard_project_setup()

# =============================================================
# 1. 定义核心业务库 (ShopCore)
# =============================================================
add_library(ShopCore STATIC
    core/user.cpp core/user.h
    core/customer.cpp core/customer.h
    core/merchant.cpp core/merchant.h
    core/product.cpp core/product.h
)

target_link_libraries(ShopCore PRIVATE Qt6::Core Qt6::Sql)
target_include_directories(ShopCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 如果是 GCC/MinGW，开启覆盖率编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ShopCore PRIVATE --coverage)
endif()

# =============================================================
# 2. 定义主程序 GUI (ShopLink)
# =============================================================
qt_add_executable(ShopLink
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_link_libraries(ShopLink PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql
    ShopCore
)

# [关键修复]：主程序也必须链接 gcov 库
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(ShopLink PRIVATE --coverage)
endif()

# =============================================================
# 3. 集成 Google Test (单元测试)
# =============================================================
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # 你的测试文件路径
    add_executable(UnitTests tests/tst_shoplink.cpp)

    target_link_libraries(UnitTests PRIVATE
        GTest::gtest_main
        ShopCore
        Qt6::Sql
    )

    # 测试程序也必须开启覆盖率链接选项
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(UnitTests PRIVATE --coverage)
    endif()

    include(GoogleTest)
    gtest_discover_tests(UnitTests DISCOVERY_MODE PRE_TEST)
endif()
