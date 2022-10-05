# include <iostream>
# include <vector>
# include <random>
# include <math.h>
# include <map>
# include <fstream>
# include <numeric>
# include <string>
// # include "numRec.h"

// Declaring seed to be a global variable. Trying to avoid declaring anything else as global
long seed;


/* RAN2 from Numerical Methods in C
Keeping this here in case I want to use it. It works, but it works differently then ran2 from cpp. This chunk came from Numerical Recipies in C
as given by Dr. Bassler. The ran2 used here is from Numerical Recipies in C++, which is written by the same people but as you can see uses some values differently.
**/
// definitions for ran2
# define IM1 2147483563
# define IM2 2147483399
# define AM (1.0/IM1)
# define IMM1 (IM1-1)
# define IA1 40014
# define IA2 40692
# define IQ1 53668
# define IQ2 52774
# define IR1 12211
# define IR2 3791
# define NTAB 32
# define NDIV (1+IMM1/NTAB)
# define EPS 1.2e-7
# define RNMX (1.0-EPS)

// ran2, copied from page 282 of Numerical Recipies (defines at top)
float ran2(long *idum)
{

    int j;
    long k;
    static long idum2 = 123456789;
    static long iy = 0;
    static long iv[NTAB];
    float temp;

    if (*idum <= 0) {                     // This just initilzes
        if (-(*idum) < 1) *idum = 1;  // prevents idum from being 0
        else *idum = -(*idum);

        idum2 = (*idum);

        for (j = NTAB + 7; j >=0; j--) {  // shuffles the values
            k = (*idum) / IQ1;
            *idum = IA1 * (*idum - k*IQ1) - k*IR1;  // pretty sure this is *idum - (k*IQ1), hence the lack of spacing
            if (*idum < 0) *idum += IM1; 
            if (j < NTAB) iv[j] = *idum;
        }
        iy = iv[0];
    }

    k = (*idum) / IQ1;
    *idum = IA1 * (*idum - k*IQ1) - k*IQ1;

    if (*idum < 0) *idum += IM1;

    k = idum2 / IQ2;
    idum2 = IA2 * (idum2 - k*IQ2) - k*IQ2;

    if (idum2 < 0) idum2 += IM2;

    j = iy / NDIV;
    iy = iv[j] - idum2;
    iv[j] = *idum;

    if (iy < 1) iy += IMM1;
    if ((temp = AM* iy) > RNMX) return RNMX;
    else return temp;

}

#undef IM1
#undef IM2
#undef AM
#undef IMM1
#undef IA1
#undef IA2
#undef IQ1
#undef IQ2
#undef IR1
#undef IR2
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX




// Generate random int: by default this is from [0, 1]
// Note it actually generates a number between [0, 2) so when being used it should thought of as [min, max + 1)
int random_int(int min = 0, int max = 2)
{

    float rval = ran2(&seed);
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

    std::cout << "Input Seed: ";
    std::cin >> seed;
    fflush(stdin);

    std::cout << "Input [square] lattice size: ";
    std::cin >> Rows;
    fflush(stdin);

    Columns = Rows;

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