//
// Created by pepieno F Wulteputte on 09/04/2024.
//
#include "Model.hpp"
#include <random>
#include <algorithm>

Model::Model() {
    for (int i = 0; i < numShops; ++i) {
        for (auto &j: demand) {
            j[i] = 0;
        }
    }
}

GeneticAlgorithm::GeneticAlgorithm() {
    for (int &fitnessScore: fitnessScores) {
        fitnessScore = 0;
    }
}

int GeneticAlgorithm::initialisePopulation(Model model) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> randInt(0, 2);
    std::uniform_int_distribution<int> packRNG(40, 50);

    for (int p = 0; p < populationSize; ++p) {
        int packAmount = packRNG(rng);
        Input input;
        for (int g = 0; g < packAmount; ++g) {

            bool Stop[numItems][numShops];
            int totalItems[numItems][numShops] = {0};
            for (int i = 0; i < numItems; ++i) {
                for (int j = 0; j < numShops; ++j) {
                    for (int q = 0; q < numPacks; ++q) {
                        if (input.packContent[q][i] != 0) {
                            totalItems[i][j] += input.packContent[q][i] * input.packAllocation[q][j];
                        }
                    }
                }
            }

            int difference[numItems][numShops] = {0};
            for (int i = 0; i < numItems; ++i) {
                for (int j = 0; j < numShops; ++j) {
                    difference[i][j] = model.demand[i][j] - totalItems[i][j];
                    if (difference[i][j] < 0) {
                        Stop[i][j] = true;
                    }
                }
            }
            for (int j = 0; j < numItems; ++j) {
                bool enough = false;
                for (int i = 0; i < numShops; ++i) {
                    if (Stop[j][i]) {
                        enough = true;
                    }
                }
                //if (!enough){
                input.packContent[g][j] = randInt(rng);
                //}
            }

            for (int j = 0; j < numShops; ++j) {
                bool enough = false;
                for (auto &i: Stop) {
                    if (i[j]) {
                        enough = true;
                    }
                }
                //if (!enough){
                input.packAllocation[g][j] = randInt(rng);
                //}
            }

        }
        int totalItems[numItems][numShops] = {0};
        for (int i = 0; i < numItems; ++i) {
            for (int j = 0; j < numShops; ++j) {
                for (int q = 0; q < numPacks; ++q) {
                    if (input.packContent[q][i] != 0) {
                        totalItems[i][j] += input.packContent[q][i] * input.packAllocation[q][j];
                    }
                }
            }
        }

        int difference[numItems][numShops] = {0};
        for (int i = 0; i < numItems; ++i) {
            for (int j = 0; j < numShops; ++j) {
                difference[i][j] = totalItems[i][j] - model.demand[i][j];
            }
        }

        //Clean up Overstocking and Understocking for stores
        for (int i = 0; i < numShops; ++i) {
            int min = 100;
            for (auto &j: difference) {
                if (j[i] < min) {
                    min = j[i];
                }
            }
            for (int j = 0; j < min; ++j) {
                input.packAllocation[j][i]--;
            }
            for (int j = 0; j < -min; ++j) {
                input.packAllocation[j][i]++;
            }
        }
        for (int i = 0; i < numItems; ++i) {
            int min = 100;
            for (int j = 0; j < numShops; ++j) {
                if (difference[i][j] < min) {
                    min = difference[i][j];
                }
            }
            for (int j = 0; j < min; ++j) {
                input.packContent[j][i]--;
            }
            for (int j = 0; j < -min; ++j) {
                input.packContent[j][i]++;
            }
        }
        population.push_back(input);
    }
    return 0;
}

int GeneticAlgorithm::runGA(Model m) {
    for (int i = 0; i < numberGenerations; ++i) {
        evaluatePopulation(m);
        generateChildren(m);
        for (int j = 0; j < populationSize; ++j) {
            fitnessScores[j + populationSize] = calculateCost(m, children[j]);
        }
        selectPopulation();
        printf("\nGeneration %d complete", i);
    }

    return 0;
}

