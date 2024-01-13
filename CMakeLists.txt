cmake_minimum_required(VERSION 3.15)

project(CPPGameLauncher)

set(CMAKE_CXX_STANDARD 20)

# Kaynak dosyalarını belirt
set(SOURCE_FILES
    button.cpp
    compression.cpp
    downloader.cpp
    fontloader.cpp
    graphicscache.cpp
    helper.cpp
    main.cpp
    newslabel.cpp
    patcher.cpp
    pngloader.cpp
    progressbar.cpp
    rsaverifier.cpp
    sha256.cpp
    stdafx.cpp
    init.cpp
)

# Header dosyalarını belirt
set(HEADER_FILES
    button.h
    compression.h
    constants.h
    crc.h
    downloader.h
    fontloader.h
    graphicscache.h
    helper.h
    iniparser.h
    newslabel.h
    patcher.h
    pngloader.h
    progressbar.h
    main.h
    Resource.h
    rsaverifier.h
    sha256.h
    stdafx.h
    targetver.h
)

# Kaynak dosyalarını ekleyin
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} CPPGameLauncher.rc)

# OpenSSL, zlib ve libcurl kütüphanelerini ekleyin
# Yol ve bağlantı ayarlarını doğru bir şekilde düzenlemelisiniz
target_include_directories(${PROJECT_NAME} PRIVATE
	libs/OpenSSL-Win64/include
    libs/zlib/include
    libs/curl/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    libs/OpenSSL-Win64/lib/VC/x64/MT
    libs/zlib/bin
	libs/zlib/lib
    libs/curl/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    libssl.lib
    libcrypto.lib
	libssl_static.lib
    zlibstatic.lib
)

# Windows kaynak dosyalarını ekleyin
target_sources(${PROJECT_NAME} PRIVATE
    #close_window.bmp
    #G:/Pictures/background.bmp
    #icon1.ico
    #minimize_window.bmp
    #resources/background.bmp
    resources/background.png
    resources/buttons/about.png
    resources/buttons/about_hovered.png
    resources/buttons/about_pressed.png
    #resources/buttons/button.png
    resources/buttons/close_window.png
    resources/buttons/close_window_hovered.png
    resources/buttons/close_window_pressed.png
    resources/buttons/continue.png
    resources/buttons/continue_hovered.png
    resources/buttons/continue_pressed.png
    resources/buttons/minimize_window.png
    resources/buttons/minimize_window_hovered.png
    resources/buttons/minimize_window_pressed.png
    resources/buttons/news_label.png
    resources/buttons/news_label_hovered.png
    resources/buttons/play.png
    resources/buttons/play_disabled.png
    resources/buttons/play_hovered.png
    resources/buttons/play_pressed.png
    resources/buttons/repair.png
    resources/buttons/repair_hovered.png
    resources/buttons/repair_pressed.png
    resources/buttons/settings.png
    resources/buttons/settings_hovered.png
    resources/buttons/settings_pressed.png
    resources/buttons/try_again.png
    resources/buttons/try_again_hovered.png
    resources/buttons/try_again_pressed.png
    #resources/close_window.png
    resources/fade.png
    resources/newslabel.png
    resources/progressindicator.png
    misc/favicon.ico
)

# Font dosyalarını ekleyin
target_sources(${PROJECT_NAME} PRIVATE
    resources/TitilliumWeb-Bold.ttf
    resources/TitilliumWeb-Regular.ttf
)
