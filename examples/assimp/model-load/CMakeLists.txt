find_package(assimp REQUIRED)

include_directories(${ASSIMP_INCLUDE_DIR})

coffee_add_application_longer (
    AssimpModelLoad "Assimp Model Load" "Coffee Guy" "1" main.cpp
    "CoffeeCore;CoffeeAssimp" "${ASSIMP_LIBRARIES}" ""
    )