int GeneticAlgorithm::calculateCost(Model model, Input input) {
    int totalItems[numItems][numShops] = {0};
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            for (int p = 0; p < numPacks; ++p) {
                if (input.packContent[p][i] != 0) {
                    totalItems[i][j] += input.packContent[p][i] * input.packAllocation[p][j];
                }
            }
        }
    }
    int difference[numItems][numShops] = {0};
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            difference[i][j] = model.demand[i][j] - totalItems[i][j];
        }
    }
    float cost = 0;
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            if (difference[i][j] > 0) {
                cost += difference[i][j] * overStockCost[i];
            }
            if (difference[i][j] < 0) {
                cost += -difference[i][j] * underStockCost[i];
            }

        }
    }

    for (auto &i: input.packContent) {
        for (int j: i) {
            if (j > 0) {
                cost += creationCost;
                break;
            }
        }
    }
    for (auto &i: input.packAllocation) {
        for (int j: i) {
            cost += j * handlingCost;
        }
    }
    return cost;
}

int GeneticAlgorithm::generateChildren(Model model) {
    std::vector<std::pair<int, int>> pairs;
    int numPairs = populationSize / 2;
    std::mt19937 rng(std::random_device{}());

    // Initialize a vector to store all unique numbers
    std::vector<int> uniqueNumbers;
    for (int i = 0; i < populationSize; ++i) {
        uniqueNumbers.push_back(i);
    }

    // Shuffle the unique numbers
    std::shuffle(uniqueNumbers.begin(), uniqueNumbers.end(), rng);

    // Generate pairs
    for (int i = 0; i < numPairs; ++i) {
        pairs.emplace_back(uniqueNumbers[i], uniqueNumbers[i + numPairs]);
    }
    for (int i = 0; i < numPairs; ++i) {
        std::uniform_int_distribution<int> dist(1, numPacks / 2 - 1);
        int splitContent = dist(rng);
        int splitAllocation = dist(rng);

        Input son = population[pairs[i].first];
        Input daughter = population[pairs[i].second];

        for (int j = 0; j < splitContent; ++j) {
            for (int l = 0; l < numItems; ++l) {
                son.packContent[j][l] = population[pairs[i].first].packContent[j][l];
                daughter.packContent[j][l] = population[pairs[i].second].packContent[j][l];
            }
        }
        for (int j = splitContent; j < numPacks; ++j) {
            for (int l = 0; l < numItems; ++l) {
                son.packContent[j][l] = population[pairs[i].second].packContent[j][l];
                daughter.packContent[j][l] = population[pairs[i].first].packContent[j][l];
            }
        }

        for (int j = 0; j < splitAllocation; ++j) {
            for (int l = 0; l < numShops; ++l) {
                son.packAllocation[j][l] = population[pairs[i].first].packAllocation[j][l];
                daughter.packAllocation[j][l] = population[pairs[i].second].packAllocation[j][l];
            }
        }
        for (int j = splitAllocation; j < numPacks; ++j) {
            for (int l = 0; l < numShops; ++l) {
                son.packAllocation[j][l] = population[pairs[i].second].packAllocation[j][l];
                daughter.packAllocation[j][l] = population[pairs[i].first].packAllocation[j][l];
            }
        }
        //Mutation
        std::uniform_real_distribution<float> mutation(0, 1);
        for (auto &j: son.packContent) {
            if (mutation(rng) < mutationChance / 2) {
                for (int &l: j) {
                    l = 0;
                }
            }
            if (mutation(rng) < mutationChance || mutation(rng) > mutationChance / 2) {
                for (int &l: j) {
                    l = rand() % mutationAmount;
                }
            }
        }
        for (auto &j: daughter.packContent) {
            if (mutation(rng) < mutationChance / 2) {
                for (int &l: j) {
                    l = 0;
                }
            }
            if (mutation(rng) < mutationChance || mutation(rng) > mutationChance / 2) {
                for (int &l: j) {
                    l = rand() % mutationAmount;
                }
            }
        }
        for (auto &j: son.packAllocation) {
            if (mutation(rng) < mutationChance / 2) {
                for (int &l: j) {
                    l = 0;
                }
            }
            if (mutation(rng) < mutationChance || mutation(rng) > mutationChance / 2) {
                for (int &l: j) {
                    l = rand() % mutationAmount;
                }
            }
        }
        for (auto &j: daughter.packAllocation) {
            if (mutation(rng) < mutationChance / 2) {
                for (int &l: j) {
                    l = 0;
                }
            }
            if (mutation(rng) < mutationChance || mutation(rng) > mutationChance / 2) {
                for (int &l: j) {
                    l = rand() % mutationAmount;
                }
            }
        }

        int totalItems[numItems][numShops] = {0};
        for (int w = 0; w < numItems; ++w) {
            for (int j = 0; j < numShops; ++j) {
                for (int q = 0; q < numPacks; ++q) {
                    if (son.packContent[q][w] != 0) {
                        totalItems[w][j] += son.packContent[q][w] * son.packAllocation[q][j];
                    }
                }
            }
        }

        int difference[numItems][numShops] = {0};
        for (int w = 0; w < numItems; ++w) {
            for (int j = 0; j < numShops; ++j) {
                difference[w][j] = totalItems[w][j] - model.demand[w][j];
            }
        }

        //Clean up Overstocking and Understocking for stores
        for (int w = 0; w < numShops; ++w) {
            int min = 100;
            for (auto &j: difference) {
                if (j[w] < min) {
                    min = j[w];
                }
            }
            for (int j = 0; j < min; ++j) {
                son.packAllocation[j][w]--;
            }
            for (int j = 0; j < -min; ++j) {
                son.packAllocation[j][w]++;
            }
        }

        int totalItems2[numItems][numShops] = {0};
        for (int i = 0; i < numItems; ++i) {
            for (int j = 0; j < numShops; ++j) {
                for (int q = 0; q < numPacks; ++q) {
                    if (daughter.packContent[q][i] != 0) {
                        totalItems2[i][j] += daughter.packContent[q][i] * daughter.packAllocation[q][j];
                    }
                }
            }
        }

        difference[numItems][numShops] = {0};
        for (int i = 0; i < numItems; ++i) {
            for (int j = 0; j < numShops; ++j) {
                difference[i][j] = totalItems2[i][j] - model.demand[i][j];
            }
        }

        //Clean up Overstocking and Understocking for stores
        for (int i = 0; i < numShops; ++i) {
            int min = 100;
            for (auto &j: difference) {
                if (j[i] < min) {
                    min = j[i];
                }
            }
            for (int j = 0; j < min; ++j) {
                daughter.packAllocation[j][i]--;
            }
            for (int j = 0; j < -min; ++j) {
                daughter.packAllocation[j][i]++;
            }
        }


        children.push_back(son);
        children.push_back(daughter);
    }
    return 0;
}

