find_package(Bullet)

include_directories ( SYSTEM
    ${BULLET_INCLUDE_DIRS}
    )

coffee_add_library ( CoffeeBullet "bullet.cpp" "${BULLET_LIBRARIES}" )
