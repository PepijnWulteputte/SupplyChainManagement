#include <iostream>
#include <fstream>

#include "Model.hpp"

int main() {


    int demand[numItems][numShops] = {      //Gewoon zelf invullen joh
            {17, 16, 15, 39, 5, 10, 4, 9, 8, 8, 11},
            {21, 20, 19, 49, 7, 13, 6, 12, 10, 10, 14},
            {47, 45, 43, 109, 15, 29, 12, 26, 21, 22, 31},
            {49, 46, 44, 113, 16, 30, 13, 27, 22, 22, 32},
            {19, 19, 18, 45, 6, 12, 5, 11, 9, 9, 13},
            {16, 15, 14, 37, 5, 10, 4, 9, 7, 7, 11},
            {26, 24, 23, 45, 6, 16, 7, 14, 11, 12, 17},
            {32, 31, 29, 56, 8, 20, 8, 18, 14, 15, 21},
            {72, 68, 65, 125, 17, 44, 18, 39, 32, 32, 46},
            {75, 71, 68, 130, 18, 45, 19, 41, 33, 33, 48},
            {30, 28, 27, 52, 7, 18, 8, 16, 13, 13, 19},
            {24, 23, 22, 43, 6, 15, 6, 13, 11, 11, 16},
            {33, 31, 30, 53, 7, 20, 8, 18, 15, 15, 21},
            {41, 39, 37, 67, 9, 25, 11, 23, 18, 19, 27},
            {91, 87, 83, 149, 21, 56, 23, 50, 41, 41, 59},
            {95, 90, 86, 154, 21, 58, 24, 52, 42, 43, 62},
            {38, 36, 34, 62, 9, 23, 10, 21, 17, 17, 25},
            {31, 30, 28, 51, 7, 19, 8, 17, 14, 14, 20},
            {6, 5, 5, 11, 2, 3, 1, 3, 2, 2, 3},
            {7, 7, 7, 14, 2, 4, 2, 4, 3, 3, 4},
            {16, 15, 15, 31, 4, 9, 4, 8, 6, 6, 9},
            {17, 16, 15, 33, 5, 9, 4, 8, 7, 7, 10},
            {7, 6, 6, 13, 2, 4, 1, 3, 3, 3, 4},
            {5, 5, 5, 11, 2, 3, 1, 3, 2, 2, 3},
            {37, 35, 33, 62, 9, 21, 9, 18, 15, 15, 22},
            {48, 45, 43, 81, 11, 27, 11, 24, 20, 20, 28},
            {49, 46, 44, 83, 11, 28, 11, 24, 20, 20, 29},
            {17, 16, 15, 28, 4, 9, 4, 8, 7, 7, 10},
            {12, 12, 11, 21, 3, 7, 3, 6, 5, 5, 7},
            {14, 13, 12, 23, 3, 8, 3, 7, 6, 6, 8},
            {11, 11, 10, 25, 3, 6, 3, 6, 5, 5, 7},
            {15, 14, 13, 33, 4, 8, 4, 7, 6, 6, 9},
            {15, 14, 13, 33, 4, 9, 4, 8, 6, 6, 9},
            {5, 5, 5, 11, 2, 3, 1, 3, 2, 2, 3},
            {4, 4, 3, 8, 1, 2, 1, 2, 2, 2, 2},
            {4, 4, 4, 9, 1, 2, 1, 2, 2, 2, 2},
            {39, 37, 35, 89, 12, 29, 12, 26, 21, 22, 31},
            {50, 48, 46, 115, 16, 38, 16, 34, 28, 28, 40},
            {52, 49, 47, 118, 16, 39, 16, 35, 28, 29, 41},
            {17, 17, 16, 40, 5, 13, 5, 12, 10, 10, 14},
            {13, 13, 12, 30, 4, 10, 4, 9, 7, 7, 11},
            {14, 14, 13, 33, 4, 11, 4, 10, 8, 8, 11},
            {10, 9, 9, 23, 3, 4, 2, 4, 3, 3, 4},
            {13, 12, 11, 30, 4, 5, 2, 5, 4, 4, 5},
            {13, 12, 12, 31, 4, 5, 2, 5, 4, 4, 6},
            {4, 4, 4, 11, 1, 2, 1, 2, 1, 1, 2},
            {3, 3, 3, 8, 1, 1, 1, 1, 1, 1, 1},
            {4, 3, 3, 9, 1, 1, 1, 1, 1, 1, 2},
            {52, 50, 48, 111, 15, 32, 13, 29, 23, 24, 34}};


    Model m;

    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            m.demand[i][j] = demand[i][j];
        }
    }
/*
    GeneticAlgorithm geneticAlgorithm;
    geneticAlgorithm.initialisePopulation(m);

    geneticAlgorithm.runGA(m);

    geneticAlgorithm.giveWinners(m);


    SimulatedAnnealing simulatedAnnealing;
    simulatedAnnealing.runSA(m);
*/
    PepienoHeuristic pepienoHeuristic;
    pepienoHeuristic.runNH(m);
    pepienoHeuristic.giveWinner( pepienoHeuristic.bestSolution);

    std::ofstream outputFile("C:/Users/pepij/OneDrive/Master 2/Semester 2/Supply Chain Management/Group Assignment/Results/PepienoHeuristic.csv");
    for (int i = 0; i < numPacks; ++i) {
        for (int j = 0; j < numItems; ++j) {
            outputFile << pepienoHeuristic.bestSolution.packContent[i][j] << ";";
        }
        outputFile << "\n";
    }
    outputFile << "\n";
    for (int i = 0; i < numPacks; ++i) {
        for (int j = 0; j < numShops; ++j) {
            outputFile << pepienoHeuristic.bestSolution.packAllocation[i][j] << ";";
        }
        outputFile << "\n";
    }
    outputFile.close();
    return 0;
}
