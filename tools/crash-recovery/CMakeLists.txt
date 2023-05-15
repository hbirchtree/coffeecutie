coffee_application(
  TARGET CrashRecovery USE_CMD
  TITLE "Crash Recovery"
  SOURCES crash_recovery.cpp
  LIBRARIES Core ASIO
)
