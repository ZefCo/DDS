#include <iostream>
#include "ran3c.h"
#include <stdio.h>
#include <typeinfo>

long seed;


int main() {

    std::cout << "This tests the ran2 provided in NR4C\nIt will generate 1,000,000,000,000 random values with the expectation that they are [0, 1) and will print any value of 1, the current seed, and which step it occured on." << std::endl;

    std::cout << "Input Seed (long), must not be 0: ";
    std::cin >> seed;

    // if (seed > 0) {seed = -1 * seed;}

    std::cout << "Initial Seed = " << seed << std::endl;

    for (int i = 0; i < 1000000000000; i++) {
        float rnt = ran3(&seed);
        if (rnt >= 1) {std::cout << "R Value = " << rnt << " Seed = " << seed << " Step = " << i << " Size = " << sizeof(rnt) << " Type = " << typeid(rnt).name() << std::endl;}
    }

}