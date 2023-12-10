#include <random>
#include <cassert>
#include <deque>
#include <map>
#include <set>
#include <unordered_set>
#include "../algos/Algos.h"

uint64_t datasetLength;
std::pair<TUPLES*, TIMESTAMP*> dataset;

double calcLowerBound(double alpha, double T, double n, double m, double d, double V) {
    double T0 = d * m / V;
    return T0 / d / 3;
}

double calcUpperBound(double alpha, double T, double n, double m, double d, double V) {
    double T0 = d * m / V;
    
    double zeta = 0;
    for (int i = 1; i <= n; i++)
        zeta += 1. / pow(i, alpha);

    double E = T0 / d / 3.;
    for (int k = 1; k <= n; k++) {
        // zipf 1.0
        double w_k = std::min(k * log(1 + n / k) - 0.5, T / 2);

        // zipf 1.5
        if (alpha != 1.0)
            w_k = std::min(k * 2.42 - 0.5, T / 2);

        double a = pow(k, -alpha) / zeta;

        double e = exp(-w_k / m);
        double b = T0 / d * (1 - e) * 5 / 12;
        double c = T / 2 * pow(1 - e, d);

        E += a * (b + c);
    }

    return E;
}

void testAAE(int query_freq, int window_size, int memory, int counterNum, int counterSize) {
    RingSketch *sketch = new RingSketch(window_size, memory, counterNum, counterSize);

    std::deque<std::pair<int, TUPLES>> lastTime;
    uint64_t c = datasetLength;

    double aae = 0;
    double num = 0;

    double mx = 0;
    for (int i = 0; i < c; i++) {
        TUPLES tuple = dataset.first[i];

        if (i > 0 && i % query_freq == 0) {
            while (i - lastTime.front().first > window_size)
                lastTime.pop_front();

            auto random_it = std::next(std::begin(lastTime), rand() % lastTime.size());

            int gt = i - random_it->first;

            for (auto iter = lastTime.rbegin(); iter != lastTime.rend(); ++iter)
                if (iter->second == random_it->second) {
                    gt = i - iter->first;
                    break;
                }

            int est = sketch->estimate(random_it->second, 0);
            int dt = std::abs(gt - est);

            aae += (double) dt;
            num += 1;
        }

        sketch->insert(tuple, 0);
        lastTime.emplace_back(i, tuple);
        sketch->updateClock(1);
    }

    printf("%s,%d,%d,%d,%.10f,", sketch->name.c_str(), memory, counterNum, counterSize, aae / num);
    printf("%.10f,", calcLowerBound(1.0, window_size, window_size, sketch->counterInterval, sketch->counterNum, sketch->updateSpeed));
    printf("%.10f\n", calcUpperBound(1.0, window_size, window_size, sketch->counterInterval, sketch->counterNum, sketch->updateSpeed));
}

std::string folder = "../dataset/";

std::string file[1] = {"zipf_1.0.dat"};

int main(int argc,char* argv[]) {
    srand(233);

    dataset = read_data_zipf((folder + file[0]).c_str(), 1 << 22, &datasetLength);

    int window_size = 1 << 13;

    freopen("result_math.csv", "w", stdout);
    printf("name,memory,counterNum,counterSize,aae,theoretical (collision-free),upperbound\n");
    for (int i = 1; i <= 5; i++)
        testAAE(1, window_size, 20000 + 5000 * i, 6, 8);

    return 0;
}
