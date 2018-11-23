#pragma once

#include <platforms/argument_parse.h>

namespace Coffee {
namespace BaseArgParser {

using namespace ::platform::args;

extern ArgumentParser& GetBase();

extern int PerformDefaults(ArgumentParser& parser, ArgumentResult& args);

} // namespace BaseArgParser
} // namespace Coffee
