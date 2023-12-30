#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

struct SessionResult {
    int semester;
    string discipline;
    int grade;
};

struct Student {
    string name;
    string group;
    vector<SessionResult> results;
};

// Функция для вычисления средней оценки студентов группы за семестр
double CalculateAverage(const vector<Student>& students, const string& group, int semester) {
    double sum = 0;
    int count = 0;
    for (const auto& student : students) {
        if (student.group == group) {
            for (const auto& result : student.results) {
                if (result.semester == semester) {
                    sum += result.grade;
                    ++count;
                }
            }
        }
    }
    return count == 0 ? 0 : sum / count;
}

// Функция для обработки данных без многопоточности
pair<double, double> ProcessWithoutMultithreading(const vector<Student>& students, const string& group, int semester) {
    auto start = chrono::high_resolution_clock::now();
    double average = CalculateAverage(students, group, semester);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    return make_pair(average, duration.count());
}

// Глобальный мьютекс для синхронизации вывода
mutex output_mutex;

// Функция для обработки данных в отдельном потоке
void ThreadFunction(const vector<Student>& students, const string& group, int semester, pair<double, double>& result) {
    auto start = chrono::high_resolution_clock::now();
    double average = CalculateAverage(students, group, semester);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    lock_guard<mutex> lock(output_mutex);
    result = make_pair(average, duration.count());
}

int main() {
    setlocale(LC_ALL, "Rus");
    vector<Student> students = {
        // Пример данных о студентах
        {"Иванов Иван Иванович1", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
        {"Иванов Иван Иванович2", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
        {"Иванов Иван Иванович3", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
        {"Иванов Иван Иванович1", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
        {"Иванов Иван Иванович2", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
        {"Иванов Иван Иванович3", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
        {"Иванов Иван Иванович1", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
        {"Иванов Иван Иванович2", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
        {"Иванов Иван Иванович3", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
    };

    // Вызов функции без многопоточности
    pair<double, double> result = ProcessWithoutMultithreading(students, "Г1", 1);
    cout << "Время обработки без использования многопоточности: " << result.second << " мс\n";
    cout << "Средняя успеваемость студентов группы Г1 за семестр 1: " << result.first << "\n";

    // Многопоточная обработка
    int num_threads = 16; // количество потоков
    vector<thread> workers;
    vector<pair<double, double>> threaded_results(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        workers.push_back(thread(ThreadFunction, ref(students), "Г1", 1, ref(threaded_results[i])));
    }
    for (auto& worker : workers) {
        worker.join();
    }
    double total_average = 0;
    double total_time = 0;
    for (const auto& res : threaded_results) {
        total_average += res.first;
        total_time += res.second;
    }
    total_average /= num_threads;
    total_time /= num_threads;
    cout << "Время обработки с использованием многопоточности: " << total_time << " мс\n";
    cout << "Средняя успеваемость студентов группы Г1 за семестр 1 (многопоточность): " << total_average << "\n";

    return 0;
}
