#include <vector>
#include <cmath>
#include <iostream>
#include <fftw3.h>
#include "../HeaderFiles/ran2cpp.h"

int seed = -123456789;

#define REAL 0
#define IMAG 1


int random_int(int min = 0, int max = 2)
{

    double rval = NR::ran2(seed);
    int i = min + ((max - min) * rval);
    // std::cout << "Rval: " << rval << " Rnt: " << i << std::endl;

    return i;
}


int main() {
    int R, C;

    std::cout << "Input # of Rows: ";
    std::cin >> R;
    std::cout << "Input # of Cols: ";
    std::cin >> C;
    int N = R * C;
    int P = N / 2;

    std::vector<std::vector<int>> lattice;

    fftw_complex *inR, *outR, *inC, *outC;
    fftw_plan pR, pC;

    inR = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * R);
    outR = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * R);
    inC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * C);
    outC = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * C);

    pR = fftw_plan_dft_1d(R, inR, outR, FFTW_FORWARD, FFTW_ESTIMATE);
    pC = fftw_plan_dft_1d(C, inC, outC, FFTW_FORWARD, FFTW_ESTIMATE);


    lattice.resize(R);
    for (int r = 0; r < R; r++) {
        lattice[r].resize(C);
    }

    std::cout << "Init Lattice" << std::endl;
    for (int k = 0; k < P; k++) {
        bool looking = true;

        do {
            int i = random_int(0, R); // Generate a random position
            int j = random_int(0, C);
            int local_site = lattice[i][j]; // get the value at that position
            if (local_site == 0) {lattice[i][j] = 1; looking = false;} // if it's 0, make it 1, else keep looking
        } while(looking);
    }            


    std::cout << "\nPrinting Lattice\n" << std::endl;
    
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (lattice[r][c] == 1) {std::cout << "@ ";}
            else {std::cout << "  ";}
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "FFT of R" << std::endl;

    double sfaveR = 0;

    for (int r = 0; r < R; r++) {
        // std::cout << r << " ";
        for (int c = 0; c < C; c++){inR[c][REAL] = lattice[r][c];}

        fftw_execute(pR);

        // for (int x = 0; x < C; x++) {std::cout << "X: " << x << " = " << outR[x][REAL] << " + " << outR[x][IMAG] << "i" << std::endl;}
        for (int x = 0; x < C; x++) {
            // std::cout << "X: " << x << " = " << outR[x][REAL] << " + " << outR[x][IMAG] << "i" << std::endl;
            double real = pow(outR[x][REAL], 2);
            double imag = pow(outR[x][IMAG], 2);

            sfaveR += real + imag;
            }
    }

    sfaveR = sfaveR / (double)R;

    fftw_destroy_plan(pR);

    std::cout << "SF = <|s(k)|> = " << sfaveR << std::endl << "Finished FFT of R" << std::endl;

    double sfaveC = 0;

    std::cout << std::endl << "FFT of C" << std::endl;

    for (int c = 0; c < C; c++) {
        // std::cout << c << " ";
        for (int r = 0; r < R; r++) {inC[r][REAL] = lattice[r][c];}

        fftw_execute(pC);

        // for (int x = 0; x < C; x++) {std::cout << "X: " << x << " = " << outC[x][REAL] << " + " << outC[x][IMAG] << "i" << std::endl;}
        for (int x = 0; x < C; x++) {
            // std::cout << "X: " << x << " = " << outC[x][REAL] << " + " << outC[c][IMAG] << "i" << std::endl;
            double real = pow(outC[x][REAL], 2);
            double imag = pow(outC[x][IMAG], 2);

            sfaveC += real + imag;
            }
    }

    sfaveC = sfaveC / (double)C;
    sfaveC = sfaveC;

    std::cout << "SF = (1/N) * <|s(k)|> = " << sfaveC << std::endl << "Finished FFT" << std::endl;

    fftw_destroy_plan(pC);

}