#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>
#include <vector>

void find_extremum(std::vector<int>& vec, int start, int finish, bool max_min, int& result)
{
    if (max_min == 1)
    {
        int max = vec[start];

        for (int i = start + 1; i < finish; i++)
            if (vec[i] > max)
                max = vec[i];

        result = max;
    }
    else
    {
        int min = vec[start];

        for (int i = start + 1; i < finish; i++)
            if (vec[i] < min)
                min = vec[i];

        result = min;
    }
}

int main()
{
    std::vector<int> vec_of_time;
    std::vector<int> vec_of_values;
    std::vector<int> result;
    int num_of_threads = 0;
    int size_of_vec = 0;

    std::srand(std::time(nullptr));

    std::cout << "Enter the size of array: ";
    std::cin >> size_of_vec;

    vec_of_values.resize(size_of_vec);

    for (int& i : vec_of_values)
        i = std::rand();

    std::cout << "The available number of threads: " << std::thread::hardware_concurrency() << std::endl;

    std::cout << "Enter the number of threads: ";
    std::cin >> num_of_threads;

    std::vector<std::thread*> my_threads(num_of_threads);
    result.resize(num_of_threads);

    int attitude = size_of_vec / num_of_threads;

    auto begin = std::chrono::system_clock::now();

    for (int i = 0; i < num_of_threads; i++)
        my_threads[i] = new std::thread(find_extremum,
                                        std::ref(vec_of_values),
                                        i * attitude,
                                        (i + 1) * attitude,
                                        false,
                                        std::ref(result[i]));

    for (int i = 0; i < num_of_threads; i++)
    {
        my_threads[i]->join();
        delete my_threads[i];
    }

    int min = 0;

    find_extremum(std::ref(result), 0, num_of_threads, true, std::ref(min));

    auto end = std::chrono::system_clock::now();

    int time1 = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    std::cout << "Time of finding extreme values with threats: " << time1 << " ms\n";

    /*one thread*/
    begin = std::chrono::system_clock::now();
    for (int i = 0; i < num_of_threads; i++)
        find_extremum(std::ref(vec_of_values), i * attitude, (i + 1) * attitude, false, std::ref(result[i]));

    find_extremum(std::ref(result), 0, num_of_threads, true, std::ref(min));

    end = std::chrono::system_clock::now();
    auto time2 = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    std::cout << "The time of work with one threat: " << time2 << " ms\n";

    if (time1 > time2)
        std::cout << "The best time is " << time2 << " micro\n";
    else
        std::cout << "The best time is " << time1 << " micro\n";

    result.clear();

    /* Определяю кол-во потоков и размер вектора, при котором многопоточная реализация становится эффективнее однопоточной*/

    std::vector<std::thread*> new_threads(size_of_vec);

    for (int i = 2; i < vec_of_values.size() + 1; i++)
    {
        result.resize(i);

        int attitude = size_of_vec / i;

        auto begin = std::chrono::system_clock::now();

        for (int j = 0; j < i; j++)
            new_threads[i] = new std::thread(find_extremum,
                                             std::ref(vec_of_values),
                                             j * attitude,
                                             (j + 1) * attitude,
                                             false,
                                             std::ref(result[j]));

        for (int j = 0; j < i; j++)
        {
            new_threads[j]->join();
            delete new_threads[j];
        }

        int max = 0;

        find_extremum(std::ref(result), 0, i, true, std::ref(max));

        auto end = std::chrono::system_clock::now();

        vec_of_time.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

        result.clear();
        my_threads.clear();
    }

    int min_time = vec_of_time[0];
    int best_thread = 2;

    for (int i = 0; i < vec_of_time.size(); i++)
        if (vec_of_time[i] < min_time)
        {
            min_time = vec_of_time[i];
            best_thread = i + 2;
        }

    std::cout << "The best time is " << min_time << " ms with " << best_thread << " thread(s)";

    return 0;
}