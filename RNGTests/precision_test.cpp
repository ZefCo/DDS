#include <iostream>

int main() {
    double sigma00;
    double sigma01, sigma02, sigma03, sigma04, sigma05, sigma06, sigma07, sigma08, sigma09, sigma10;
    double sigma11, sigma12, sigma13, sigma14, sigma15, sigma16, sigma17, sigma18, sigma19, sigma20;

    sigma00 = 1.0;
    sigma01 = 0.1;
    sigma02 = 0.01;
    sigma03 = 0.001;
    sigma04 = 0.0001;
    sigma05 = 0.00001;
    sigma06 = 0.000001;
    sigma07 = 0.0000001;
    sigma08 = 0.00000001;
    sigma09 = 0.000000001;
    sigma10 = 0.0000000001;
    sigma11 = 0.00000000001;
    sigma12 = 0.000000000001;
    sigma13 = 0.0000000000001;
    sigma14 = 0.00000000000001;
    sigma15 = 0.000000000000001;
    sigma16 = 0.0000000000000001;
    sigma17 = 0.00000000000000001;
    sigma18 = 0.000000000000000001;
    sigma19 = 0.0000000000000000001;
    sigma20 = 0.00000000000000000001;

    std::cout << "Base = " << sigma00 << std::endl;

    std::cout << "Sigma 01 = " << sigma01 << "\tSize = " << sizeof(sigma01) << std::endl;
    std::cout << "Sigma 02 = " << sigma02 << "\tSize = " << sizeof(sigma02) << std::endl;
    std::cout << "Sigma 03 = " << sigma03 << "\tSize = " << sizeof(sigma03) << std::endl;
    std::cout << "Sigma 04 = " << sigma04 << "\tSize = " << sizeof(sigma04) << std::endl;
    std::cout << "Sigma 05 = " << sigma05 << "\tSize = " << sizeof(sigma05) << std::endl;
    std::cout << "Sigma 06 = " << sigma06 << "\tSize = " << sizeof(sigma06) << std::endl;
    std::cout << "Sigma 07 = " << sigma07 << "\tSize = " << sizeof(sigma07) << std::endl;
    std::cout << "Sigma 08 = " << sigma08 << "\tSize = " << sizeof(sigma08) << std::endl;
    std::cout << "Sigma 09 = " << sigma09 << "\tSize = " << sizeof(sigma09) << std::endl;
    std::cout << "Sigma 10 = " << sigma10 << "\tSize = " << sizeof(sigma10) << std::endl;
    std::cout << "Sigma 11 = " << sigma11 << "\tSize = " << sizeof(sigma11) << std::endl;
    std::cout << "Sigma 12 = " << sigma12 << "\tSize = " << sizeof(sigma12) << std::endl;
    std::cout << "Sigma 13 = " << sigma13 << "\tSize = " << sizeof(sigma13) << std::endl;
    std::cout << "Sigma 14 = " << sigma14 << "\tSize = " << sizeof(sigma14) << std::endl;
    std::cout << "Sigma 15 = " << sigma15 << "\tSize = " << sizeof(sigma15) << std::endl;
    std::cout << "Sigma 16 = " << sigma16 << "\tSize = " << sizeof(sigma16) << std::endl;
    std::cout << "Sigma 17 = " << sigma17 << "\tSize = " << sizeof(sigma17) << std::endl;
    std::cout << "Sigma 18 = " << sigma18 << "\tSize = " << sizeof(sigma18) << std::endl;
    std::cout << "Sigma 19 = " << sigma19 << "\tSize = " << sizeof(sigma19) << std::endl;
    std::cout << "Sigma 20 = " << sigma20 << "\tSize = " << sizeof(sigma20) << std::endl;

    double dsigma01 = sigma00 - sigma01; float fsigma01 = (float)dsigma01;
    double dsigma02 = sigma00 - sigma02; float fsigma02 = (float)dsigma02;
    double dsigma03 = sigma00 - sigma03; float fsigma03 = (float)dsigma03;
    double dsigma04 = sigma00 - sigma04; float fsigma04 = (float)dsigma04;
    double dsigma05 = sigma00 - sigma05; float fsigma05 = (float)dsigma05;
    double dsigma06 = sigma00 - sigma06; float fsigma06 = (float)dsigma06;
    double dsigma07 = sigma00 - sigma07; float fsigma07 = (float)dsigma07;
    double dsigma08 = sigma00 - sigma08; float fsigma08 = (float)dsigma08;
    double dsigma09 = sigma00 - sigma09; float fsigma09 = (float)dsigma09;
    double dsigma10 = sigma00 - sigma10; float fsigma10 = (float)dsigma10;
    double dsigma11 = sigma00 - sigma11; float fsigma11 = (float)dsigma11;
    double dsigma12 = sigma00 - sigma12; float fsigma12 = (float)dsigma12;
    double dsigma13 = sigma00 - sigma13; float fsigma13 = (float)dsigma13;
    double dsigma14 = sigma00 - sigma14; float fsigma14 = (float)dsigma14;
    double dsigma15 = sigma00 - sigma15; float fsigma15 = (float)dsigma15;
    double dsigma16 = sigma00 - sigma16; float fsigma16 = (float)dsigma16;
    double dsigma17 = sigma00 - sigma17; float fsigma17 = (float)dsigma17;
    double dsigma18 = sigma00 - sigma18; float fsigma18 = (float)dsigma18;
    double dsigma19 = sigma00 - sigma19; float fsigma19 = (float)dsigma19;
    double dsigma20 = sigma00 - sigma20; float fsigma20 = (float)dsigma20;

    std::cout << "(Double) Delta Sigma 01 = " << dsigma01 << "      Size of DS01 (Double) = " << sizeof(dsigma01) << " (Int - D) DS01 = " << (int)dsigma01 << " (Float) FS01 = " << fsigma01 << "      Size of FS01 = " << sizeof(fsigma01) << " (Int - F) FS01 = " << (int)fsigma01 << std::endl;
    std::cout << "(Double) Delta Sigma 02 = " << dsigma02 << "     Size of DS02 (Double) = " << sizeof(dsigma02) << " (Int - D) DS02 = " << (int)dsigma02 << " (Float) FS02 = " << fsigma02 << "     Size of FS02 = " << sizeof(fsigma02) << " (Int - F) FS02 = " << (int)fsigma02 << std::endl;
    std::cout << "(Double) Delta Sigma 03 = " << dsigma03 << "    Size of DS03 (Double) = " << sizeof(dsigma03) << " (Int - D) DS03 = " << (int)dsigma03 << " (Float) FS03 = " << fsigma03 << "    Size of FS03 = " << sizeof(fsigma03) << " (Int - F) FS03 = " << (int)fsigma03 << std::endl;
    std::cout << "(Double) Delta Sigma 04 = " << dsigma04 << "   Size of DS04 (Double) = " << sizeof(dsigma04) << " (Int - D) DS04 = " << (int)dsigma04 << " (Float) FS04 = " << fsigma04 << "   Size of FS04 = " << sizeof(fsigma04) << " (Int - F) FS04 = " << (int)fsigma04 << std::endl;
    std::cout << "(Double) Delta Sigma 05 = " << dsigma05 << "  Size of DS05 (Double) = " << sizeof(dsigma05) << " (Int - D) DS05 = " << (int)dsigma05 << " (Float) FS05 = " << fsigma05 << "  Size of FS05 = " << sizeof(fsigma05) << " (Int - F) FS05 = " << (int)fsigma05 << std::endl;
    std::cout << "(Double) Delta Sigma 06 = " << dsigma06 << " Size of DS06 (Double) = " << sizeof(dsigma06) << " (Int - D) DS06 = " << (int)dsigma06 << " (Float) FS06 = " << fsigma06 << " Size of FS06 = " << sizeof(fsigma06) << " (Int - F) FS06 = " << (int)fsigma06 << std::endl;
    std::cout << "(Double) Delta Sigma 07 = " << dsigma07 << "        Size of DS07 (Double) = " << sizeof(dsigma07) << " (Int - D) DS07 = " << (int)dsigma07 << " (Float) FS07 = " << fsigma07 << "        Size of FS07 = " << sizeof(fsigma07) << " (Int - F) FS07 = " << (int)fsigma07 << std::endl;
    std::cout << "(Double) Delta Sigma 08 = " << dsigma08 << "        Size of DS08 (Double) = " << sizeof(dsigma08) << " (Int - D) DS08 = " << (int)dsigma08 << " (Float) FS08 = " << fsigma08 << "        Size of FS08 = " << sizeof(fsigma08) << " (Int - F) FS08 = " << (int)fsigma08 << std::endl;
    std::cout << "(Double) Delta Sigma 09 = " << dsigma09 << "        Size of DS09 (Double) = " << sizeof(dsigma09) << " (Int - D) DS09 = " << (int)dsigma09 << " (Float) FS09 = " << fsigma09 << "        Size of FS09 = " << sizeof(fsigma09) << " (Int - F) FS09 = " << (int)fsigma09 << std::endl;
    std::cout << "(Double) Delta Sigma 10 = " << dsigma10 << "        Size of DS10 (Double) = " << sizeof(dsigma10) << " (Int - D) DS10 = " << (int)dsigma10 << " (Float) FS10 = " << fsigma10 << "        Size of FS10 = " << sizeof(fsigma10) << " (Int - F) FS10 = " << (int)fsigma10 << std::endl;
    std::cout << "(Double) Delta Sigma 11 = " << dsigma11 << "        Size of DS11 (Double) = " << sizeof(dsigma11) << " (Int - D) DS11 = " << (int)dsigma11 << " (Float) FS11 = " << fsigma11 << "        Size of FS11 = " << sizeof(fsigma11) << " (Int - F) FS11 = " << (int)fsigma11 << std::endl;
    std::cout << "(Double) Delta Sigma 12 = " << dsigma12 << "        Size of DS12 (Double) = " << sizeof(dsigma12) << " (Int - D) DS12 = " << (int)dsigma12 << " (Float) FS12 = " << fsigma12 << "        Size of FS12 = " << sizeof(fsigma12) << " (Int - F) FS12 = " << (int)fsigma12 << std::endl;
    std::cout << "(Double) Delta Sigma 13 = " << dsigma13 << "        Size of DS13 (Double) = " << sizeof(dsigma13) << " (Int - D) DS13 = " << (int)dsigma13 << " (Float) FS13 = " << fsigma13 << "        Size of FS13 = " << sizeof(fsigma13) << " (Int - F) FS13 = " << (int)fsigma13 << std::endl;
    std::cout << "(Double) Delta Sigma 14 = " << dsigma14 << "        Size of DS14 (Double) = " << sizeof(dsigma14) << " (Int - D) DS14 = " << (int)dsigma14 << " (Float) FS14 = " << fsigma14 << "        Size of FS14 = " << sizeof(fsigma14) << " (Int - F) FS14 = " << (int)fsigma14 << std::endl;
    std::cout << "(Double) Delta Sigma 15 = " << dsigma15 << "        Size of DS15 (Double) = " << sizeof(dsigma15) << " (Int - D) DS15 = " << (int)dsigma15 << " (Float) FS15 = " << fsigma15 << "        Size of FS15 = " << sizeof(fsigma15) << " (Int - F) FS15 = " << (int)fsigma15 << std::endl;
    std::cout << "(Double) Delta Sigma 16 = " << dsigma16 << "        Size of DS16 (Double) = " << sizeof(dsigma16) << " (Int - D) DS16 = " << (int)dsigma16 << " (Float) FS16 = " << fsigma16 << "        Size of FS16 = " << sizeof(fsigma16) << " (Int - F) FS16 = " << (int)fsigma16 << std::endl;
    std::cout << "(Double) Delta Sigma 17 = " << dsigma17 << "        Size of DS17 (Double) = " << sizeof(dsigma17) << " (Int - D) DS17 = " << (int)dsigma17 << " (Float) FS17 = " << fsigma17 << "        Size of FS17 = " << sizeof(fsigma17) << " (Int - F) FS17 = " << (int)fsigma17 << std::endl;
    std::cout << "(Double) Delta Sigma 18 = " << dsigma18 << "        Size of DS18 (Double) = " << sizeof(dsigma18) << " (Int - D) DS18 = " << (int)dsigma18 << " (Float) FS18 = " << fsigma18 << "        Size of FS18 = " << sizeof(fsigma18) << " (Int - F) FS18 = " << (int)fsigma18 << std::endl;
    std::cout << "(Double) Delta Sigma 19 = " << dsigma19 << "        Size of DS19 (Double) = " << sizeof(dsigma19) << " (Int - D) DS19 = " << (int)dsigma19 << " (Float) FS19 = " << fsigma19 << "        Size of FS19 = " << sizeof(fsigma19) << " (Int - F) FS19 = " << (int)fsigma19 << std::endl;
    std::cout << "(Double) Delta Sigma 20 = " << dsigma20 << "        Size of DS20 (Double) = " << sizeof(dsigma20) << " (Int - D) DS20 = " << (int)dsigma20 << " (Float) FS20 = " << fsigma20 << "        Size of FS20 = " << sizeof(fsigma20) << " (Int - F) FS20 = " << (int)fsigma20 << std::endl;

}