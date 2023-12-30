#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore>
#include <barrier>
#include <atomic>
#include <random>
#include <chrono>

// Генератор случайных чисел для символов ASCII
char random_ascii() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(33, 126);
    return static_cast<char>(dis(gen));
}

// Функция для потока, использующая Mutex
void thread_func_mutex(std::mutex& mtx, int thread_num) {
    std::lock_guard<std::mutex> lock(mtx);
    // Критическая секция
    std::cout << "Thread " << thread_num << ": " << random_ascii() << std::endl;
}

// Функция для потока, использующая Semaphore
void thread_func_semaphore(std::counting_semaphore<>& sem, int thread_num) {
    sem.acquire();
    // Критическая секция
    std::cout << "Thread " << thread_num << ": " << random_ascii() << std::endl;
    sem.release();
}

// Функция для потока, использующая Barrier
void thread_func_barrier(std::barrier<>& bar, int thread_num) {
    bar.arrive_and_wait();
    // Критическая секция
    std::cout << "Thread " << thread_num << ": " << random_ascii() << std::endl;
}

// Функция для потока, использующая SpinLock
void thread_func_spinlock(std::atomic_flag& flag, int thread_num) {
    while (flag.test_and_set(std::memory_order_acquire)); // Spin
    // Критическая секция
    std::cout << "Thread " << thread_num << ": " << random_ascii() << std::endl;
    flag.clear(std::memory_order_release);
}

// Функция для потока, использующая SpinWait
void thread_func_spinwait(std::atomic_flag& flag, int thread_num) {
    while (flag.test_and_set(std::memory_order_acquire)); // SpinWait
    // Критическая секция
    std::cout << "Thread " << thread_num << ": " << random_ascii() << std::endl;
    flag.clear(std::memory_order_release);
}

// Функция для потока, использующая SemaphoreSlim
void thread_func_semaphoreslim(std::counting_semaphore<>& sem, int thread_num) {
    sem.acquire();
    // Критическая секция
    std::cout << "Thread " << thread_num << ": " << random_ascii() << std::endl;
    sem.release();
}

// Функция для потока, использующая Monitor (через std::unique_lock)
void thread_func_monitor(std::mutex& mtx, int thread_num) {
    std::unique_lock<std::mutex> lock(mtx);
    // Критическая секция
    std::cout << "Thread " << thread_num << ": " << random_ascii() << std::endl;
}

int main() {
    setlocale(LC_ALL, "Rus");
    int num_threads;
    std::string sync_primitive;
    std::cout << "Введите количество потоков: ";
    std::cin >> num_threads;
    std::cout << "Введите примитив синхронизации (Mutexes, Semaphore, Barrier, SpinLock, Monitor, SpinWait, SemaphoreSlim): ";
    std::cin >> sync_primitive;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    // Выбор примитива синхронизации
    if (sync_primitive == "Mutexes") {
        std::mutex mtx;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(thread_func_mutex, std::ref(mtx), i);
        }
    }
    else if (sync_primitive == "Semaphore") {
        std::counting_semaphore<> sem(3); // Разрешить одновременный доступ трех потоков
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(thread_func_semaphore, std::ref(sem), i);
        }
    }
    else if (sync_primitive == "SpinWait") {
        std::atomic_flag flag = ATOMIC_FLAG_INIT;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(thread_func_spinwait, std::ref(flag), i);
        }
    }
    else if (sync_primitive == "SemaphoreSlim") {
        std::counting_semaphore<> sem(1); // Разрешить одновременный доступ одного потока
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(thread_func_semaphoreslim, std::ref(sem), i);
        }
    }
    else if (sync_primitive == "Barrier") {
        std::barrier<> bar(num_threads);
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(thread_func_barrier, std::ref(bar), i);
        }
    }
    else if (sync_primitive == "SpinLock") {
        std::atomic_flag flag = ATOMIC_FLAG_INIT;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(thread_func_spinlock, std::ref(flag), i);
        }
    }
    else if (sync_primitive == "Monitor") {
        std::mutex mtx;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(thread_func_monitor, std::ref(mtx), i);
        }
    }
    else {
        std::cout << "Неверный ввод примитива синхронизации." << std::endl;
        return 1;
    }
    auto start = std::chrono::high_resolution_clock::now(); // Начало измерения времени
    // Запуск и ожидание завершения всех потоков
    for (auto& th : threads) {
        th.join();
    }
    auto end = std::chrono::high_resolution_clock::now(); // Конец измерения времени
    std::chrono::duration<double, std::milli> elapsed = end - start; // Вычисление затраченного времени

    std::cout << "Время выполнения: " << elapsed.count() << " мс\n";
    return 0;
}
