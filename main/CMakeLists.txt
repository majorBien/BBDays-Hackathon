set(component_srcs "main.c")

idf_component_register(SRCS "${component_srcs}" "lora.c" "wifi.c" "gpio.c" "http_server.c"
                       INCLUDE_DIRS "."
                       EMBED_FILES 
                       "webpage/Index.html" 
                       "webpage/Login.html" 
                       "webpage/Profile.html"  
                       "webpage/Register.html"
                       "webpage/jquery.js")
