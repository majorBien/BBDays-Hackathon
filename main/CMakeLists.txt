set(component_srcs "main.c")

idf_component_register(SRCS "${component_srcs}" "lora.c" "wifi.c"
                       INCLUDE_DIRS ".")
