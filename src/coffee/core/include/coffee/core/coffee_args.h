#pragma once

#include <platforms/argument_parse.h>

namespace Coffee {
namespace BaseArgParser {

using namespace ::platform::args;

extern ArgumentParser& GetBase();

COFFEE_APP_FUNC int PerformDefaults(
    ArgumentParser& parser, ArgumentResult& args);

} // namespace BaseArgParser
} // namespace Coffee
