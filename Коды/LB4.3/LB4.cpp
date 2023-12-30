#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>

std::counting_semaphore<2> max_philosophers(2);

void philosopher(int id, std::vector<std::unique_ptr<std::binary_semaphore>>& forks) {
    while (true) {
        std::cout << "Философ " << id << " думает\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Философ " << id << " хочет есть\n";
        max_philosophers.acquire();
        forks[id]->acquire();
        forks[(id + 1) % 5]->acquire();
        std::cout << "Философ " << id << " ест\n";
        forks[id]->release();
        forks[(id + 1) % 5]->release();
        max_philosophers.release();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    setlocale(LC_ALL, "Rus");
    std::vector<std::unique_ptr<std::binary_semaphore>> forks;
    for (int i = 0; i < 5; ++i) {
        forks.push_back(std::make_unique<std::binary_semaphore>(1));
    }
    std::vector<std::thread> philosophers;
    for (int i = 0; i < 5; ++i) {
        philosophers.emplace_back(philosopher, i, std::ref(forks));
    }
    for (auto& philosopher : philosophers) {
        philosopher.join();
    }
    return 0;
}
