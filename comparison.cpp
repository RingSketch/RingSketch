#include <random>
#include <cassert>
#include <deque>
#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <vector>
#include <mutex>
#include <sstream>
#include "Algos.h"

typedef TUPLES KEY_T;

std::string folder = "../datasets/";
std::string file[4] = {"130000.dat", "criteo_key.log", "zipf_1.0.dat", "zipf_1.5.dat"};

uint64_t datasetLength;
KEY_T* dataset;

class scheme {
public:
    std::string name;
    scheme(std::string n): name(n) {}
    virtual double measure(int, int) = 0;
};

class ARE: public scheme {
public:
    ARE(): scheme(std::string("ARE")) {}
    double measure(int ans, int truans) {
        double dt = std::abs(ans - truans);
        return dt / truans;
    }
};

class AAE: public scheme {
public:
    AAE(): scheme(std::string("AAE")) {}
    double measure(int ans, int truans) {
        double dt = std::abs(ans - truans);
        return dt;
    }
};

Abstract* getSketch(int window_size, int memory, int counterNum, int counterSize, int sketchtype) {
    Abstract* sketch;

    switch (sketchtype)
    {
    case 4:
        sketch = new RingSketch(window_size, memory, counterNum, counterSize);
        break;
    case 5:
        sketch = new ClockSketch(window_size, memory, 1 + (0.6931 * memory * 8) / (window_size * counterSize), counterSize);
        break;
    case 6:
        sketch = new TimeoutBloomFilter(memory, counterNum);
        break;
    case 7:
        sketch = new HyperBloomFilter(window_size, memory, 8, 8, counterSize);
        break;
    case 8:
        sketch = new RingSketchSimd(window_size, memory, counterNum, counterSize);
        break;
    case 9:
        sketch = new SWAMP(memory / 25, memory);
        break;
    case 10:
        sketch = new RingSketchThread(window_size, memory, counterNum, counterSize, 1);
        break;
    }

    return sketch;
}

std::string test(int query_freq, int window_size, int memory, int counterNum, int counterSize, int sketchtype, scheme* measurer) {
    Abstract* sketch;
    uint64_t c = datasetLength;

    double accu = 0;
    double num = 0;

    for(int k = 0; k < 4; ++k) {
        thread_local std::mt19937 rng(131 + k + std::hash<std::thread::id>{}(std::this_thread::get_id()));
        
        std::deque<std::pair<int, KEY_T>> lastTime;
        sketch = getSketch(window_size, memory, counterNum, counterSize, sketchtype);
        for (int i = 0; i < c; i++) {
            KEY_T tuple = dataset[i];
        
            if (i > 0 && i % query_freq == 0) {
                while (i - lastTime.front().first > window_size)
                    lastTime.pop_front();

                std::uniform_int_distribution<int> dist(0, lastTime.size() - 1);
                auto random_it = std::next(std::begin(lastTime), dist(rng));

                int gt = i - random_it->first;

                for (auto iter = lastTime.rbegin(); iter != lastTime.rend(); ++iter)
                    if (iter->second == random_it->second) {
                        gt = i - iter->first;
                        break;
                    }

                int est = sketch->estimate(random_it->second, i);

                accu += measurer->measure(est, gt);
                num += 1;
            }

            sketch->insert(tuple, i);
            lastTime.emplace_back(i, tuple);
            sketch->updateClock(1);
        }
        delete sketch;
    }
    
    char returnString[200] = {0};
    sprintf(returnString, ",%.10f", accu / num);
    return std::string(returnString);
}

std::string testTHR(int window_size, int memory, int counterNum, int counterSize, int sketchtype, int test_cycle) {
    Abstract* sketch;

    uint64_t c = datasetLength;
    auto t3 = 0;

    for(int i = 0; i < test_cycle; ++i) {
        sketch = getSketch(window_size, memory, counterNum, counterSize, sketchtype == 8 ? 10 : sketchtype);
        auto t1 = std::chrono::steady_clock::now();
        for(int k = 0; k < c; k++){
            KEY_T tuple = dataset[k];
            sketch->insert(tuple, i);
            if(sketchtype != 10)
                sketch->updateClock(1);
        }
        auto t2 = std::chrono::steady_clock::now();
        t3 += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        delete sketch;
    }

    char returnString[200] = {0};
    sprintf(returnString, ",%.10f", c * test_cycle / (1.0 * t3));
    return std::string(returnString);
}

std::string testQueryTHR(int window_size, int memory, int counterNum, int counterSize, int sketchtype, int test_cycle) {
    Abstract* sketch;

    uint64_t c = datasetLength;
    auto t13 = 0, t23 = 0;
    int64_t t3 = 0;

    for(int i = 0; i < test_cycle; ++i) {
        sketch = getSketch(window_size, memory, counterNum, counterSize, sketchtype);
        for(int k = 0; k < c; k++){
            TUPLES tuple = dataset[k];
            sketch->insert(tuple, i);
            if(sketchtype != 3)
                sketch->updateClock(1);
        }
        auto t1 = std::chrono::steady_clock::now();
        for(int k = 0; k < c; k++)
            sketch->estimate(dataset[k], k);
        auto t2 = std::chrono::steady_clock::now();
        t3 +=std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        delete sketch;
    }

    char returnString[200] = {0};
    sprintf(returnString, ",%.10f", c * test_cycle / (1.0 * t3));
    return std::string(returnString);
}

