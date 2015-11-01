#ifndef DATA_TYPES
#define DATA_TYPES

/*
 * Most of development has been done purely on 64-bit systems.
 * I have no idea how it will react to 32-bit.
 * As such, I have formulated these tests to check for most of the important assumptions
 *   we are operating under.
 * If something fails here, the rest will surely not work.
 *
 */

namespace CoffeeTests{

extern void vector2_test();

extern void vector3_test();

extern void vector4_test();

extern void quaternion_test();

extern void matrix_test();

extern void vector_tests(bool silent = true);

extern void int_tests();

extern void floating_tests();

extern void run_tests(bool silent = true);


} //CoffeeTests

#endif // DATA_TYPES