int GeneticAlgorithm::evaluatePopulation(Model m) {
    for (int i = 0; i < populationSize; ++i) {
        fitnessScores[i] = calculateCost(m, population[i]);
    }
    return 0;
}

int GeneticAlgorithm::selectPopulation() {
    int numScores = populationSize * 2;
    std::vector<int> indices(numScores);
    for (int i = 0; i < numScores; ++i) {
        indices[i] = i;
    }

    // Sort the indices based on the scores they point to
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return fitnessScores[a] < fitnessScores[b];
    });

    for (int i = 0; i < populationSize; ++i) {
        if (indices[i] < populationSize) { //Smaller than popSize means it's from an OG parent
            population[i] = population[indices[i]];
        }
        if (indices[i] >= populationSize) {   //Larger or equal means it's from new child
            population[i] = children[indices[i] - populationSize]; //Subtract popSize to get the correct index
        }
    }

    return 0;
}

int GeneticAlgorithm::giveWinners(Model m) {
    evaluatePopulation(m);
    std::vector<int> indicesBest = getLowestThreeIndices(); //Indices of the best 3 solutions

    Input bestInput = population[indicesBest.at(0)];   //Best solution

    printf("\nBest solution: %d ", calculateCost(m, bestInput));
    printf("Overstock: %d ", getOverUnderStock(m, bestInput).first);
    printf("Understock: %d ", -getOverUnderStock(m, bestInput).second);

    return 0;
}

