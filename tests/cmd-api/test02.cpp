#include <coffee/core/CUnitTesting>
#include <coffee/core/CMD>

using namespace Coffee;

const constexpr cstring test1_names[20] = {
    "Hello",
    "HAAAAAAAAAAAAAAAA",
    "Laaaaaaaaaaaaaaaaaaaaaaaaaa"
};

const constexpr bigscalar test1_values[20] = {
    20.020432,
    21.084,
    130492.0405
};

bool table_test()
{
    Table::Header h = {"Names", "Values"};
    Table::Table table(h);
    table.push_back(Table::GenColumn(test1_names,20));
    table.push_back(Table::GenColumn(test1_values,20));


    cBasicPrint("{0}",Table::GenTable(table,h));

    return true;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {table_test,"Print a table", nullptr, false, false}
};

COFFEE_RUN_TESTS(_tests);
