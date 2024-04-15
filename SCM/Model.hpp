//
// Created by pepij on 09/04/2024.
//

#ifndef Model_hpp
#define Model_hpp

#endif //Model_hpp

#include "vector"

const int numShops = 12;
const int numItems = 49;
const int creationCost = 127;
const double handlingCost = 8.53;
const int overStockCost = 100;
const int underStockCost = 100;

const int populationSize = 50;
const int numberGenerations = 1000;
const float mutationChance = 0.25;
const int numPacks = 100; //Arbitrary number, should be plenty tbh

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

    int initialisePopulation();
    int evaluatePopulation(Model m);
    int generateChildren();
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