std::vector<int> GeneticAlgorithm::getLowestThreeIndices() {
    int numScores = populationSize * 2;
    std::vector<int> indices(numScores);
    for (int i = 0; i < numScores; ++i) {
        indices[i] = i;
    }

    // Sort the indices based on the scores they point to
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return fitnessScores[a] < fitnessScores[b];
    });

    // Take the first three indices
    std::vector<int> lowestThreeIndices(indices.begin(), indices.begin() + 3);
    return lowestThreeIndices;
}

std::pair<int, int> GeneticAlgorithm::getOverUnderStock(Model m, Input i) {
    int totalItems[numItems][numShops] = {0};
    for (int j = 0; j < numItems; ++j) {
        for (int k = 0; k < numShops; ++k) {
            for (int l = 0; l < numPacks; ++l) {
                if (i.packContent[l][j] != 0) {
                    totalItems[j][k] += i.packContent[l][j] * i.packAllocation[l][k];
                }
            }
        }
    }
    int difference[numItems][numShops] = {0};
    for (int j = 0; j < numItems; ++j) {
        for (int k = 0; k < numShops; ++k) {
            difference[j][k] = m.demand[j][k] - totalItems[j][k];
        }
    }
    int OverStock = 0;
    int UnderStock = 0;
    for (auto &j: difference) {
        for (int k: j) {
            if (k > 0) {
                UnderStock += k;
            }
            if (k < 0) {
                OverStock += -k;
            }
        }
    }
    return std::make_pair(OverStock, UnderStock);
}


Input::Input() {
    for (auto &i: packContent) {
        for (int &j: i) {
            j = 0;
        }
    }
    for (auto &i: packAllocation) {
        for (int &j: i) {
            j = 0;
        }
    }
}


SimulatedAnnealing::SimulatedAnnealing() = default;

int SimulatedAnnealing::runSA(Model m) {
    initialisePopulation(m);
    int i = 0;
    do {
        generateNeighbour(m, bestSolution);
        acceptNeighbour(m, bestSolution, neighbour);
        printf("\nGeneration %d complete", i);
        i++;
    } while (temperature > 0.1);
    giveWinner(m, bestSolution);
    return 0;
}

int SimulatedAnnealing::initialisePopulation(Model model) {
    bestSolution = Input();
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> randInt(0, 2);
    std::uniform_int_distribution<int> packRNG(40, 50);

    int packAmount = packRNG(rng);
    Input input;
    for (int g = 0; g < packAmount; ++g) {

        bool Stop[numItems][numShops];
        int totalItems[numItems][numShops] = {0};
        for (int i = 0; i < numItems; ++i) {
            for (int j = 0; j < numShops; ++j) {
                for (int q = 0; q < numPacks; ++q) {
                    if (input.packContent[q][i] != 0) {
                        totalItems[i][j] += input.packContent[q][i] * input.packAllocation[q][j];
                    }
                }
            }
        }

        int difference[numItems][numShops] = {0};
        for (int i = 0; i < numItems; ++i) {
            for (int j = 0; j < numShops; ++j) {
                difference[i][j] = model.demand[i][j] - totalItems[i][j];
                if (difference[i][j] < 0) {
                    Stop[i][j] = true;
                }
            }
        }
        for (int j = 0; j < numItems; ++j) {
            bool enough = false;
            for (int i = 0; i < numShops; ++i) {
                if (Stop[j][i]) {
                    enough = true;
                }
            }
            //if (!enough){
            input.packContent[g][j] = randInt(rng);
            //}
        }

        for (int j = 0; j < numShops; ++j) {
            bool enough = false;
            for (auto &i: Stop) {
                if (i[j]) {
                    enough = true;
                }
            }
            //if (!enough){
            input.packAllocation[g][j] = randInt(rng);
            //}
        }

    }
    int totalItems[numItems][numShops] = {0};
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            for (int q = 0; q < numPacks; ++q) {
                if (input.packContent[q][i] != 0) {
                    totalItems[i][j] += input.packContent[q][i] * input.packAllocation[q][j];
                }
            }
        }
    }

    int difference[numItems][numShops] = {0};
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            difference[i][j] = totalItems[i][j] - model.demand[i][j];
        }
    }

    //Clean up Overstocking and Understocking for stores
    for (int i = 0; i < numShops; ++i) {
        int min = 100;
        for (auto &j: difference) {
            if (j[i] < min) {
                min = j[i];
            }
        }
        for (int j = 0; j < min; ++j) {
            input.packAllocation[j][i]--;
        }
        for (int j = 0; j < -min; ++j) {
            input.packAllocation[j][i]++;
        }
    }
    for (int i = 0; i < numItems; ++i) {
        int min = 100;
        for (int j = 0; j < numShops; ++j) {
            if (difference[i][j] < min) {
                min = difference[i][j];
            }
        }
        for (int j = 0; j < min; ++j) {
            input.packContent[j][i]--;
        }
        for (int j = 0; j < -min; ++j) {
            input.packContent[j][i]++;
        }
    }
    bestSolution = input;
    return 0;
}

