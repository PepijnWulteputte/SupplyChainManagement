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


SimulatedAnnealing::SimulatedAnnealing() {
}

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


normalHeuristic::normalHeuristic() {

}

int normalHeuristic::runNH(Model m) {
    int bestCost = 1000000;
    for (int i = 0; i < 100; ++i) {
        Input input;

        int cost = calculateCost(m, input);
        if (cost < bestCost) {
            bestSolution = input;
            bestCost = cost;
        }
    }
    return 0;
}

int normalHeuristic::calculateCost(Model m, Input i) {
    int totalItems[numItems][numShops] = {0};
    for (int j = 0; j < numItems; ++j) {
        for (int k = 0; k < numShops; ++k) {
            for (int p = 0; p < numPacks; ++p) {
                if (i.packContent[p][j] != 0) {
                    totalItems[j][k] += i.packContent[p][j] * i.packAllocation[p][k];
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
    float cost = 0;
    for (int l = 0; l < numItems; ++l) {
        for (int j = 0; j < numShops; ++j) {
            if (difference[l][j] > 0) {
                cost += difference[l][j] * overStockCost[l];
            }
            if (difference[l][j] < 0) {
                cost += -difference[l][j] * underStockCost[l];
            }
        }
    }

    for (auto &j: i.packContent) {
        for (int k: j) {
            if (k > 0) {
                cost += creationCost;
                break;
            }
        }
    }
    for (auto &j: i.packAllocation) {
        for (int k: j) {
            cost += k * handlingCost;
        }
    }
    return cost;
}

int normalHeuristic::giveWinner(Model m, Input i) {
    int cost = calculateCost(m, i);
    printf("\nBest solution: %d ", cost);
    printf("Overstock: %d ", getOverUnderStock(m, i).first);
    printf("Understock: %d ", -getOverUnderStock(m, i).second);
    return 0;
}

std::pair<int, int> normalHeuristic::getOverUnderStock(Model m, Input i) {
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

