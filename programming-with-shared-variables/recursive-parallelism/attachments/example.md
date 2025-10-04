Вот модифицированная версия кода с отслеживанием количества потоков и использованием мьютексов:

```cpp
// ./programming-with-shared-variables/recursive-parallelism/recursive-app.cpp

#include <cmath>
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

constexpr double R = 1;
constexpr double eps = 0.001;

// Глобальные переменные для отслеживания потоков
int current_threads = 1;  // Начинаем с 1 (главный поток)
int max_threads = 1;
mutex thread_counter_mutex;

double f(double x);
void integrate(double a, double b, double fa, double fb, double* s);

// RAII класс для автоматического управления счетчиком потоков
class ThreadCounter {
public:
    ThreadCounter() {
        lock_guard<mutex> lock(thread_counter_mutex);
        current_threads++;
        if (current_threads > max_threads) {
            max_threads = current_threads;
        }
        cout << "Поток запущен. Текущее количество потоков: " << current_threads << endl;
    }
    
    ~ThreadCounter() {
        lock_guard<mutex> lock(thread_counter_mutex);
        current_threads--;
        cout << "Поток завершен. Текущее количество потоков: " << current_threads << endl;
    }
};

double f(const double x) {
    return sqrt(R * R - x * x);
}

void integrate(const double a, const double b, const double fa, const double fb, double* s) {
    // Создаем счетчик потоков для текущей функции (кроме главного потока)
    ThreadCounter* counter = nullptr;
    if (this_thread::get_id() != thread([](){}).get_id()) {
        // Проверяем, что это не главный поток (приблизительно)
        // В реальности лучше передавать флаг или использовать другой способ
        static thread_local bool is_main_thread = false;
        static once_flag main_thread_flag;
        call_once(main_thread_flag, []() { is_main_thread = true; });
        
        if (!is_main_thread) {
            counter = new ThreadCounter();
        }
    }
    
    double m = (a + b) / 2;
    double fm = f(m);

    double sl = (m - a) * (fa + fm) / 2;
    double sr = (b - m) * (fm + fb) / 2;

    if (abs(sl + sr - *s) < eps * 3) {
        *s = sl + sr;
        delete counter;  // Освобождаем память
        return;
    }

    auto tl = thread(integrate, m, b, fm, fb, &sr);
    auto tr = thread(integrate, a, m, fa, fm, &sl);

    tl.join();
    tr.join();

    *s = sl + sr;
    delete counter;  // Освобождаем память
}

int main() {
    constexpr double a = 0;
    constexpr double b = R;

    const double fa = f(a);
    const double fb = f(b);

    // Запоминаем начальное количество потоков
    int initial_threads;
    {
        lock_guard<mutex> lock(thread_counter_mutex);
        initial_threads = current_threads;
    }

    cout << "Начальное количество потоков: " << initial_threads << endl;
    cout << "=== Начало вычислений ===" << endl;

    double s = (b - a) * (fa + fb) / 2;
    integrate(a, b, fa, fb, &s);

    cout << "=== Конец вычислений ===" << endl;

    // Проверяем финальное количество потоков
    int final_threads;
    int max_threads_reached;
    {
        lock_guard<mutex> lock(thread_counter_mutex);
        final_threads = current_threads;
        max_threads_reached = max_threads;
    }

    constexpr double area = M_PI * R * R / 4;
    const double error = abs(s - area);

    cout << "\n=== РЕЗУЛЬТАТЫ ===" << endl;
    cout << "Вычисленная площадь: " << s << endl;
    cout << "Аналитическая площадь: " << area << endl;
    cout << "Погрешность: " << error << endl;
    
    cout << "\n=== СТАТИСТИКА ПОТОКОВ ===" << endl;
    cout << "Начальное количество потоков: " << initial_threads << endl;
    cout << "Финальное количество потоков: " << final_threads << endl;
    cout << "Максимальное количество потоков: " << max_threads_reached << endl;
    cout << "Совпадение начального и финального: " << (initial_threads == final_threads ? "ДА" : "НЕТ") << endl;

    return 0;
}
```

Однако, приведенный выше код имеет проблему с определением главного потока. Вот более чистая версия:

