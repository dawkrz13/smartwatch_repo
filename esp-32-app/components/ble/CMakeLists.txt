set(COMPONENT_SRCDIRS "src")
set(COMPONENT_ADD_INCLUDEDIRS "src" 
        "$ENV{IDF_TOOLS_PATH}/components/bt/host/bluedroid/api/include/api"
        "$ENV{IDF_TOOLS_PATH}/arduino-esp32/cores/esp32"
        "$ENV{IDF_TOOLS_PATH}/components/bt/include/esp32/include"
)
list(APPEND COMPONENT_REQUIRES "nvs_flash")
register_component()



