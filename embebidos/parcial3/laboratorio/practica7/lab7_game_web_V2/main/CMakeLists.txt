idf_component_register(SRCS "main.c" "modulos/WIFI/wifi_lib.c" "modulos/GPIO/gpio_lib.c"
                    INCLUDE_DIRS "." "modulos/WIFI" "modulos/GPIO")
spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)