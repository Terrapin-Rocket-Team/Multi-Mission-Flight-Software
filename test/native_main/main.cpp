#ifndef PIO_UNIT_TESTING

#include <iostream>

int main()
{
    std::cout << "Hello World!" << std::endl;
    return 0;
}

// This file exists to allow native to build without being in unit test mode. 
// I wanted a way to click the build button and just know if would work without having to go thru the whole testing thing.
// also clicking the build button saves the files

#endif