coffee_application(
  TARGET EnvTest
  TITLE "Environment Data"
  SOURCES main.cpp
  LIBRARIES Core
)

if(TARGET Net)
  target_link_libraries(EnvTest PUBLIC Net)
endif()
