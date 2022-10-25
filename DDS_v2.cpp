# include <iostream>
# include <vector>
# include <random>
# include <math.h>
# include <map>
# include <fstream>
# include <numeric>
# include <string>
# include <array>
# include <iomanip>
# include <filesystem>
# include <fftw3.h>
# include "HeaderFiles/ran2cpp.h"

#define REAL 0
#define IMAG 1

// compile with this line!
// g++ DDW_v2.cpp -lfftw3 -lm

namespace fs = std::filesystem;

// Declaring seed to be a global variable. Trying to avoid declaring anything else as global
int seed;
int const total_measures = 4;


// Just pauses the script and waits for the user to press enter
void pause_for_input() {
    fflush(stdin);
    std::cout << "\nPress Enter to continue\n";
    // Clears the buffer of any extra bits
    fflush(stdin);
    getchar();
    fflush(stdin);

}


// Generate random int: by default this is from [0, 1]
// Note it actually generates a number between [0, 2) so when being used it should thought of as [min, max + 1)
int random_int(int min = 0, int max = 2)
{

    double rval = NR::ran2(seed);
    // if (print_rval){std::cout << "Rval: " << rval;}
    int i = min + ((max - min) * rval);
    // if (print_rval){std::cout << " Rnt: " << i << " Seed : " << seed << std::endl;}
    return i;
}



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

    // Memory allocation for FFT  and FFT in/out & plan
    fftw_complex *in2D, *out2D;
    fftw_plan plan;


    // Boltzman Energy. jolt ties to J, holt ties to h
    // Saving these as Maps, with their key as the DE and values as the actual exp
    std::map<int, double> joltzman;
    std::map<int, double> eoltzman;

    // Initialize the arrays that hold the Boltzman values
    // sadly, this is not a dune reference
    void init_eoltzman() {
        for (int i = -1; i <=1; i++ ) {
            eoltzman[i] = exp((E * i) / T);
        }

    }

    // Initialize the arrays that hold the Boltzman values
    // joltzman for J Boltzman
    void init_joltzman() {
        for (int i = 0; i <= 8; i++) {
            joltzman[i] = exp((-2.0 * i) / T);  // for loop goes from 0 to 8
        }

    }


    void init_fftw() {
        in2D = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        out2D = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

        plan = fftw_plan_dft_2d(R, C, in2D, out2D, FFTW_FORWARD, FFTW_ESTIMATE);
    }


    // Place spins on the lattice
    void init_lattice() {

        double SaveE = E;
        E = 0;
        std::array<double, 4> dump;  // These will not be used, they are just to save data that is going to be dumped.

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
            bool looking = true;

            do {
                int i = random_int(0, R); // Generate a random position
                int j = random_int(0, C);
                int local_site = lattice[i][j]; // get the value at that position
                if (local_site == 0) {lattice[i][j] = 1; looking = false;} // if it's 0, make it 1, else keep looking
            } while(looking);
        }


        init_joltzman();
        init_eoltzman();
        // print_boltzman();

        std::cout << "Warming up lattice with E = 0 for 15,000 sweeps" << std::endl;
        for (int s = 0; s < 15000; s++) {dump = sweep();}

        std::cout << "Warming up lattice with E = " << SaveE << " for 15,000 sweeps" << std::endl;
        eoltzman.clear();
        E = SaveE;
        init_eoltzman();
        // print_boltzman();
        for (int s = 0; s < 15000; s++) {dump = sweep();}
    }


    // Density of +
    void Pplus() {
        int sites = R * C;

        float localP = p * (float)sites;
        P = (int)localP;

        // return psites;
    }


    // Finds the nearest neighbor to the randomly selected particle
    // then picks a direction and at random: order is starts at bottom and goes cw
    std::array<int, 2> nearest_neighbor(int row, int col, int dir) {

        int rowp, colp;
        std::array<int, 2> prime;

        if (dir == 1) {rowp = (row + 1) % R; colp = col;}  // down one
        else if (dir == 2) {rowp = row; colp = (col - 1 + C) % C;}  // left one
        else if (dir == 3) {rowp = (row - 1 + R) % R; colp = col;} // up one
        else if (dir == 4) {rowp = row; colp = (col + 1) % C;}  // right one


        prime = {rowp, colp};

        return prime;

    }

    // Finds the positions of the nearest neighbors
    // Order being returned is Down, Left, Up, Right: starts at bottom goes cw
    std::array<int, 4> nearest_neighbors(int r, int c) {
        std::array<int, 4> nn;

        int rowu = (r - 1 + R) % R; // up one
        int colr = (c + 1) % C; // right one
        int rowd = (r + 1) % R; // down one
        int coll = (c - 1 + C) % C; // left one

        nn = {rowd, coll, rowu, colr};  // while the above does not start at the bottom and go cw, the return part does

        return nn;

    }


    // made this a seperate function to make it easier to read
    int set_charge(int sk, int skp, int dir) {
        int q;

        if (dir == 1) // if the direction is going down (increasing Row #), it's going with the field
        {
            if (sk == 1) {q = 1;} // if it's 1 then it's a + charge
            else {q = -1;} // else it's a - charge
        }
        else if (dir == 3) // if the direction is going up (decreasing Row #), it's going against the field
        {
            if (sk == 1) {q = -1;} // if it's a 1 then it's a + charge
            else {q = 1;} // else it's a - charge
        }
        else {q = 0;}  // Else it's going perpendicular to the field and therefore we don't care.

        return q;

    }