std::string getString(int memory, int counterNum, int counterSize, int t) {
    char returnString[200] = {0};
    Abstract *sketch = getSketch(1<<13, memory, counterNum, counterSize, t);
    sprintf(returnString, "%s,%d,%d,%d", sketch->name.c_str(), memory, counterNum, counterSize);
    delete sketch;
    return std::string(returnString);
}

struct TestTask {
    int memory_multiplier;
    int counterNum;
    int counterSize;
    int sketchtype;
    std::string task_group;
    
    TestTask(int mem_mult, int cn, int cs, int st, const std::string& group) 
        : memory_multiplier(mem_mult), counterNum(cn), counterSize(cs), sketchtype(st), task_group(group) {}
};

struct TestResult {
    std::string info;
    std::string are_result;
    std::string aae_result;
    std::string update_thr_result;
    std::string query_thr_result;
    std::string task_group;
    int order_index;
    
    std::string toCSVLine() const {
        return info + are_result + aae_result + update_thr_result + query_thr_result;
    }
};

TestResult executeTestTask(const TestTask& task, int query_freq, int window_size, int memory_base, int order_idx) {
    TestResult result;
    result.task_group = task.task_group;
    result.order_index = order_idx;
    
    ARE* are = new ARE();
    AAE* aae = new AAE();
    
    try {
        int memory = memory_base * task.memory_multiplier;
        
        result.info = getString(memory, task.counterNum, task.counterSize, task.sketchtype);
        result.are_result = test(query_freq, window_size, memory, task.counterNum, task.counterSize, task.sketchtype, are);
        result.aae_result = test(query_freq, window_size, memory, task.counterNum, task.counterSize, task.sketchtype, aae);
        result.update_thr_result = testTHR(window_size, memory, task.counterNum, task.counterSize, task.sketchtype, 5);
        result.query_thr_result = testQueryTHR(window_size, memory, task.counterNum, task.counterSize, task.sketchtype, 5);
    } catch (const std::exception& e) {
        std::cerr << "Error in test task (group: " << task.task_group << "): " << e.what() << std::endl;
        result.info = "ERROR";
        result.are_result = ",0.0";
        result.aae_result = ",0.0";
        result.update_thr_result = ",0.0";
        result.query_thr_result = ",0.0";
    }
    
    delete are;
    delete aae;
    
    return result;
}

void testCaseParallel(std::ofstream &outFile, int memory_base) {
    int query_freq = 10;
    int window_size = 1 << 13;

    outFile << "name,memory,counterNum,counterSize,are,aae,thr" << std::endl;

    std::vector<TestTask> tasks;
    int order_counter = 0;
    
    for (int i = 16; i >= 4; i--) {
        for(int counterNum = 16; counterNum <= 16; counterNum += 1) {
                tasks.emplace_back(i, 4, 16, 8, "group1");
                order_counter++;
            }
        }

    for (int i = 16; i >= 4; i --) {
        for(int t = 9; t > 6; t -= 2) {
            tasks.emplace_back(i, 0, 8, t, "group1_subgroup1");
            order_counter++;
        }
        
        for(int counterNum = 16; counterNum <= 16; counterNum += 4) {
            tasks.emplace_back(i, counterNum, 8, 6, "group1_subgroup2");
            order_counter++;
        }
        
        for(int counterNum = 4; counterNum <= 4; counterNum += 4) {
            for(int counterSize = 16; counterSize <= 16; counterSize += 4) {
                tasks.emplace_back(i, counterNum, counterSize, 5, "group1_subgroup3");
                order_counter++;
            }
        }
    }

    unsigned int num_threads = std::min(static_cast<unsigned int>(tasks.size()), 
                                       std::thread::hardware_concurrency());
    if (num_threads == 0) num_threads = 1;
    
    std::cout << "Using " << num_threads << " threads for " << tasks.size() << " tasks" << std::endl;

    std::vector<std::future<TestResult>> futures;
    futures.reserve(tasks.size());

    for (size_t i = 0; i < tasks.size(); ++i) {
        futures.push_back(
            std::async(std::launch::async, executeTestTask, tasks[i], query_freq, window_size, memory_base, i)
        );
    }

    std::mutex file_mutex;
    
    for (size_t i = 0; i < futures.size(); ++i) {
        try {
            TestResult result = futures[i].get();
            
            std::lock_guard<std::mutex> lock(file_mutex);
            outFile << result.toCSVLine() << std::endl;
            outFile.flush();
            
            std::cout << "Completed task " << (i + 1) << "/" << futures.size() << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Error getting result for task " << i << ": " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {

    int dataset_index = 3;
    
    std::cout << "Loading dataset: " << file[dataset_index] << std::endl;
    dataset = read_data("/root/RingSketch/github_backup/dataset/zipf.dat", 1 << 20, &datasetLength, 4);
    std::cout << "Dataset length: " << datasetLength << std::endl;

    int memory_base = 8000;

    std::string output_filename = "../results/testZIPF0_parallel_" + std::to_string(dataset_index) + ".csv";
    std::ofstream outFile(output_filename);
    
    auto start_time = std::chrono::steady_clock::now();
    testCaseParallel(outFile, memory_base);
    auto end_time = std::chrono::steady_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "Total execution time: " << duration.count() << " seconds" << std::endl;
    std::cout << "Results saved to: " << output_filename << std::endl;
    
    outFile.close();
    return 0;
}
