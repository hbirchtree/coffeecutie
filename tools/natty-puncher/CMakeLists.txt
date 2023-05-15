coffee_application(
  TARGET NattyPuncher USE_CMD
  TITLE "Natty Puncher"
  SOURCES natty_puncher.cpp
  LIBRARIES Core ASIO
)

coffee_application(
  TARGET NattyTester USE_CMD
  TITLE "Natty Tester"
  SOURCES natty_tester.cpp
  LIBRARIES Core ASIO
)