public:

    int delta_energy(int row, int col, int rop, int cop) {
        int local_energy, prime_energy, return_energy;
        // std::array<int, 2> prime_position = nearest_neighbor(row, col, dir);

        // std::cout << lattice[row][col] << std::endl;//"\t" << lattice[prime_position[0]][prime_position[1]] << std::endl;

        std::array<int, 4> local_neighbors = nearest_neighbors(row, col);
        std::array<int, 4> prime_neighbors = nearest_neighbors(rop, cop);

        int rowd = local_neighbors[0]; int colr = local_neighbors[1]; int rowu = local_neighbors[2]; int coll = local_neighbors[3];
        int ropd = prime_neighbors[0]; int copr = local_neighbors[1]; int ropu = local_neighbors[2]; int copl = local_neighbors[3];

        local_energy = (lattice[rowd][col] + lattice[row][colr] + lattice[rowu][col] + lattice[row][coll]) * lattice[row][col]; 
        prime_energy = (lattice[ropd][cop] + lattice[rop][copr] + lattice[ropu][cop] + lattice[rop][copl]) * lattice[rop][cop];

        return_energy = local_energy + prime_energy;

        return return_energy;

    }


    // The road to hell is paved in minus signs
    // assumes the move has been accepted
    int get_current(int sk, int dir) {
        int current_dir = 0;

        if (sk == 1) 
        {
            if (dir == 1) {current_dir = 1;}
            else if (dir == 3) {current_dir = -1;}
        }
        else if (sk == 0)
        {
            if (dir == 1) {current_dir = -1;}
            else if (dir == 3) {current_dir = 1;}
        }

        return current_dir;

    }


    // In case I need the total spins
    void get_density() {
        std::cout << "Soft Density is:\tp = " << p << "\tP = " << P << std::endl;
        
        int hard_count = 0;

        for (int r = 0; r < R; r++){
            for (int c = 0; c < C; c++) {
                if (lattice[r][c] == 1) {hard_count += 1;}
            }
        }

        std::cout << "The hard count of 1s is " << hard_count << std::endl;
    }



    // just outputs the current parameters
    void get_inputs() {

        std::cout << "\nInputs:" << std::endl;
        std::cout << "Current Seed: " << seed << std::endl;
        std::cout << "Dimensions of Lattice: " << R << " x " << C << std::endl;
        std::cout << "Density of 1s (% & #): " << p << " " << P << "\tTempurature: " << T << "\tE Field: " << E << "\tJ = k (1, dimensionless)" << std::endl;
        print_boltzman();
        // std::cout << "Sweeps: " << max_sweeps << std::endl;
        // std::cout << "Measurement will be taken every 1/" << partial << "sweep" << std::endl;


    }

    // In case I need the total spins
    float get_total_sites() {
        return (N);
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
                if (lattice[i][j] == 1) { std::cout << "@" << " "; }
                else if (lattice[i][j] == 0) { std::cout << " " << " "; } 
            }
            std::cout << "\n";
        }

        std::cout << "\n";
    }


    // Assumes that the memory has already been allocated and that the plan has already been established
    // IMPORTANT NOTE: the resulting 2D Spin Fluctuation lattice is actually a 1D vectory/array, and therefore
    // must be indexed by i = c + Cr where C is the number of columns, c is the specific column, and r is the
    // specific row. You will generally want i = C(R - 1) for k(1, 0) and i = C - 1 for k(0, 1)
    std::array< double, 2> structure_factor() {
        double k10, k01;
        int index10 = C * (R - 1), index01 = C - 1;

        for (int r = 0; r < R; r++) {
            for (int c = 0; c < C; c++) {
                in2D[c + (C * r)][REAL] = lattice[r][c];
            }
        }

        fftw_execute(plan);

        k10 = (out2D[index10][REAL] * out2D[index10][REAL]) + (out2D[index10][IMAG] * out2D[index10][IMAG]);
        k01 = (out2D[index01][REAL] * out2D[index01][REAL]) + (out2D[index01][IMAG] * out2D[index01][IMAG]);

        return {k10, k01};

    }



    std::array<double, total_measures> sweep() {
        std::array<double, total_measures> measurements;
        double Nplus = 0, Nminus = 0, current, exchanges;
        int sk, skp;  // S sub k and S sub k prime

        for (int n = 1; n < N + 1; n++) {
            int rran, cran, rrap, crap, dir, q;
            int energy;
            int exchange_one, exchange_two;

            // std::cout << "\tline 317";
            rran = random_int(0, R); 
            cran = random_int(0, C); // gets a random position

            // std::cout << "\tline 321";
            dir = random_int(1, 5); // returns a number between 1 and 4

            std::array<int, 2> prime_position = nearest_neighbor(rran, cran, dir);
            rrap = prime_position[0]; crap = prime_position[1];  // gets the nearest neighbor in a direction

            sk = lattice[rran][cran]; skp = lattice[rrap][crap];

            if (sk != skp) {  // checks if those neighbors are different: if not then proceed

                // nearest_neighbor
                energy = delta_energy(rran, cran, rrap, crap); // Does not look at the Field

                q = set_charge(sk, skp, dir);

                double probability = joltzman[energy] * eoltzman[q];
                double chance = NR::ran2(seed);

                if (probability >= chance) {
                    exchange_one = lattice[rran][cran];
                    exchange_two = lattice[rrap][crap];

                    lattice[rran][cran] = exchange_two;
                    lattice[rrap][crap] = exchange_one;

                    if (get_current(sk, dir) == 1) {Nplus += 1;} // current with field, charge against
                    else if (get_current(sk, dir) == -1) {Nminus += 1;} // b/c Fuck Ben Franklin

                }
            }
        }

        current = (1/(double)N)* (Nplus - Nminus);
        exchanges = (1/(double)N)* (Nplus + Nminus);

        measurements = {current, exchanges, Nplus, Nminus};

        return measurements;

    }

    void write_lattice(fs::path write_path) {

        ofstream myfile (write_path);
        if (myfile.is_open()) {
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < C; c++) {
                    if (lattice[r][c] == 1) {myfile << "@" << " ";}
                    else {myfile << " " << " ";}
                }
                myfile << std::endl;
            }
            myfile.close();
        }
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
        // init_joltzman();
        // init_eoltzman();
        init_fftw();

    };

};