int SimulatedAnnealing::generateNeighbour(Model m, Input i) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> randInt(1, 3);
    std::uniform_int_distribution<int> randShops(0, numShops);
    std::uniform_int_distribution<int> randItems(0, numItems);
    std::uniform_int_distribution<int> randPacks(0, 50);

    neighbour = i;
    int randGain = randInt(rng);
    int randLoss = randInt(rng);
    for (int j = 0; j < randGain; ++j) {
        int randPack = randPacks(rng);
        int randItem = randItems(rng);
        neighbour.packContent[randPack][randItem]++;
    }
    for (int j = 0; j < randLoss; ++j) {
        int randPack = randPacks(rng);
        int randItem = randItems(rng);
        if (neighbour.packContent[randPack][randItem] > 0)
            neighbour.packContent[randPack][randItem]--;
    }
    for (int j = 0; j < randGain; ++j) {
        int randPack = randPacks(rng);
        int randShop = randShops(rng);
        neighbour.packAllocation[randPack][randShop]++;
    }
    for (int j = 0; j < randLoss; ++j) {
        int randPack = randPacks(rng);
        int randShop = randShops(rng);
        if (neighbour.packAllocation[randPack][randShop] > 0)
            neighbour.packAllocation[randPack][randShop]--;
    }

    return 0;
}

int SimulatedAnnealing::calculateCost(Model model, Input input) {
    int totalItems[numItems][numShops] = {0};
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            for (int p = 0; p < numPacks; ++p) {
                if (input.packContent[p][i] != 0) {
                    totalItems[i][j] += input.packContent[p][i] * input.packAllocation[p][j];
                }
            }
        }
    }
    int difference[numItems][numShops] = {0};
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            difference[i][j] = model.demand[i][j] - totalItems[i][j];
        }
    }
    float cost = 0;
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            if (difference[i][j] > 0) {
                cost += difference[i][j] * overStockCost[i];
            }
            if (difference[i][j] < 0) {
                cost += -difference[i][j] * underStockCost[i];
            }

        }
    }

    for (auto &i: input.packContent) {
        for (int j: i) {
            if (j > 0) {
                cost += creationCost;
                break;
            }
        }
    }
    for (auto &i: input.packAllocation) {
        for (int j: i) {
            cost += j * handlingCost;
        }
    }
    return cost;
}

int SimulatedAnnealing::acceptNeighbour(Model m, Input i, Input neighbour) {
    int cost = calculateCost(m, i);
    int neighbourCost = calculateCost(m, neighbour);

    if (neighbourCost < cost) {
        i = neighbour;
    } else {
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(0, 100);
        if (dist(rng) < temperature) {
            i = neighbour;
        }
    }
    temperature *= (1 - coolingRate);
    bestSolution = i;

    return 0;
}

int SimulatedAnnealing::giveWinner(Model m, Input i) {
    int cost = calculateCost(m, i);
    printf("\nBest solution: %d ", cost);
    printf("Overstock: %d ", getOverUnderStock(m, i).first);
    printf("Understock: %d ", -getOverUnderStock(m, i).second);
    return 0;
}

