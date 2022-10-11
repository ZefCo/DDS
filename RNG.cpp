#include "ran2.h"
#include <stdio.h>

int seed;
// Generate random int: by default this is from [0, 1]
// Note it actually generates a number between [0, 2) so when being used it should thought of as [min, max + 1)
int random_int(int min = 0, int max = 2)
{

    float rval = NR::ran2(seed);
    int i = min + ((max - min) * rval);
    // std::cout << "Rval: " << rval << " Rnt: " << i << std::endl;

    return i;
}



// Gets a random int between 0 and 2 (remember it's [0, 1) ) and then turns the 0 into -1
int random_spin() 
{
    int spin;
    spin = random_int(seed);
    if (spin == 0) { spin = -1; }
    // std::cout << "Spin: " << spin << std::endl;

    return spin;

}



int main() {

    // int init_seed = 1111;
    seed = -111111111;

    if (seed > 0) {seed = -1 * seed;}
    // seed = 50027102;
    // for (int i = 0; i < 10; i++) {
    //     float random = NR::ran2(seed);
    //     std::cout << "Random value = " << random << " Seed Value = " << seed << std::endl;
    // }

    // std::cout << std::endl;


    // seed = 50027102;
    // for (int i = 0; i < 10; i++) {
    //     float random = NR::ran2(seed);
    //     std::cout << "Random value = " << random << " Seed Value = " << seed << std::endl;
    // }

    std::cout << "Initial Seed = " << seed << " Size = " << sizeof(seed) << std::endl;

    for (int i = 0; i < 10; i++) {
        int rnt = random_int();

        std::cout << "Random Int = " << rnt << " Seed = " << seed << std::endl;
        // std::cout << "Rvalue = " << NR::ran2(seed) << " Seed = " << seed << std::endl;
    }

}