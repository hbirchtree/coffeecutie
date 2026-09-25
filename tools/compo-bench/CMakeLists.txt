find_package(nanobench CONFIG REQUIRED)

coffee_application(
  TARGET CompoBench USE_CMD
  TITLE "Component Benchmarks"
  SOURCES compo_bench.cpp
  LIBRARIES Core EntComp
)
target_link_libraries(CompoBench PRIVATE nanobench::nanobench)
