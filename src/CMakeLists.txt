cmake_policy(SET CMP0020 NEW)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

find_package(Cryptopp REQUIRED)
find_package(Opus REQUIRED)
find_package(Jrtplib REQUIRED)

#Set variables with all the sources to compile
set(SVOIP_HEADERS
    contactlistwindow.h
    editcontactwindow.h
    messengerwindow.h
    settingswindow.h
    contact.h
    message.h
    voip.h
    qopusdevice.h
    networkmanager.h
    handshake.h
    qjrtp.h
)
set(SVOIP_SOURCES
    main.cpp
    contactlistwindow.cpp
    editcontactwindow.cpp
    messengerwindow.cpp
    settingswindow.cpp
    contact.cpp
    message.cpp
    voip.cpp
    qopusdevice.cpp
    networkmanager.cpp
    handshake.cpp
    qjrtp.cpp

)
set(SVOIP_FORMS
    contactlistwindow.ui
    editcontactwindow.ui
    settingswindow.ui
    messengerwindow.ui
)
set(SVOIP_RES
    res/svoip.rc
)

#process the ui files.
qt5_wrap_ui(SVOIP_UI_HEADERS ${SVOIP_FORMS})

add_executable(${PROJECT} ${SVOIP_SOURCES} ${SVOIP_HEADERS} ${SVOIP_UI_HEADERS} ${SVOIP_RES})

target_link_libraries(${PROJECT}
    ${CRYPTOPP_LIBRARIES}
    ${OPUS_LIBRARIES}
    ${JRTPLIB_LIBRARIES}
)

qt5_use_modules(${PROJECT} Widgets Multimedia)
