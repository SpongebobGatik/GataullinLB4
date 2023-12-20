#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

const int num_philosophers = 5;

std::mutex forks[num_philosophers]; // Массив мьютексов, представляющих вилки.

// Функция, описывающая поведение философа.
void dine(int philosopher_number) {
    int left_fork = philosopher_number; // Номер левой вилки для философа.
    int right_fork = (philosopher_number + 1) % num_philosophers; // Номер правой вилки для философа.

    while (true) { // Бесконечный цикл, философы постоянно едят и размышляют.
        std::cout << "Философ " << philosopher_number << " размышляет.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Философ размышляет 1 секунду.

        forks[left_fork].lock(); // Философ берет левую вилку.
        std::cout << "Философ " << philosopher_number << " взял левую вилку.\n";

        forks[right_fork].lock(); // Философ берет правую вилку.
        std::cout << "Философ " << philosopher_number << " взял правую вилку.\n";

        std::cout << "Философ " << philosopher_number << " ест.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Философ ест 1 секунду.

        forks[right_fork].unlock(); // Философ кладет правую вилку.
        forks[left_fork].unlock(); // Философ кладет левую вилку.

        std::cout << "Философ " << philosopher_number << " положил вилки.\n";
    }
}

int main() {
    setlocale(LC_ALL, "Rus");
    std::thread philosophers[num_philosophers]; // Массив потоков для философов.
    for (int i = 0; i < num_philosophers; ++i) {
        philosophers[i] = std::thread(dine, i); // Создание потока для каждого философа.
    }

    for (auto& philosopher : philosophers) {
        philosopher.join(); // Ожидание завершения работы каждого потока.
    }

    return 0;
}
