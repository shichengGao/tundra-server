
set(http_SRCS
        HttpRequest.h
        HttpResponse.cpp
        HttpContext.cpp
        HttpServer.cpp
        )

add_library(tundra_http ${http_SRCS})
target_link_libraries(tundra_http tundra_net)

