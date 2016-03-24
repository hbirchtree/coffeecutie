#ifndef CAPPLICATION_START
#define CAPPLICATION_START


typedef int(*CoffeeMainWithArgs)(int,char**);

extern "C" int deref_main(CoffeeMainWithArgs mainfun, int argc, char** argv);

//This is a cheeky little macro that allows us to wrap the main function.
#define COFFEE_APPLICATION_MAIN(mainfun) \
    extern "C" int main(int argv, char** argc){ return deref_main(mainfun,argv,argc); }

#endif