```cpp
// ./programming-with-shared-variables/recursive-parallelism/recursive-app.cpp

#include <cmath>
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

constexpr double R = 1;
constexpr double eps = 0.001;

// Глобальные переменные для отслеживания потоков
int current_threads = 1;  // Начинаем с 1 (главный поток)
int max_threads = 1;
mutex thread_counter_mutex;

double f(double x);
void integrate(double a, double b, double fa, double fb, double* s, bool is_main_call = false);

double f(const double x) {
    return sqrt(R * R - x * x);
}

void integrate(const double a, const double b, const double fa, const double fb, double* s, bool is_main_call) {
    // Увеличиваем счетчик потоков только для дочерних вызовов
    bool should_count = !is_main_call;
    
    if (should_count) {
        lock_guard<mutex> lock(thread_counter_mutex);
        current_threads++;
        if (current_threads > max_threads) {
            max_threads = current_threads;
        }
        cout << "Поток запущен. Текущее количество: " << current_threads << endl;
    }
    
    double m = (a + b) / 2;
    double fm = f(m);

    double sl = (m - a) * (fa + fm) / 2;
    double sr = (b - m) * (fm + fb) / 2;

    if (abs(sl + sr - *s) < eps * 3) {
        *s = sl + sr;
        
        // Уменьшаем счетчик потоков при выходе
        if (should_count) {
            lock_guard<mutex> lock(thread_counter_mutex);
            current_threads--;
            cout << "Поток завершен. Текущее количество: " << current_threads << endl;
        }
        return;
    }

    auto tl = thread(integrate, m, b, fm, fb, &sr, false);
    auto tr = thread(integrate, a, m, fa, fm, &sl, false);

    tl.join();
    tr.join();

    *s = sl + sr;
    
    // Уменьшаем счетчик потоков при выходе
    if (should_count) {
        lock_guard<mutex> lock(thread_counter_mutex);
        current_threads--;
        cout << "Поток завершен. Текущее количество: " << current_threads << endl;
    }
}

int main() {
    constexpr double a = 0;
    constexpr double b = R;

    const double fa = f(a);
    const double fb = f(b);

    // Запоминаем начальное количество потоков
    int initial_threads;
    {
        lock_guard<mutex> lock(thread_counter_mutex);
        initial_threads = current_threads;
    }

    cout << "Начальное количество потоков: " << initial_threads << endl;
    cout << "=== Начало вычислений ===" << endl;

    double s = (b - a) * (fa + fb) / 2;
    integrate(a, b, fa, fb, &s, true);  // true означает вызов из main

    cout << "=== Конец вычислений ===" << endl;

    // Проверяем финальное количество потоков
    int final_threads;
    int max_threads_reached;
    {
        lock_guard<mutex> lock(thread_counter_mutex);
        final_threads = current_threads;
        max_threads_reached = max_threads;
    }

    constexpr double area = M_PI * R * R / 4;
    const double error = abs(s - area);

    cout << "\n=== РЕЗУЛЬТАТЫ ===" << endl;
    cout << "Вычисленная площадь: " << s << endl;
    cout << "Аналитическая площадь: " << area << endl;
    cout << "Погрешность: " << error << endl;
    
    cout << "\n=== СТАТИСТИКА ПОТОКОВ ===" << endl;
    cout << "Начальное количество потоков: " << initial_threads << endl;
    cout << "Финальное количество потоков: " << final_threads << endl;
    cout << "Максимальное количество потоков: " << max_threads_reached << endl;
    cout << "Совпадение начального и финального: " << (initial_threads == final_threads ? "ДА" : "НЕТ") << endl;

    return 0;
}
```

**Основные изменения:**

1. **Глобальные переменные:** `current_threads`, `max_threads` и `thread_counter_mutex` для отслеживания потоков
2. **Мьютекс:** `lock_guard<mutex>` используется для безопасного доступа к общим переменным
3. **Счетчики потоков:** Увеличиваются при входе в функцию (кроме главного потока) и уменьшаются при выходе
4. **Максимальное количество:** Отслеживается при каждом увеличении счетчика
5. **Параметр `is_main_call`:** Позволяет различать вызов из main и из дочернего потока

Программа покажет:

- Начальное количество потоков (1)
- Максимальное количество потоков, работавших одновременно
- Финальное количество потоков (должно совпасть с начальным)
- Детальную информацию о запуске и завершении каждого потока
