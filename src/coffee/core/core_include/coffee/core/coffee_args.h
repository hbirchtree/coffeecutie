#pragma once

#include <cxxopts.hpp>
#include <peripherals/identify/compiler/function_export.h>

namespace Coffee::BaseArgParser {

extern cxxopts::Options& GetBase(cxxopts::Options& parser);

COFFEE_APP_FUNC int PerformDefaults(
    cxxopts::Options& parser, std::vector<const char*>& args);

} // namespace BaseArgParser
