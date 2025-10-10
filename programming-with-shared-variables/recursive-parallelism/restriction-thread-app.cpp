// ./programming-with-shared-variables/recursive-parallelism/restriction-thread-app.cpp

#include <atomic>
#include <cmath>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

constexpr double R = 1;
constexpr double eps = 1e-7;
constexpr int max_thread_count = 7;

double f(double x);
void integrate(double a, double b, double fa, double fb, double* s);

mutex mtx;
atomic current_threads(0);
atomic max_threads(0);

double f(const double x) {
    return sqrt(R * R - x * x);
}

void integrate(const double a, const double b, const double fa, const double fb, double* s) {
    const double m = (a + b) / 2;
    const double fm = f(m);

    double sl = (m - a) * (fa + fm) / 2;
    double sr = (b - m) * (fm + fb) / 2;

    if (abs(sl + sr - *s) < eps * 3) {
        *s = sl + sr;
        return;
    }

    bool createLeft = false;
    bool createRight = false;

    {
        lock_guard lock(mtx);
        if (current_threads < max_thread_count) {
            createLeft = true;
            ++current_threads;

            if (current_threads > max_threads) {
                max_threads.store(current_threads);
            }
        }
        if (current_threads < max_thread_count) {
            createRight = true;
            ++current_threads;

            if (current_threads > max_threads) {
                max_threads.store(current_threads);
            }
        }
    }

    thread tl, tr;

    if (createLeft) {
        tl = thread([=, &sl] {
            integrate(a, m, fa, fm, &sl);
            {
                lock_guard lock(mtx);
                --current_threads;
            }
        });
    } else {
        integrate(a, m, fa, fm, &sl);
    }

    if (createRight) {
        tr = thread([=, &sr] {
            integrate(m, b, fm, fb, &sr);
            {
                lock_guard lock(mtx);
                --current_threads;
            }
        });
    } else {
        integrate(m, b, fm, fb, &sr);
    }

    if (tl.joinable()) {
        tl.join();
    }

    if (tr.joinable()) {
        tr.join();
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
