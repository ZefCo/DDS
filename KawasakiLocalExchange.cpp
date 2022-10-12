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
    int N;

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

        N = R * C;

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


    // Finds the nearest neighbor to the randomly selected particle
    // then picks a direction and at random: order is starts at bottom and goes ccw... I think
    std::array<int, 2> nearest_neighbor(int row, int col, int dir) {

        int rowp, colp;
        std::array<int, 2> prime;

        if (dir == 1) {rowp = (row + 1) % R; colp = col;}
        else if (dir == 2) {rowp = row; colp = (col - 1 + C) % C;}
        else if (dir == 3) {rowp = (row - 1 + R) % R; colp = col;}
        else if (dir == 4) {rowp = row; colp = (col + 1) % C;}


        prime = {rowp, colp};

        return prime;

    }

    // Finds the positions of the nearest neighbors
    // Order being returned is Down, Right, Up, Left: starts at bottom goes ccw
    std::array<int, 4> nearest_neighbors(int r, int c) {
        std::array<int, 4> nn;

        int rowu = (r - 1 + R) % R; // up one
        int colr = (c + 1) % C; // right one
        int rowd = (r + 1) % R; // down one
        int coll = (c - 1 + C) % C; // left one

        nn = {rowd, coll, rowu, colr};

        return nn;

    }


public:

    int delta_energy(int row, int col, int dir) {
        int local_energy, prime_energy, return_energy;
        std::array<int, 2> prime_position = nearest_neighbor(row, col, dir);

        // std::cout << lattice[row][col] << std::endl;//"\t" << lattice[prime_position[0]][prime_position[1]] << std::endl;

        if (lattice[row][col] != lattice[prime_position[0]][prime_position[1]]) {
            std::array<int, 4> local_neighbors = nearest_neighbors(row, col);
            std::array<int, 4> prime_neighbors = nearest_neighbors(prime_position[0], prime_position[1]);

            local_energy = (lattice[local_neighbors[0]][col] + lattice[row][local_neighbors[1]] + lattice[local_neighbors[2]][col] + lattice[row][local_neighbors[3]]) * lattice[row][col]; 
            prime_energy = (lattice[prime_neighbors[0]][prime_position[1]] + lattice[prime_position[0]][prime_neighbors[1]] + lattice[prime_neighbors[2]][prime_position[1]] + lattice[prime_position[0]][prime_neighbors[3]]) * lattice[prime_position[0]][prime_position[1]]; 

            return_energy = local_energy + prime_energy;
        }
        else {return_energy = -1;}

        return return_energy;

    }


    // just outputs the current parameters
    void get_inputs() {

        std::cout << "\nInputs:" << std::endl;
        std::cout << "Current Seed: " << seed << std::endl;
        std::cout << "Dimensions of Lattice: " << R << " x " << C << std::endl;
        std::cout << "Density (% & #): " << p << " " << P << "\tTempurature: " << T << "\tE Field: " << E << "\tJ = k (1, dimensionless)" << std::endl;
        print_boltzman();
        // std::cout << "Sweeps: " << max_sweeps << std::endl;
        // std::cout << "Measurement will be taken every 1/" << partial << "sweep" << std::endl;


    }

    // In case I need the total spins
    float get_total_sites() {
        return (N);
    }


    // In case I need the total spins
    void get_density() {
        std::cout << "Density is:\tp = " << p << "\tP = " << P << std::endl;
    }

    int get_R() {
        return(R);
    }

    int get_C() {
        return(C);
    }


    // Just prints the Boltzman J and H Maps
    void print_boltzman() {
        
        std::cout << "\nJ Botlzman Energy:" << std::endl;
        for( std::map<int, double>::iterator i = joltzman.begin(); i != joltzman.end(); i++) {
            std::cout << "\tKey: " << (*i).first << "\tValue: " << (*i).second << std::endl;
        }

        std::cout << "E Boltzman Energy" << std::endl;
        for( std::map<int, double>::iterator i = eoltzman.begin(); i != eoltzman.end(); i++) {
            std::cout << "\tKey: " << (*i).first << "\tValue: " << (*i).second << std::endl;
        }
        std::cout << std::endl;

    }



    // Prints the Lattice
    void print_lattice() {
        for (int i = 0; i < R; i++){
            for (int j = 0; j < C; j++){
                // std::cout << lattice[i][j] << "\t";
                if (lattice[i][j] == 1) { std::cout << "1" << " "; }
                else if (lattice[i][j] == 0) { std::cout << " " << " "; } 
            }
            std::cout << "\n";
        }

        std::cout << "\n";
    }



    void sweep() {
        std::cout << "\nline 309";
        int moves = 0;
        for (int n = 1; n < N + 1; n++) {
            std::cout << "\tline 312";
            int rran, cran, rpan, cpan, dir;
            int energy;
            int exchange_one, exchange_two;

            std::cout << "\tline 317";
            rran = random_int(0, R); 
            cran = random_int(0, C); // gets a random position

            std::cout << "\tline 321";
            dir = random_int(1, 5); // returns a number between 1 and 4
            energy = delta_energy(rran, cran, dir);

            if (energy > -1) {
                std::cout << "\tline 326";
                float probability = joltzman[energy];
                std::cout << "\tline 328" << std::endl;
                float chance = NR::ran2(seed);
                std::cout << "chance = " << chance << std::endl;
                std::cout << "probability = " << probability << std::endl;

                if (probability >= chance) {
                    std::cout << "Exchanging positions" << std::endl;
                    std::cout << "Rran = " << rran << " Cran = " << cran << " Rran' = " << rpan << " Cran' = " << cpan << std::endl;
                    exchange_one = lattice[rran][cran];
                    exchange_two = lattice[rpan][cpan];

                    std::cout << exchange_one << "\t" << exchange_two << std::endl;
                    
                    std::cout << "The actual exchange" << std::endl;
                    lattice[rran][cran] = exchange_two;
                    lattice[rpan][cpan] = exchange_one;

                    moves += 1;

                }
                
            }
        }

        // std::cout << "Total Moves made: " << moves << std::endl;

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

    std::cout << "Input tempurature: ";
    std::cin >> Temp;
    fflush(stdin);

    // std::cout << "Input External E Field: ";
    // std::cin >> Efield;
    // fflush(stdin);
    std::cout << "External E Field set to 0 for now" << std::endl;
    

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

    motorcycle.get_inputs();
    std::cout << "Will preform " << max_sweeps << " sweeps\n\t" << motorcycle.get_total_sites() << " steps ever sweep" << std::endl;

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
    std::cout << std::endl;

    for (int s = 1; s < max_sweeps + 1; s++) {
        motorcycle.sweep();

    }

    if (print_final_lattice == 1) {
        std::cout << "Final Lattice:" << std::endl;
        motorcycle.print_lattice();
    }


}