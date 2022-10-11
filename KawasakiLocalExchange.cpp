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


// Just pauses the script and waits for the user to press enter
void pause_for_input() {
    std::cout << "\nPress Enter to continue\n";
    // Clears the buffer of any extra bits
    fflush(stdin);
    getchar();

}


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
    // External E Field
    double E;
    // Tempurature
    double T;

    // % of +
    float p;
    // # of +
    int P;

    // The lattice
    std::vector<std::vector<int>> lattice;

    // Energy
    int energy;


    // Boltzman Energy. jolt ties to J, holt ties to h
    // Saving these as Maps, with their key as the DE and values as the actual exp
    std::map<int, double> joltzman;
    std::map<int, double> eoltzman;

    // Initialize the arrays that hold the Boltzman values
    // joltzman for J Boltzman
    void init_joltzman() {
        for (int i = 0; i <= 8; i++) {
            joltzman[i] = exp((-2.0 * i) / T);  // for loop goes from 0 to 8
        }

    }

    // Initialize the arrays that hold the Boltzman values
    // sadly, this is not a dune reference
    void init_eoltzman() {
        for (int i = 0; i <=1; i++ ) {
            eoltzman[i] = exp((-E * i) / T);
        }

    }


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


    void nearest_neighbors(int dir, int r, int c) {
        int rp, cp;

        if (dir == 1) {(rp = r - 1 + R) % R; cp = c;}
        else if (dir == 2) {rp = r; (cp = c + 1) % C;}
        else if (dir == 3) {(rp = r + 1) % R; cp = c;}
        else if (dir == 4) {rp = r; (cp = c -1 + C) % C;} // But what to do if it's out of bounds?

    }


    // Prints the Lattice
    void print_lattice() {
        for (int i = 0; i < R; i++){
            for (int j = 0; j < C; j++){
                // std::cout << lattice[i][j] << "\t";
                if (lattice[i][j] == 1) { std::cout << "1" << "\t"; }
                else if (lattice[i][j] == 0) { std::cout << " " << "\t"; } 
            }
            std::cout << "\n";
        }

        std::cout << "\n";
    }

    int get_R() {
        return(R);
    }

    int get_C() {
        return(C);
    }


    // Constructor
    KawasakiLattice(int rows, int columns, double temp, double coupeling, double efield, float density)
    {
        R = rows;
        C = columns;
        // N = get_total_sites();
        T = temp;
        J = coupeling;
        E = efield;
        p = density;
        Pplus();

        init_lattice();
        init_joltzman();
        init_eoltzman();

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
    int Rows, Columns, max_sweeps, print_first_lattice, print_final_lattice;
    double Temp, Coupeling, Efield;
    float Density;

    std::cout << "Input Seed\nSeed must not be 0\nInput Seed: ";
    std::cin >> seed;
    fflush(stdin);

    if (seed > 0) {seed = -1 * seed;}

    std::cout << "Input lattice rows: ";
    std::cin >> Rows;
    fflush(stdin);

    std::cout << "Input lattice Columns: ";
    std::cin >> Columns;
    fflush(stdin);

    int sweep_steps = Rows*Columns;


    std::cout << "Input tempurature: ";
    std::cin >> Temp;
    fflush(stdin);

    std::cout << "Input External E Field: ";
    std::cin >> Efield;
    fflush(stdin);
    

    std::cout << "Coupeling Constant set to 1, Kb" << std::endl;
    fflush(stdin);

    std::cout << "Choose a density for p+\nNumber must be between 0 and 1 (will round down, recomend 0.5): ";
    std::cin >> Density;
    fflush(stdin);

    KawasakiLattice motorcycle(Rows, Columns, Temp, 1.0, Efield, Density);
    motorcycle.get_density();

    std::cout << "Input number of sweeps to preform: ";
    std::cin >> max_sweeps;
    fflush(stdin);

    std::cout << "\nDo you wish to see the inital lattice? 1 for yes 0 for no: ";
    std::cin >> print_first_lattice;
    fflush(stdin);

    if (print_first_lattice == 1) {
            std::cout << "Initial Lattice" << std::endl;
            motorcycle.print_lattice();
    }

    std::cout << "\nDo you wish to see the final Lattice at the end?\n1 for yes, 0 for no: ";
    std::cin >> print_final_lattice;
    fflush(stdin);

    std::cout << "Ready to begin simulation\nNote: this does NOT include a warm up phase" << std::endl;
    pause_for_input();

    for (int s = 1; s < max_sweeps + 1; s++) {
        for (int t = 1; t < sweep_steps + 1; t++) {
            int rran, cran, rpan, cpan, dir;

            rran = random_int(0, motorcycle.get_R() + 1); 
            cran = random_int(0, motorcycle.get_C() + 1); // gets a random position

            dir = random_int(1, 5); // returns a number between 1 and 4

            

        }
    }


    if (print_final_lattice == 1) {
        std::cout << "Final Lattice:" << std::endl;
        motorcycle.print_lattice();
    }


}