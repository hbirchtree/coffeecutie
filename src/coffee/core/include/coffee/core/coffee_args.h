#pragma once

#include "plat/environment/argument_parse.h"

namespace Coffee {
namespace BaseArgParser {

extern ArgumentParser& GetBase();

extern int PerformDefaults(ArgumentParser& parser, ArgumentResult& args);

} // namespace BaseArgParser
} // namespace Coffee
