//
// Created by pepij on 09/04/2024.
//

#ifndef Model_hpp
#define Model_hpp

#endif //Model_hpp

#include "vector"

const int numShops = 11;
const int numItems = 49;
const int creationCost = 127;
const float handlingCost = 8.53;
const float overStockCost[numItems] = {73,73,73,73,73,73,73,73,73,73,73,73,28,28,28,28,28,28,97,97,97,97,97,97,73,73,73,73,73,73,83,83,83,83,83,83,28.5,28.5,28.5,28.5,28.5,28.5,97,97,97,97,97,97,2};
const float underStockCost[numItems] = {47,47,47,47,47,47,47,47,47,47,47,47,12,12,12,12,12,12,53,53,53,53,53,53,47,47,47,47,47,47,67,67,67,67,67,67,11.5,11.5,11.5,11.5,11.5,11.5,53,53,53,53,53,53,5.99};
const int numPacks = 100; //Arbitrary number, should be plenty tbh
const int maxItems = 20; //Maximum number of items in a pack, lets say a normal person can lift a maximum of 20 items (+-10kg total)

const int populationSize = 100;
const int numberGenerations = 100;
const float mutationChance = 0.05;
const int mutationAmount = 3+1; //+1 because of the random number generator

class Model {
public:
    int demand[numItems][numShops]{};

    explicit Model();
};

struct Input {
    Input();
    int packContent[numPacks][numItems];
    int packAllocation[numPacks][numShops];
};

struct GeneticAlgorithm {

    GeneticAlgorithm();

    int initialisePopulation(Model m);
    int evaluatePopulation(Model m);
    int generateChildren(Model m);
    int selectPopulation();
    std::pair<int,int> getOverUnderStock(Model m, Input i);
    int giveWinners(Model m);
    std::vector<int> getLowestThreeIndices();


    static int calculateCost(Model m, Input i);

    int runGA(Model m);

    std::vector<Input> population;
    std::vector<Input> children;
    int fitnessScores[populationSize * 2]{};
};

struct SimulatedAnnealing {
    SimulatedAnnealing();

    int runSA(Model m);

    int initialisePopulation(Model m);
    int generateNeighbour(Model m, Input i);
    int acceptNeighbour(Model m, Input i, Input neighbour);
    int calculateCost(Model m, Input i);
    int giveWinner(Model m, Input i);
    std::pair<int,int> getOverUnderStock(Model m, Input i);

    Input neighbour;
    Input bestSolution;
    double temperature = 100;
    double coolingRate = 0.005;
};

struct PepienoHeuristic{

    PepienoHeuristic();
    int runNH(Model m);
    float calculateCost(Input i);
    int calculateDifference(Model m, Input i);
    int giveWinner(Model m, Input i);
    std::pair<int,int> getOverUnderStock(Model m, Input i);

    int difference[numItems][numShops]{}; //Difference between demand and supply
    Input bestSolution;
};
