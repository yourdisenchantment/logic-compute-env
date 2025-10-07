// ./programming-with-shared-variables/recursive-parallelism/restriction-thread-app.cpp

#include <atomic>
#include <cmath>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

constexpr double R = 1;
constexpr double eps = 1e-7;
constexpr int max_thread_count = 4;

double f(double x);
void integrate(double a, double b, double fa, double fb, double* s);

mutex mtx;
atomic current_threads(0);
atomic max_threads(0);

double f(const double x) {
    return sqrt(R * R - x * x);
}

void integrate(const double a, const double b, const double fa, const double fb, double* s) {
    bool can_create_threads = false;

    {
        lock_guard lock(mtx);

        if (current_threads < max_thread_count) {
            current_threads += 2;
            can_create_threads = true;

            if (current_threads > max_threads) {
                max_threads.store(current_threads);
            }
        }
    }

    double m = (a + b) / 2;
    double fm = f(m);

    double sl = (m - a) * (fa + fm) / 2;
    double sr = (b - m) * (fm + fb) / 2;

    if (abs(sl + sr - *s) < eps * 3) {
        *s = sl + sr;
        return;
    }

    if (can_create_threads) {
        auto tl = thread(integrate, a, m, fa, fm, &sl);
        auto tr = thread(integrate, m, b, fm, fb, &sr);

        tl.join();
        tr.join();

        {
            lock_guard lock(mtx);
            current_threads -= 2;
        }

    } else {
        integrate(a, m, fa, fm, &sl);
        integrate(m, b, fm, fb, &sr);
    }

    *s = sl + sr;
}

int main() {
    constexpr double a = 0;
    constexpr double b = R;

    const double fa = f(a);
    const double fb = f(b);

    double s = (b - a) * (fa + fb) / 2;
    integrate(a, b, fa, fb, &s);

    constexpr double area = M_PI * R * R / 4;
    const double error = abs(s - area);

    cout << s << endl;
    cout << area << endl;
    cout << error << endl;
    cout << max_threads << endl;
    cout << current_threads << endl;
}
