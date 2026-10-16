coffee_application(
  TARGET ElegyInfo
  TITLE "Halo 3 Map Info"
  SOURCES main.cpp
  LIBRARIES Core Blam
)

# WAV output next to extracted XMA2 when the decoder is built
if(TARGET OAFXMA2Decoder)
  target_link_libraries(ElegyInfo PRIVATE OAFXMA2Decoder)
endif()