std::pair<int, int> SimulatedAnnealing::getOverUnderStock(Model m, Input i) {
    int totalItems[numItems][numShops] = {0};
    for (int j = 0; j < numItems; ++j) {
        for (int k = 0; k < numShops; ++k) {
            for (int l = 0; l < numPacks; ++l) {
                if (i.packContent[l][j] != 0) {
                    totalItems[j][k] += i.packContent[l][j] * i.packAllocation[l][k];
                }
            }
        }
    }
    int difference[numItems][numShops] = {0};
    for (int j = 0; j < numItems; ++j) {
        for (int k = 0; k < numShops; ++k) {
            difference[j][k] = m.demand[j][k] - totalItems[j][k];
        }
    }
    int OverStock = 0;
    int UnderStock = 0;
    for (auto &j: difference) {
        for (int k: j) {
            if (k > 0) {
                UnderStock += k;
            }
            if (k < 0) {
                OverStock += -k;
            }
        }
    }
    return std::make_pair(OverStock, UnderStock);
}


PepienoHeuristic::PepienoHeuristic() {
    for (auto &i: difference) {
        for (int &j: i) {
            j = 0;
        }
    }
    bestSolution = Input();
}

int PepienoHeuristic::runNH(Model m) {
    int packCounter = -1;    //Counter for the packs
    int freeSpacePacks[numPacks];   //Array to store the free space in each pack
    for (int &i: freeSpacePacks) { //Initialize the array
        i = maxItems;  //Max items is the maximum amount of items that can be stored in a pack
    }


    Input input;
    calculateDifference(m, input); //Calculate the difference between demand and supply

    //Pepieno algorithm
    //Initialize the packs
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            do {
                if (difference[i][j] <
                    freeSpacePacks[packCounter]) {   //If the difference is smaller than the free space in the pack
                    input.packContent[packCounter][i] = difference[i][j];   //Fill the pack with the difference
                    freeSpacePacks[packCounter] -= difference[i][j];    //Subtract the difference from the free space
                } else {    //If the difference is larger than the free space in the pack
                    input.packContent[packCounter][i] = freeSpacePacks[packCounter];    //Fill the pack
                    freeSpacePacks[packCounter] = 0;  //Set the free space to 0
                }
                input.packAllocation[packCounter][j]++;   //Allocate the pack to the shop
                for (int k = 0; k < j; ++k) {    //Check if the pack can be allocated to other shops
                    if (difference[i][k] >
                        input.packContent[packCounter][i]) {     //If the difference is larger than the content of the pack
                        input.packAllocation[packCounter][k]++;     //Allocate the pack to the shop
                    }
                }
                for (int k = j + 1; k < numShops; ++k) {    //Check if the pack can be allocated to other shops
                    if (difference[i][k] >
                        input.packContent[packCounter][i]) {     //If the difference is larger than the content of the pack
                        input.packAllocation[packCounter][k]++;     //Allocate the pack to the shop
                    }
                }
                packCounter++;  //Increment the pack counter
                calculateDifference(m, input);  //Recalculate the difference
            } while (difference[i][j] > 0);
        }
    }
    for (int i = 0; i <numItems; ++i) {
        input.packContent[numPacks-1][i]=0;
    }
    //Optimize the solution with same pack content
    for (int i = 0; i < numPacks; i++) {
        bool used = false;
        for (int j = i + 1; j < numPacks; ++j) {
            for (int k = 0; k < numItems; ++k) {
                if (input.packContent[i][k] != input.packContent[j][k]) {//If the content of the packs is not the same
                    goto notSame;
                }
                if (input.packContent[i][k]!=0){
                    used = true;
                }
            }
            //If the contents of the packs are the same
            if (used) {
                for (int l = 0; l < numShops; ++l) {
                    input.packAllocation[i][l] += input.packAllocation[j][l];   //Add the allocation of the pack to the other pack
                    input.packAllocation[j][l] = 0;   //Set the allocation of the other pack to 0
                }
                for (int n = 0; n < numItems; ++n) {
                    input.packContent[j][n] = 0;    //Clear the content of the other pack
                }
                for (int n = j; n < numPacks - 1; ++n) {
                    for (int l = 0; l < numShops; ++l) {
                        input.packAllocation[n][l] = input.packAllocation[n + 1][l];  //Move the allocation a layer up
                    }
                    for (int k = 0; k < numItems; ++k) {
                        input.packContent[n][k] = input.packContent[n + 1][k];  //Move the content a layer up
                    }
                }
                printf("\nPack %d and Pack %d are the same", i, j);
                j--;    //Decrement j to check the pack that was moved up
            }
            notSame:;
        }
    }

    //Optimize the solution with same pack allocation
    for (int i = 0; i < numPacks; i++) {
        bool used = false;
        for (int j = i + 1; j < numPacks; ++j) {
            int content1 = 0;
            int content2 = 0;

            for (int k = 0; k < numShops; ++k) {
                if (input.packAllocation[i][k] != input.packAllocation[j][k]) {//If the allocation of the packs is not the same
                    goto notSame2;
                }
                if (input.packAllocation[i][k]!=0){
                    used = true;
                }
            }
            for (int k = 0; k < numItems; ++k) {
                content1 += input.packContent[i][k];
                content2 += input.packContent[j][k];
            }
            //If the allocations of the packs are the same and there is enough space left in pack 1
            if (used && content1+content2<=maxItems) {
                for (int l = 0; l < numItems; ++l) {
                    input.packContent[i][l] += input.packContent[j][l];   //Add the content of the pack to the other pack
                    input.packContent[j][l] = 0;   //Set the content of the other pack to 0
                }
                for (int n = 0; n < numShops; ++n) {
                    input.packAllocation[j][n] = 0;    //Clear the allocation of the other pack
                }
                for (int n = j; n < numPacks - 1; ++n) {
                    for (int l = 0; l < numItems; ++l) {
                        input.packContent[n][l] = input.packContent[n + 1][l];  //Move the content a layer up
                    }
                    for (int k = 0; k < numShops; ++k) {
                        input.packAllocation[n][k] = input.packAllocation[n + 1][k];  //Move the allocation a layer up
                    }
                }
                printf("\nPack %d and Pack %d are the same", i, j);
                j--;    //Decrement j to check the pack that was moved up
            }
            notSame2:;
        }
    }
    calculateDifference(m, input);  //Recalculate the difference
    bestSolution = input;
    return 0;
}