void check_folder(fs::path file_path) {
    if (fs::exists(file_path)) {std::cout << file_path << " Already exists" << std::endl;}
    else {fs::create_directory(file_path); std::cout << file_path << " Created" << std::endl;}
}



// writes the Mag and mag to a csv file. Do not include the .csv portion of the filename, that will be added later
void write_csv(fs::path directory, std::string master_filename, std::vector<double> Current, std::vector<double> Exchanges, std::vector<double> k10, std::vector<double> k01) { //, std::vector<float> Energy, std::vector<float> energy) {

    int Lmod = 1000000;
    int L = Current.size(); // this really only works if Mag and mags are the same size, which they should be.

    int iters = 1;
    int DL = L - Lmod;
    int extra_iters = DL % Lmod;  // this is becasue everything is going to be broken up into seperate files

    while (DL > 0) {
        extra_iters = DL % Lmod;
        DL -= Lmod;
        iters += 1;
    }

    int start = 0;
    int end;
    for (int I = 0; I < iters; I++) {
        std::string filename = "Part_" + std::to_string(I + 1) + "x" + std::to_string(iters) + "_" + master_filename + ".csv";
        fs::path output_path = directory / filename;
        std::cout << "Filepath: " << output_path << std::endl;
        std::ofstream fileout(output_path);

        if (((I + 1) >= iters) && (extra_iters > 0)) {
            end = start + extra_iters - 1;
            std::cout << "writing file part " << I + 1 << " with excess iters " << extra_iters << std::endl;
            std::cout << "Start = " << start << " End = " << end << std::endl;

        }
        else if (extra_iters < 0) {
            end = start + Lmod + extra_iters - 1;
            std::cout << "writing file part " << I + 1 << " with excess iters " << Lmod + extra_iters << std::endl;
            std::cout << "Start = " << start << " End = " << end << std::endl;
        }
        else {
            end = start + Lmod - 1;
            std::cout << "writing file part " << I + 1 << std::endl;
            std::cout << "Start = " << start << " End = " << end << std::endl;
        }

        fileout << "sweep,Current,Exchanges,SFk10,SFk01\n";
        for (long i = start; i < end; i++) {
            long index = ((i + 1) / 1);
            // std::cout << "Index: " << index << " Current: " << Current.at(i) << " Exchanges: " << Exchanges.at(i) << std::endl;
            fileout << index << "," << Current.at(i) << "," << Exchanges.at(i) << k10.at(i) << k01.at(i) << "\n"; //"," << Energy.at(i) << "," << energy.at(i) << "\n";
        }
        start += Lmod;

        fileout.close();
    }

    std::cout << "Finished Writing to csv(s)" << std::endl;
}



