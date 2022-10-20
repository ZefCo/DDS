#include <vector>
#include "../HeaderFiles/nr.h"
// #include "../HeaderFiles/rlft3.h"
// #include <fftw3.h>
// #include "fourn.h"
#include "../HeaderFiles/fourn.h"


// Counts the number of digits for a number
int count_digits(int number) {
    int count = 0;

    while(number != 0) {
        number = number / 10;
        count++;
    }
    
    return count;

}



int main() {
    int R = 20, C = 20, Z = 1;

    Vec_IO_DP fuckNRin1(R);
    Vec_IO_DP fuckNRin2(R);
    Vec_I_INT fuckNRout(R);

    std::cout << "Creating lattices" << std::endl;

    for (int i = 0; i < R; i++) {fuckNRin1[i] = 1.0;}
    // for (int i = 0; i < R; i++) {if (i < 10) {fuckNRin2[i] = 0.0;} else {fuckNRin2[i] = 1.0;}}

    std::cout << "Starting Fourier transform" << std::endl;

    NR::fourn(fuckNRin1, fuckNRout, 1);

    std::cout << "Finished fourier" << std::endl;
    
    // fuckNRin.resize(R);
    // for (int r = 0; r < R; r++) {
    //     fuckNRin[r].resize(C);
    // }

    // std::cout << "Init Lattice" << std::endl;
    // for (int r = 0; r < R; r++) {
    //     for (int c = 0; c < C; c++) {
            
    //         if (c < 10) {fuckNRin[r][c] = (double)1;}
    //         else {fuckNRin[r][c] = (double)0;}

    //     }
    // }

    // std::cout << "\nPrinting Lattice\n" << std::endl;
    
    // for (int r = 0; r < R; r++) {
    //     for (int c = 0; c < C; c++) {
    //         if (fuckNRin[r][c] == 1) {std::cout << "@ ";}
    //         else {std::cout << "  ";}
    //     }
    //     std::cout << std::endl;
    // }

    // for (int r = 0; r < R; r++) {
    //     std::vector<double> copyRow(fuckNRin[r].begin(), fuckNRin[r].end());

    //     std::cout << "Row: " << r << std::endl;
    //     for (auto item: copyRow) {std::cout << item << " ";}
    //     std::cout << std::endl;
    // }

    // for (int c = 0; c < C; c++) {
    //     std::vector<double> copyCol;
    //     for (int r = 0; r < R; r++) {
    //         copyCol.push_back(fuckNRin[r][c]);
    //     }

    //     std::cout << "T Col: " << c << std::endl;
    //     for (auto item: copyCol) {std::cout << item << " ";}
    //     std::cout << std::endl;

    // }
    
    // // std::cout << std::endl;

    // // std::cout << "Fuck Numerical Recipies" << std::endl;

    // // // fftw_plan fftw_plan_dft_2d()

    // // // try {
    // // //     std::cout << "Working on FFT" << std::endl;
    // // //     NR::rlft3(fuckNRin, speq, 1);}
    // // // catch(exception& exc) {std::cout << "Yep, fuck Numerical Recipies" << std::endl;}
    // // // // NR::rlft3(copy_data, speq, 1);
    

    // // // std::cout << "Finished beng fucked by Numerical Recipies" << std::endl;

    // // // // NR::fourn(fuckNRin, );

    // // std::cout <<"No but seriously fuck NR" << std::endl;

}