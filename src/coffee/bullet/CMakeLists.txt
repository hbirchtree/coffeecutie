find_package(Bullet)

include_directories (
    ${BULLET_INCLUDE_DIRS}
    )

coffee_add_library ( CoffeeBullet "bullet.cpp" "${BULLET_LIBRARIES}" )
