#include "nr.h"
#include <vector>
#include "rlft3.h"
// #include "fourn.h"


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

    // int max_r = count_digits(R);
    // int max_c = count_digits(C);
    // int max_z = count_digits(Z);

    NRMat3d<DP> fuckNR(1, R, C);
    // Vec_IO_DP fuckNR;

    // std::vector<std::vector<int>> data;
    // std::vector<std::vector<int>> copy_data;


    // // std::cout << data[1][2][0] << std::endl;
    // // std::cout << "Passed" << std::endl;

    std::cout << "Init Lattice" << std::endl;
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            // fuckNR[r][c] = 0.0;
            // std::cout << r << "x" << c << std::endl;
            
            if (c < 10) {fuckNR[0][r][c] = 1;}
            else {fuckNR[0][r][c] = 0;}

            // std::cout << r << "x" << c << " = " << fuckNR[r][c] << std::endl;
        }
    }
    // data.resize(R);
    // for (int r = 0; r < R; r++) {
    //     data[r].resize(C);
    // }

    // for (int r = 0; r < R; r++) {
    //     for (int c = 0; c < C; c++) {
    //         if (c < 10) {data[r][c] = 1;}
    //         else {data[r][c] = 0;}
    //     }
    // }

    std::cout << "Printing Lattice" << std::endl;
    
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (fuckNR[0][r][c] == 1) {std::cout << "@ ";}
            else {std::cout << "  ";}
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;

    Mat3D_IO_DP copy_data = fuckNR;
    Mat_IO_DP speq(1, 2*R);

    // for (int s = 0; s < 2*R; s++) {speq[0][s] = 0;}
    std::cout << "Fuck Numerical Recipies" << std::endl;

    try {
        std::cout << "Working on FFT" << std::endl;
        rlft3(copy_data, speq, 1);}
    catch(exception& exc) {std::cout << "Yep, fuck Numerical Recipies" << std::endl;}
    // NR::rlft3(copy_data, speq, 1);
    

    std::cout << "Finished beng fucked by Numerical Recipies" << std::endl;

    // NR::fourn(fuckNR, );

}