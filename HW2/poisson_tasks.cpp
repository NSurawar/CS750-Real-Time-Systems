#include <iostream>
#include <random>
#include <cmath>

// Function to generate inter-arrival times using exponential distribution
double generateInterArrivalTime(double lambda, std::mt19937& gen) {
    std::exponential_distribution<double> expDist(lambda);
    return expDist(gen);
}

int main() {
    const double arrival_rate = 0.05; // Arrival rate (lambda)
    std::mt19937 gen(std::random_device{}()); // Mersenne Twister engine

    // Initialize time and generate arrival times
    int current_time = 0;
    for (int i = 0; i < 1000; ++i) { // Generate 10 arrival times for example
        double inter_arrival_time = generateInterArrivalTime(arrival_rate, gen);
        current_time += std::ceil(inter_arrival_time); // Using ceil to keep arrival times as integers
        std::cout << "3," << current_time << std::endl;
    }

    return 0;
}