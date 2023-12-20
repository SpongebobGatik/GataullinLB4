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
double ProcessWithoutMultithreading(const vector<Student>& students, const string& group, int semester) {
    auto start = chrono::high_resolution_clock::now();
    double average = CalculateAverage(students, group, semester);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Время обработки без использования многопоточности: " << duration.count() << " мс\n";
    return average;
}

// Глобальный мьютекс для синхронизации вывода
mutex output_mutex;

// Функция для обработки данных в отдельном потоке
void ThreadFunction(const vector<Student>& students, const string& group, int semester, double& average) {
    auto start = chrono::high_resolution_clock::now();
    average = CalculateAverage(students, group, semester);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    lock_guard<mutex> lock(output_mutex);
    cout << "Время обработки с использованием многопоточности: " << duration.count() << " мс\n";
}

int main() {
    setlocale(LC_ALL, "Rus");
    vector<Student> students = {
        // Пример данных о студентах
        {"Иванов Иван Иванович", "Г1", {{1, "Математика", 4}, {1, "Физика", 5}}},
    };

    // Вызов функции без многопоточности
    double average = ProcessWithoutMultithreading(students, "Г1", 1);
    cout << "Средняя успеваемость студентов группы Г1 за семестр 1: " << average << "\n";

    // Многопоточная обработка
    double threaded_average = 0;
    thread worker(ThreadFunction, ref(students), "Г1", 1, ref(threaded_average));
    worker.join();
    cout << "Средняя успеваемость студентов группы Г1 за семестр 1 (многопоточность): " << threaded_average << "\n";

    return 0;
}
