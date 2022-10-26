#include <vector>
#include <cmath>
#include <iostream>
#include <fftw3.h>
#include "../HeaderFiles/ran2cpp.h"

// -lfftw3 -lm

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
    double sfave;
    int R, C;

    std::cout << "Input # of Rows: ";
    std::cin >> R;
    std::cout << "Input # of Cols: ";
    std::cin >> C;
    int N = R * C;
    int P = N / 2;

    std::vector<std::vector<int>> lattice;

    fftw_complex *in2D, *out2D;
    fftw_plan p;

    in2D = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out2D = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    p = fftw_plan_dft_2d(R, C, in2D, out2D, FFTW_FORWARD, FFTW_ESTIMATE);
    // p = fftw_plan_dft_r2c_2d(R, C, in2D, out2D, FFTW_FORWARD, FFTW_ESTIMATE);

    lattice.resize(R);
    for (int r = 0; r < R; r++) {
        lattice[r].resize(C);
    }

    std::cout << "\nInit Vertical Lattice" << std::endl;
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (c < C / 2) {lattice[r][c] = 1;}
            else {lattice[r][c] = 0;}
        }
    }
    
    std::cout << "\nPrinting Lattice\n" << std::endl;
    
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (lattice[r][c] == 1) {std::cout << "@ ";}
            else {std::cout << "  ";}
            in2D[c + (C * r)][REAL] = lattice[r][c];
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "FFT of R" << std::endl;

    sfave = 0;

    fftw_execute(p);

    for (int x = 0; x < N; x++) {std::cout << "X: " << x << " = " << out2D[x][REAL] << " + " << out2D[x][IMAG] << "i" << std::endl;}
    std::cout << "K = (2 pi / R, 0) = " << out2D[C * (R - 1)][REAL] << " + " << out2D[C * (R - 1)][IMAG] << "i" << std::endl;
    std::cout << "K = (1, 0) = " << out2D[C][REAL] << " + " << out2D[C][IMAG] << std::endl;
    std::cout << "K = (0, 2 pi / C) = " << out2D[C - 1][REAL] << " + " << out2D[C - 1][IMAG] << "i" << std::endl;
    std::cout << "K = (0, 1) = " << out2D[1][REAL] << " + " << out2D[1][IMAG] << std::endl;
    std::cout << "K = (2 pi / R, 2 pi / C) = " << out2D[N][REAL] << " + " << out2D[N][IMAG] << "i" << std::endl;

    // for (int x = 0; x < N; x++ ){
    //     sfave += out2D[x][REAL]*out2D[x][REAL] + out2D[x][IMAG]*out2D[x][IMAG];
    // }
    sfave = (1/(double)N) * sfave;

    // fftw_destroy_plan(p);
    // fftw_free(p);

    std::cout << "SF = <|s(k)|> = " << sfave << std::endl << "Finished FFT of R" << std::endl;


  

    std::cout << "\n\nInit Horizontal Lattice" << std::endl;
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            lattice[r][c] = 0;
        }
    }
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (r < R / 2) {lattice[r][c] = 1;}
            else {lattice[r][c] = 0;}
        }
    }

    std::cout << "\nPrinting Lattice\n" << std::endl;
    
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (lattice[r][c] == 1) {std::cout << "@ ";}
            else {std::cout << "  ";}
            in2D[c + (C * r)][REAL] = lattice[r][c];
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "FFT of R" << std::endl;

    sfave = 0;

    fftw_execute(p);

    for (int x = 0; x < N; x++) {std::cout << "X: " << x << " = " << out2D[x][REAL] << " + " << out2D[x][IMAG] << "i" << std::endl;}
    std::cout << "K = (2 pi / R, 0) = " << out2D[C * (R - 1)][REAL] << " + " << out2D[C * (R - 1)][IMAG] << "i" << std::endl;
    std::cout << "K = (1, 0) = " << out2D[C][REAL] << " + " << out2D[C][IMAG] << std::endl;
    std::cout << "K = (0, 2 pi / C) = " << out2D[C - 1][REAL] << " + " << out2D[C - 1][IMAG] << "i" << std::endl;
    std::cout << "K = (0, 1) = " << out2D[1][REAL] << " + " << out2D[1][IMAG] << std::endl;
    std::cout << "K = (2 pi / R, 2 pi / C) = " << out2D[N][REAL] << " + " << out2D[N][IMAG] << "i" << std::endl;

    // for (int x = 0; x < N; x++) {std::cout << "X: " << x << " = " << out2D[x][REAL] << " + " << out2D[x][IMAG] << "i" << std::endl;}
    // for (int x = 0; x < N; x++ ){
    //     sfave += out2D[x][REAL]*out2D[x][REAL] + out2D[x][IMAG]*out2D[x][IMAG];
    // }
    // sfave = (1/(double)N) * sfave;

    // fftw_destroy_plan(p);
    // fftw_free(p);

    std::cout << "SF = <|s(k)|> = " << sfave << std::endl << "Finished FFT of R" << std::endl;



    std::cout << "\n\nInit Random Lattice" << std::endl;
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            lattice[r][c] = 0;
        }
    }
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
            in2D[c + (C * r)][REAL] = lattice[r][c];
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "FFT of R" << std::endl;

    sfave = 0;

    fftw_execute(p);

    for (int x = 0; x < N; x++) {std::cout << "X: " << x << " = " << out2D[x][REAL] << " + " << out2D[x][IMAG] << "i" << std::endl;}
    std::cout << "K = (2 pi / R, 0) = " << out2D[C * (R - 1)][REAL] << " + " << out2D[C * (R - 1)][IMAG] << "i" << std::endl;
    std::cout << "K = (1, 0) = " << out2D[C][REAL] << " + " << out2D[C][IMAG] << std::endl;
    std::cout << "K = (0, 2 pi / C) = " << out2D[C - 1][REAL] << " + " << out2D[C - 1][IMAG] << "i" << std::endl;
    std::cout << "K = (0, 1) = " << out2D[1][REAL] << " + " << out2D[1][IMAG] << std::endl;
    std::cout << "K = (2 pi / R, 2 pi / C) = " << out2D[N][REAL] << " + " << out2D[N][IMAG] << "i" << std::endl;

    // for (int x = 0; x < N; x++) {std::cout << "X: " << x << " = " << out2D[x][REAL] << " + " << out2D[x][IMAG] << "i" << std::endl;}
    // for (int x = 0; x < N; x++ ){
    //     sfave += out2D[x][REAL]*out2D[x][REAL] + out2D[x][IMAG]*out2D[x][IMAG];
    // }
    // sfave = (1/(double)N) * sfave;

    // fftw_destroy_plan(p);
    // fftw_free(p);

    std::cout << "SF = <|s(k)|> = " << sfave << std::endl << "Finished FFT of R" << std::endl;

    fftw_destroy_plan(p);

}