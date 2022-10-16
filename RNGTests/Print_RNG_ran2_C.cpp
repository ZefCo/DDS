#include <iostream>
#include "..\HeaderFiles\ran2c.h"
#include <stdio.h>
#include <typeinfo>

long seed;


// Generate random int: by default this is from [0, 1]
// Note it actually generates a number between [0, 2) so when being used it should thought of as [min, max + 1)
int random_int(float rvalue, int min = 0, int max = 2)
{

    // float rval = ran2(&seed);
    // if (print_rval){std::cout << "Rval: " << rval;}
    int i = min + ((max - min) * rvalue);
    // if (print_rval){std::cout << " Rnt: " << i << " Seed : " << seed << std::endl;}
    return i;
}


int main() {

    int first_print, last_print;

    std::cout << "This outputs PRNG generated from NRC Ran2 between a certain range.\nIt asks for a seed (long), the first iteration it should print at, and the last.\nIt will generate PRNG from 0 to last_print, but will only show between first_print & last_print" << std::endl;

    std::cout << "Input Seed (long), must not be 0: ";
    std::cin >> seed;
    std::cout << std::endl;
    std::cout << "Input First Print: ";
    std::cin >> first_print;
    std::cout << std::endl;
    std::cout << "Input Last Print: ";
    std::cin >>last_print;
    std::cout << std::endl;

    // if (seed > 0) {seed = -1 * seed;}

    std::cout << "Initial Seed = " << seed << std::endl;

    for (int i = 0; i < last_print + 1; i++) {
        float rnt = ran2(&seed);
        if (i > first_print){
            std::cout << "Iteration = " << i << "\tSeed = " << seed << "\tR Value = " << rnt << "\tR Int" << random_int(rnt) << std::endl;
            std::cout << "\t\t(Int)Rvalue = " << (int)rnt << "\t(double)Rvalue = " << (double)rnt << std::endl;
            }
    }
}