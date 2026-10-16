coffee_application(
  TARGET ElegyInfo
  TITLE "Halo 3 Map Info"
  SOURCES main.cpp
  LIBRARIES Core Blam
)

# WAV output next to extracted XMA2 when OAF carries the decoder
if(TARGET OAF)
  target_link_libraries(ElegyInfo PRIVATE OAF)
endif()
