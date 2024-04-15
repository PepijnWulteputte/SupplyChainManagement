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

int GeneticAlgorithm::initialisePopulation() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> randInt(0, 3);
    std::uniform_int_distribution<int> packRNG(20, 100);

    for (int p = 0; p < populationSize; ++p) {
        int packAmount = packRNG(rng);
        Input input;
        for (int i = 0; i < packAmount; ++i) {
            for (int j = 0; j < numItems; ++j) {
                input.packContent[i][j] = randInt(rng);
            }
            for (int j = 0; j < numShops; ++j) {
                input.packAllocation[i][j] = randInt(rng);
            }
        }
        population.push_back(input);
    }
    return 0;
}

int GeneticAlgorithm::runGA(Model m) {
    for (int i = 0; i < numberGenerations; ++i) {
        evaluatePopulation(m);
        generateChildren();
        for (int j = 0; j < populationSize; ++j) {
            fitnessScores[j+populationSize]=calculateCost(m, children[j]);
        }
        selectPopulation();
        printf("\nGeneteration %d complete", i);
    }

    return 0;
}

int GeneticAlgorithm::calculateCost(Model model, Input input) {
    int totalItems[numItems][numShops] = {0};
    for (int i = 0; i < numItems; ++i) {
        for (int j = 0; j < numShops; ++j) {
            for (int p = 0; p < numPacks; ++p) {
                if (input.packContent[p][i]!=0){
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
    int cost = 0;
    for (auto & i : difference) {
        for (int j : i) {
            if (j>0){
                cost += j * overStockCost;
            }
            if (j<0){
                cost += -j * underStockCost;
            }
        }
    }
    for (auto & i : input.packContent) {
        for (int j : i) {
            if (j > 0) {
                cost += creationCost;
                break;
            }
        }
    }
    for (auto & i : input.packAllocation) {
        for (int j : i) {
            cost += j * handlingCost;
        }
    }
    return cost;
}

int GeneticAlgorithm::generateChildren() {
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
        std::uniform_int_distribution<int> dist(1, numPacks/2-1);
        int splitContent = dist(rng);
        int splitAllocation = dist(rng);

        Input son = population[pairs[i].first];
        Input daughter = population[pairs[i].second];

        for (int j = 0; j < splitContent; ++j) {
            for (int l = 0; l < numItems; ++l) {
                son.packContent[j][l]=population[pairs[i].first].packContent[j][l];
                daughter.packContent[j][l]=population[pairs[i].second].packContent[j][l];
            }
        }
        for (int j = splitContent; j < numPacks; ++j) {
            for (int l = 0; l < numItems; ++l) {
                son.packContent[j][l]=population[pairs[i].second].packContent[j][l];
                daughter.packContent[j][l]=population[pairs[i].first].packContent[j][l];
            }
        }

        for (int j = 0; j < splitAllocation; ++j) {
            for (int l = 0; l < numShops; ++l) {
                son.packAllocation[j][l]=population[pairs[i].first].packAllocation[j][l];
                daughter.packAllocation[j][l]=population[pairs[i].second].packAllocation[j][l];
            }
        }
        for (int j = splitAllocation; j < numPacks; ++j) {
            for (int l = 0; l < numShops; ++l) {
                son.packAllocation[j][l]=population[pairs[i].second].packAllocation[j][l];
                daughter.packAllocation[j][l]=population[pairs[i].first].packAllocation[j][l];
            }
        }
        //Mutation
        std::uniform_real_distribution<float> mutation(0, 1);
        for (auto & j : son.packContent) {
            if (mutation(rng) < mutationChance/2) {
                for (int & l : j) {
                    l = 0;
                }
            }
            if (mutation(rng) < mutationChance || mutation(rng) > mutationChance/2) {
                for (int & l : j) {
                    l = rand() % 4;
                }
            }
        }
        for (auto & j : daughter.packContent) {
            if (mutation(rng) < mutationChance/2) {
                for (int & l : j) {
                    l = 0;
                }
            }
            if (mutation(rng) < mutationChance || mutation(rng) > mutationChance/2) {
                for (int & l : j) {
                    l = rand() % 4;
                }
            }
        }
        for (auto & j : son.packAllocation) {
            if (mutation(rng) < mutationChance/2) {
                for (int & l : j) {
                    l = 0;
                }
            }
            if (mutation(rng) < mutationChance || mutation(rng) > mutationChance/2) {
                for (int & l : j) {
                    l = rand() % 4;
                }
            }
        }
        for (auto & j : daughter.packAllocation) {
            if (mutation(rng) < mutationChance/2) {
                for (int & l : j) {
                    l = 0;
                }
            }
            if (mutation(rng) < mutationChance || mutation(rng) > mutationChance/2) {
                for (int & l : j) {
                    l = rand() % 4;
                }
            }
        }

        children.push_back(son);
        children.push_back(daughter);
    }
    return 0;
}

int GeneticAlgorithm::evaluatePopulation(Model m) {
    for (int i = 0; i < populationSize; ++i) {
        fitnessScores[i]=calculateCost(m, population[i]);
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

std::pair<int,int> GeneticAlgorithm::getOverUnderStock(Model m, Input i) {
    int totalItems[numItems][numShops] = {0};
    for (int j = 0; j < numItems; ++j) {
        for (int k = 0; k < numShops; ++k) {
            for (int l = 0; l < numPacks; ++l) {
                if (i.packContent[l][j]!=0){
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
    for (auto & j : difference) {
        for (int k : j) {
            if (k>0){
                OverStock += k;
            }
            if (k<0){
                UnderStock += -k;
            }
        }
    }
    return std::make_pair(OverStock, UnderStock);
}


Input::Input(){
    for (auto & i : packContent) {
        for (int & j : i) {
            j = 0;
        }
    }
    for (auto & i : packAllocation) {
        for (int & j : i) {
            j = 0;
        }
    }
};
