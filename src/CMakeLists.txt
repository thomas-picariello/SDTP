cmake_policy(SET CMP0020 NEW)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

#Set variables with all the sources to compile
set(SVOIP_HEADERS
    svoip.h
    contactlistwindow.h
    editcontactwindow.h
    settingswindow.h
    responder.h
    contact.h
    starter.h
    messenger.h
)
set(SVOIP_SOURCES
    main.cpp
    svoip.cpp
    contactlistwindow.cpp
    editcontactwindow.cpp
    settingswindow.cpp
    responder.cpp
    contact.cpp
    starter.cpp
    messenger.cpp
)
set(SVOIP_FORMS
    contactlistwindow.ui
    editcontactwindow.ui
    settingswindow.ui
)

#add libs as include dirs
include_directories(${LIBS_SOURCE_DIR})

#process the ui files.
qt5_wrap_ui(SVOIP_UI_HEADERS ${SVOIP_FORMS})

add_executable(${PROJECT} ${SVOIP_SOURCES} ${SVOIP_HEADERS} ${SVOIP_UI_HEADERS})

target_link_libraries(${PROJECT} cryptopp)

qt5_use_modules(${PROJECT} Widgets Multimedia)


