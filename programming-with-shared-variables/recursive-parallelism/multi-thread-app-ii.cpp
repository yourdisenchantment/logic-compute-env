// ./programming-with-shared-variables/recursive-parallelism/multi-thread-app-ii.cpp

#include <cmath>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

constexpr double R = 1;
constexpr double eps = 0.001;

double f(double x);
void integrate(double a, double b, double fa, double fb, double *s);

double f(const double x) {
    return sqrt(R * R - x * x);
}

void integrate(const double a, const double b, const double fa, const double fb, double *s) {
    double m = (a + b) / 2;
    double fm = f(m);

    double sl = (m - a) * (fa + fm) / 2;
    double sr = (b - m) * (fm + fb) / 2;

    if (abs(sl + sr - *s) < eps * 3) {
        *s = sl + sr;
        return;
    }

    auto tl = thread(integrate, m, b, fm, fb, &sr);
    auto tr = thread(integrate, a, m, fa, fm, &sl);

    tl.join();
    tr.join();

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
}
