cmake_policy(SET CMP0020 NEW)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Sql REQUIRED)


find_package(Cryptlib REQUIRED)
find_package(Opus REQUIRED)
find_package(Jrtplib REQUIRED)
find_package(JThread REQUIRED)

#Set variables with all the sources to compile
set(SVOIP_HEADERS
    contactlistwindow.h
    contactitemwidget.h
    editcontactwindow.h
    settingswindow.h
    passwordwindow.h
    contactdb.h
    contact.h
    svoip.h
    voip.h
    qpcmbuffer.h
    networkmanager.h
    rsakeyring.h
    packetagent.h
    rootlink.h
    abstractapp.h
    messengerapp.h
    abstractlink.h

)

set(SVOIP_SOURCES
    main.cpp
    contactlistwindow.cpp
    contactitemwidget.cpp
    editcontactwindow.cpp
    settingswindow.cpp
    passwordwindow.cpp
    contactdb.cpp
    contact.cpp
    svoip.cpp
    voip.cpp
    qpcmbuffer.cpp
    networkmanager.cpp
    rsakeyring.cpp
    packetagent.cpp
    rootlink.cpp
    abstractapp.cpp
    messengerapp.cpp
    abstractlink.cpp

)

set(SVOIP_FORMS
    contactlistwindow.ui
    editcontactwindow.ui
    settingswindow.ui
    passwordwindow.ui
    contactitemwidget.ui
    messengerapp.ui
)

set(SVOIP_RES
   ../res/res.qrc
)

#process the ui files.
qt5_wrap_ui(SVOIP_UI_MOC ${SVOIP_FORMS})

#process the ressource files.
qt5_add_resources(SVOIP_RES_MOC ${SVOIP_RES})

add_executable(${PROJECT}
    ${SVOIP_SOURCES}
    ${SVOIP_HEADERS}
    ${SVOIP_UI_MOC}
    ${SVOIP_RES_MOC}
)

target_link_libraries(${PROJECT}
    ${CRYPTLIB_LIBRARIES}
    ${OPUS_LIBRARIES}
    ${JRTPLIB_LIBRARIES}
    ${JTHREAD_LIBRARIES}
)

qt5_use_modules(${PROJECT} Widgets Multimedia Sql)
