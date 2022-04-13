cmake_minimum_required(VERSION 3.10)

project(monocular)

set(CMAKE_BUILD_TYPE Release)

############################################################
# Create a library
############################################################

#Generate the static library from the library sources
add_library(monocular SHARED 
    cmd_parse/cmd_parse.c
    cssc132/cssc132.c
    led/led.c
    monocular/monocular.c
    mpu9250/mpu9250.c
    net/net.c
    serial/serial.c
    sync/sync.c
    sync_module/sync_module.c
    ub482/ub482.c
    ui3240/ui3240.c
)

target_link_libraries(monocular 
    m
    jpeg
    pthread
    ueye_api
)

# target_include_directories(monocular
#     PUBLIC 
#         ${PROJECT_SOURCE_DIR}/serial
#         ${PROJECT_SOURCE_DIR}/net
#         ${PROJECT_SOURCE_DIR}/ub482
#         ${PROJECT_SOURCE_DIR}/monocular
#         ${PROJECT_SOURCE_DIR}/cmd_parse
#         ${PROJECT_SOURCE_DIR}/mpu9250
#         ${PROJECT_SOURCE_DIR}/ui3240
#         ${PROJECT_SOURCE_DIR}/cssc132
#         ${PROJECT_SOURCE_DIR}/sync_module
#         ${PROJECT_SOURCE_DIR}/led
#         ${PROJECT_SOURCE_DIR}/sync
#         ${PROJECT_SOURCE_DIR}/interface
# )

include_directories(
    ${UEYE_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/serial
    ${PROJECT_SOURCE_DIR}/net
    ${PROJECT_SOURCE_DIR}/ub482
    ${PROJECT_SOURCE_DIR}/monocular


    ${PROJECT_SOURCE_DIR}/cmd_parse
    ${PROJECT_SOURCE_DIR}/cssc132

    ${PROJECT_SOURCE_DIR}/mpu9250
    ${PROJECT_SOURCE_DIR}/ui3240
    
    ${PROJECT_SOURCE_DIR}/sync_module
    ${PROJECT_SOURCE_DIR}/led
    ${PROJECT_SOURCE_DIR}/sync
)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(sdk
    main.c
)

# link the new hello_library target with the monocular target
target_link_libraries(sdk
    PRIVATE 
        monocular
)
