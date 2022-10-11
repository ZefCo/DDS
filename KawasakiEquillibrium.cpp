# include <iostream>
# include <vector>
# include <random>
# include <math.h>
# include <map>
# include <fstream>
# include <numeric>
# include <string>
# include "ran2.h"

// Declaring seed to be a global variable. Trying to avoid declaring anything else as global
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
    spin = random_int();
    if (spin == 0) { spin = -1; }
    // std::cout << "Spin: " << spin << std::endl;

    return spin;

}


struct POnes
{

};


struct PZeros 
{

};


// Holds the position of particle
struct XY
{
    int x = random_int(0, 2);
    int y = random_int();

    operator int();
};
// XY::operator int()
// {
//     return spin;
// }


// Class for storing the Lattice
class KawasakiLattice
{
// Private section, for values that shouldn't be changed (easily)
private:

    // Rows
    int R;
    // Columns
    int C;
    // Total Sites
    int N = R * C;

    // Coupeling Constant
    double J;
    // External Mag Field
    double H;
    // Tempurature
    double T;

    // % of +
    float p;
    // # of +
    int P;

    // The lattice
    std::vector<std::vector<int>> lattice;

    // Energy
    int E;



    // Place spins on the lattice
    void init_lattice() {

        // Generate lattice vector
        lattice.resize(R);
        for (int i = 0; i < R; i++) {
            lattice[i].resize(C);
        }

        // Puts holes at lattice sites
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                // int v = random_spin();
                lattice[i][j] = 0;

            }
        }

        // Place gas on lattice
        for (int k = 0; k < P; k++) {
            // for (int i = 0; i < R)
            int i = random_int(R);
            int j = random_int(C);

            lattice[i][j] = 1;
        }


        // Locate holes on gas

    }


    // Density of +
    void Pplus() {
        int sites = R * C;

        float localP = p * (float)sites;
        P = (int)localP;

        // return psites;
    }



public:

    // In case I need the total spins
    float get_total_sites() {
        return (N);
    }


    // In case I need the total spins
    void get_density() {
        std::cout << "Density is:\tp = " << p << "\tP = " << P << std::endl;
    }


    // Prints the Lattice
    void print_lattice() {
        for (int i = 0; i < R; i++){
            for (int j = 0; j < C; j++){
                std::cout << lattice[i][j] << "\t";
                // if (lattice[i][j] == 1) { std::cout << "+" << "\t"; }
                // else if (lattice[i][j] == -1) { std::cout << "-" << "\t"; } 
            }
            std::cout << "\n";
        }

        std::cout << "\n";
    }


    // Constructor
    KawasakiLattice(int rows, int columns, double temp, double coupeling, double magfield, float density)
    {
        R = rows;
        C = columns;
        // N = get_total_sites();
        T = temp;
        J = coupeling;
        H = magfield;
        p = density;
        Pplus();

        init_lattice();


        // lattice = init_lattice(); // places things on lattice
        // init_joltzman();
        // init_holtzman();

        // Mag = init_magnitization();
        // mag = Mag / (get_total_spins());
        // Energy = init_energy();
        // energy = Energy / (get_total_spins());

    };

};



int main() {
    // long seed;
    int Rows, Columns;
    double Temp, Coupeling, Magfield;
    float Density;

    std::cout << "Input Seed\nSeed must not be 0: ";
    std::cin >> seed;
    fflush(stdin);

    if (seed > 0) {seed = -1 * seed;}

    std::cout << "Input lattice rows: ";
    std::cin >> Rows;
    fflush(stdin);

    std::cout << "Input lattice Columns: ";
    std::cin >> Columns;
    fflush(stdin);

    std::cout << "Input tempurature: ";
    std::cin >> Temp;
    fflush(stdin);

    std::cout << "Input External Mag Field (recomended to be 0): ";
    std::cin >> Magfield;
    fflush(stdin);
    

    std::cout << "Coupeling Constant set to 1" << std::endl;
    fflush(stdin);

    std::cout << "Choose a density for p+\nNumber must be between 0 and 1 (will round down): ";
    std::cin >> Density;
    fflush(stdin);

    KawasakiLattice motorcycle(Rows, Columns, Temp, 1.0, Magfield, Density);

    motorcycle.print_lattice();
    motorcycle.get_density();



}