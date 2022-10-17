# include <iostream>
# include <vector>
# include <random>
# include <math.h>
# include <map>
# include <fstream>
# include <numeric>
# include <string>
# include "ran2.h"
# include "rlft3.h"
# include <array>

// Declaring seed to be a global variable. Trying to avoid declaring anything else as global
int seed;


// Just pauses the script and waits for the user to press enter
void pause_for_input() {
    std::cout << "\nPress Enter to continue\n";
    // std::cin.clear();
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
    // // # of +
    // int P;

    // The lattice
    std::vector<std::vector<int>> lattice;

    // Energy
    int energy;


    // Boltzman Energy. jolt ties to J, holt ties to h
    // Saving these as Maps, with their key as the DE and values as the actual exp
    std::map<int, double> joltzman;
    std::map<int, double> eoltzman;


    // Struct to hold all the 1s
    struct POnes
    {
        std::vector<std::array<int, 2>> oindex;
        int length;

    };


    // Struct to hold all the 0s
    struct PZeros 
    {
        std::vector<std::array<int, 2>> zindex;
        int length;

    };

    POnes PO;
    PZeros PZ;


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

        //     float localP = p * (float)sites;
        //     P = (int)localP;

        PO.length = (int)(p * (float)N);
        PZ.length = N - PO.length;
        // std::cout << "### " << PO.length << "  " << PZ.length << " ###" << std::endl;


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
        // The reason for doing it like this - making the lattice, placing holes, then placing the gas - is to
        // preserve the density. 
        for (int k = 0; k < PO.length; k++) {
            bool looking = true;

            do {
                int i = random_int(0, R); // Generate a random position
                int j = random_int(0, C);
                int local_site = lattice[i][j]; // get the value at that position
                if (local_site == 0) {lattice[i][j] = 1; looking = false;} // if it's 0, make it 1, else keep looking
            } while(looking);

            
        }

        // Locate holes and gas, index them in the Structs
        int osafety = 0, zsafety= 0;
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                int gas = lattice[i][j];

                if (gas == 1) {PO.oindex.push_back({i, j}); osafety += 1;}
                else {PZ.zindex.push_back({i, j}); zsafety += 1;}
            }
        }

        if (PO.length != osafety) {PO.length = osafety;} // Makes sure that everything actually did what it's supposed to do.
        if (PZ.length != zsafety) {PZ.length = zsafety;} // Well there are better ways to do it then this

    }


    // // Density of +
    // void Pplus() {
    //     int sites = R * C;

    //     float localP = p * (float)sites;
    //     P = (int)localP;

    //     // return psites;
    // }


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

    int delta_energy(int row, int col, int rop, int cop) {
        int local_energy, prime_energy, return_energy;
        // std::array<int, 2> prime_position = nearest_neighbor(row, col, dir);

        // std::cout << lattice[row][col] << std::endl;//"\t" << lattice[prime_position[0]][prime_position[1]] << std::endl;

        if (lattice[row][col] != lattice[rop][cop]) {
            std::array<int, 4> local_neighbors = nearest_neighbors(row, col);
            std::array<int, 4> prime_neighbors = nearest_neighbors(rop, cop);

            int rowd = local_neighbors[0]; int colr = local_neighbors[1]; int rowu = local_neighbors[2]; int coll = local_neighbors[3];
            int ropd = prime_neighbors[0]; int copr = local_neighbors[1]; int ropu = local_neighbors[2]; int copl = local_neighbors[3];

            local_energy = (lattice[rowd][col] + lattice[row][colr] + lattice[rowu][col] + lattice[row][coll]) * lattice[row][col]; 
            prime_energy = (lattice[ropd][cop] + lattice[rop][copr] + lattice[ropu][cop] + lattice[rop][copl]) * lattice[rop][cop]; 

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
        std::cout << "Density of Ones (% & #): " << p << " " << PO.length << "\tTempurature: " << T << "\tE Field: " << E << "\tJ = k (1, dimensionless)" << std::endl;
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
        std::cout << "Density ones is:\tp = " << p << "\tP = " << PO.length << std::endl;
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


    void print_index() {
        int large_index, small_index;
        string gap_output = "          ";


        // std::cout << PO.length << " " << PZ.length << std::endl;

        if (PO.length == PZ.length) {small_index = PO.length; large_index = 0;}
        else if (PO.length > PZ.length) {large_index = PO.length; small_index = PZ.length;}
        else {large_index = PZ.length; small_index = PO.length;}
        
        // 4 characters - 10 characters - 5 chracaters
        std::cout << "Ones" << gap_output << "Zeros" << std::endl; 
        for (int index = 0; index < small_index; index++) {
            std::array ones = PO.oindex[index]; std::array zeroes = PZ.zindex[index];
            
            string ones_output = "(" + std::to_string(ones[0]) + ", " + std::to_string(ones[1]) + ")";
            string zeros_output = "(" + std::to_string(zeroes[0]) + ", " + std::to_string(zeroes[1]) + ")";
            
            std::cout <<  ones_output << gap_output << zeros_output << std::endl;
        }

        if (large_index > 0) {

            // std::cout << "Large index = " << large_index << std::endl;
            // std::cout << "One Index = " << PO.length << std::endl;
            // std::cout << "Zero Index = " << PZ.length << std::endl;

            if (PO.length > PZ.length){
                for (int index = small_index + 1; index < large_index; index++) {
                    std::array ones = PO.oindex[index];
                    
                    string ones_output = "(" + std::to_string(ones[0]) + ", " + std::to_string(ones[1]) + ")";
                    
                    std::cout <<  ones_output << std::endl;
                }
            }

            else if (PZ.length > PO.length){
                string gap_output = "                 ";
                for (int index = small_index + 1; index < large_index; index++) {
                    std::array zeros = PZ.zindex[index];
                    
                    string zeros_output = "(" + std::to_string(zeros[0]) + ", " + std::to_string(zeros[1]) + ")";
                    
                    std::cout << gap_output << zeros_output << std::endl;
                }
            }
        }


        // std::cout << "Ones Elements" << std::endl;
        // for (int index = 0; index <= PO.length; index++) {
        //     std::cout << "( " << element[0] << ", " << element[1] << " )" << std::endl;
        // }

        // std::cout << "Zeroes Elements" << std::endl;
        // for (std::array element: PZ.zindex) {
        //     std::cout << "( " << element[0] << ", " << element[1] << " )" << std::endl;
        // }
    }


    // // Not sure how this is going to work yet... but we're going to look at the structure function
    // void structure() {

    //     std::vector<std::vector<int>> copy = lattice; // Copies the lattice, so we don't end up screwing anything up

    // NR::rlft3(copy, )

    // }



    void sweep() {
        // std::cout << "\nline 309";
        int moves = 0;
        for (int n = 1; n < N + 1; n++) {
            // std::cout << "\tline 312";
            int oran, zran, energy;
            std::array<int, 2> one_position, zero_position;
            // std::array<int, 2> exchange_one, exchange_zero;

            // std::cout << "\tline 317";
            oran = random_int(0, PO.length); 
            zran = random_int(0, PZ.length); // gets a random position

            // std::cout << "\tline 321";
            // dir = random_int(1, 5); // returns a number between 1 and 4

            one_position = PO.oindex[oran]; zero_position = PZ.zindex[zran];

            // std::array<int, 2> prime_position = nearest_neighbor(rran, cran, dir);
            // rrap = prime_position[0]; crap = prime_position[1];

            // nearest_neighbor
            energy = delta_energy(one_position[0], one_position[1], zero_position[0], zero_position[1]);

            if (energy > -1) {
                // std::cout << "\tline 326";
                float probability = joltzman[energy];
                // std::cout << "\tline 328" << std::endl;
                float chance = NR::ran2(seed);
                // std::cout << "Chance = " << chance << " Probability = " << probability << std::endl;
                // std::cout << "probability = " << probability << std::endl;

                if (probability >= chance) {
                    PO.oindex[oran] = zero_position; PZ.zindex[zran] = one_position;
                    lattice[one_position[0]][one_position[1]] = 0;
                    lattice[zero_position[0]][zero_position[1]] = 1;
                    // std::cout << "Exchanging positions" << std::endl;
                    // std::cout << "Rran = " << rran << " Cran = " << cran << " Rran' = " << rrap << " Cran' = " << crap << std::endl;
                    // exchange_one = lattice[rran][cran];
                    // exchange_zero = lattice[rrap][crap];

                    // std::cout << exchange_one << "\t" << exchange_two << std::endl;
                    
                    // std::cout << "The actual exchange" << std::endl;
                    // lattice[rran][cran] = exchange_two;
                    // lattice[rrap][crap] = exchange_one;

                    moves += 1;

                }
            }

            // std::cout << n << std::endl;
            // print_lattice();
            // std::cout << "Total Moves made: " << moves << std::endl;

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
        // Pplus();

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
    int Rows, Columns, max_sweeps, print_first_lattice, print_final_lattice, print_position_index;
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

    std::cout << "\nDo you wish to see the positional index? 1 for yes 0 for no: ";
    std::cin >> print_position_index;
    fflush(stdin);

    if (print_position_index) {
        motorcycle.print_index();
    }

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
    // std::cout << std::endl;

    for (int s = 1; s < max_sweeps + 1; s++) {
        motorcycle.sweep();
        // std::cout << "Finished sweep " << s << std::endl;
        if ((s % 100000) == 0) {
            std::cout << "Completed Sweep " << s << std::endl;
        }


    }

    if (print_final_lattice == 1) {
        std::cout << "Final Lattice:" << std::endl;
        motorcycle.print_lattice();
    }


}