int main() {
    int Rows, Columns, max_sweeps, print_first_lattice, print_final_lattice, init_seed;
    double Temp, Coupeling, Efield;
    float Density;
    std::array<double, 2> sf;
    std::array<std::vector<double>, total_measures + 2> measurements;

    fs::path image_output, data_output, lattice_images, lattice_data, image_filename ;
    std::string folder_name;
    
    lattice_images = fs::current_path() / "LatticeImages";
    lattice_data = fs::current_path() / "Data";

    check_folder(lattice_images);
    check_folder(lattice_data);   

    // std::cout.precision(17);

    std::cout << "Input Seed\nSeed must not be 0\nInput Seed: ";
    std::cin >> init_seed;
    fflush(stdin);

    seed = init_seed;
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

    std::cout << "Input External E Field: ";
    std::cin >> Efield;
    fflush(stdin);
    // std::cout << "External E Field set to 0 for now" << std::endl;
    

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
    // for (int i = 0; i < total_measures; i++) {measurements[i].resize(max_sweeps);}

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

    folder_name = "Temp_" + std::to_string(Temp) + "_Efield_" + std::to_string(Efield) + "_Size_" + std::to_string(Rows) + "x" + std::to_string(Columns) + "_seed_" + std::to_string(init_seed);

    image_output = lattice_images / folder_name;
    check_folder(image_output);
    data_output = lattice_data / folder_name;
    check_folder(data_output);
    
    // std::cout << "Ready to begin simulation\nNote: this does NOT include a warm up phase" << std::endl;
    fflush(stdin);
    std::cout << "Ready to begin simulation" << std::endl;
    pause_for_input();
    // std::cout << std::endl;


    for (int s = 0; s < max_sweeps; s++) {
        std::array<double, 4> local_measurement;
        local_measurement = motorcycle.sweep();
        // std::cout << "Finished sweep " << s << std::endl;

        for (int i = 0; i < total_measures; i++) {measurements[i].push_back(local_measurement[i]);}

        sf = motorcycle.structure_factor();

        measurements[total_measures].push_back(sf[0]); measurements[total_measures + 1].push_back(sf[1]);

        if ((s % 100000) == 0) {
            std::cout << "Completed Sweep " << s << std::endl;
            // std::cout << "Measured Current = " << std::setprecision(5) << local_measurement[0] << " Exchanges = " << local_measurement[1] << " N+ = " << local_measurement[2] << " N- = " << local_measurement[3] << std::endl;
            // std::cout << "Measured Current = " << typeid(local_measurement[0]).name() << " Exchanges = " << typeid(local_measurement[1]).name() << " N+ = " << typeid(local_measurement[2]).name() << " N- = " << typeid(local_measurement[3]).name() << std::endl;

            // measurement_folder.push_back({local_measurement[0], local_measurement[1], local_measurement[2], local_measurement[3]});

            // print lattice to file
            // 
            image_filename = std::to_string(s) + "_Current_" + std::to_string(local_measurement[0]) + "_Exchanges_" + std::to_string(local_measurement[1]) + "_Nplus_" + std::to_string(local_measurement[2]) + "_Nminus_" + std::to_string(local_measurement[3]) + ".txt";
            image_filename = image_output / image_filename;
            motorcycle.write_lattice(image_filename);

        }


    }

    if (print_final_lattice == 1) {
        std::cout << "Final Lattice:" << std::endl;
        motorcycle.print_lattice();
    }

    // motorcycle.get_density();

    std::string filename = "Temp_" + std::to_string(Temp) + "_Size_" + std::to_string(Rows) + "x" + std::to_string(Columns) + "_seed_" + std::to_string(init_seed) + "_sweeps_" + std::to_string(max_sweeps);

    write_csv(data_output, filename, measurements[0], measurements[1], measurements[total_measures], measurements[total_measures + 1]);

    // for (int i = 0; i < measurements[0].size(); i++) {
    //     std::cout << "Current: " << measurements[0][i] << std::endl;
    // }


}