float PepienoHeuristic::calculateCost(Input i) {

    float cost = 0;
    for (int l = 0; l < numItems; ++l) {
        for (int j = 0; j < numShops; ++j) {
            if (difference[l][j] > 0) {
                cost += difference[l][j] * overStockCost[l];
            }
            if (difference[l][j] < 0) {
                cost += -difference[l][j] *
                        underStockCost[l];  //Negative difference means understock so we multiply by -1
            }
        }
    }

    for (auto &j: i.packContent) {
        for (int k: j) {
            if (k > 0) {
                cost += creationCost;   //Creation cost is added for every pack that has items in it
                break;
            }
        }
    }
    for (auto &j: i.packAllocation) {
        for (int k: j) {
            cost += k * handlingCost;   //Handling cost is added for every pack that is allocated to a shop
        }
    }
    return cost;
}

int PepienoHeuristic::giveWinner(Input i) {
    float cost = calculateCost(i);
    printf("\nBest solution: %f ", cost);
    printf("Overstock: %d ", getOverUnderStock(i).first);
    printf("Understock: %d ", -getOverUnderStock(i).second);
    return 0;
}

std::pair<int, int> PepienoHeuristic::getOverUnderStock(Input i) {
    int OverStock = 0;
    int UnderStock = 0;
    for (auto &j: difference) {
        for (int k: j) {
            if (k > 0) {
                UnderStock += k;
            }
            if (k < 0) {
                OverStock += -k;
            }
        }
    }
    return std::make_pair(OverStock, UnderStock);
}

int PepienoHeuristic::calculateDifference(Model m, Input i) {
    int totalItems[numItems][numShops] = {0};
    for (int j = 0; j < numItems; ++j) {
        for (int k = 0; k < numShops; ++k) {
            for (int l = 0; l < numPacks; ++l) {
                if (i.packContent[l][j] != 0) {
                    totalItems[j][k] += i.packContent[l][j] * i.packAllocation[l][k];
                }
            }
        }
    }

    for (int j = 0; j < numItems; ++j) {
        for (int k = 0; k < numShops; ++k) {
            this->difference[j][k] = m.demand[j][k] - totalItems[j][k];
        }
    }
    return 0;
}

