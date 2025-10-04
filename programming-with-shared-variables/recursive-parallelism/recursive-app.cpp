// ./programming-with-shared-variables/recursive-parallelism/recursive-app.cpp

#include <cmath>
#include <iostream>

using namespace std;

constexpr double R = 1;
constexpr double eps = 1e-7;

double f(double x);
double integrate(double a, double b, double fa, double fb, double s);

double f(const double x) {
    return sqrt(R * R - x * x);
}

double integrate(const double a, const double b, const double fa, const double fb, const double s) {
    const double m = (a + b) / 2;
    const double fm = f(m);

    const double sl = (m - a) * (fa + fm) / 2;
    const double sr = (b - m) * (fm + fb) / 2;

    if (abs(sl + sr - s) < eps * 3) {
        return sl + sr;
    }

    return integrate(a, m, fa, fm, sl) + integrate(m, b, fm, fb, sr);
}

int main() {
    constexpr double a = 0;
    constexpr double b = R;

    const double fa = f(a);
    const double fb = f(b);

    const double s = (b - a) * (fa + fb) / 2;

    const double result = integrate(a, b, fa, fb, s);
    constexpr double area = M_PI * R * R / 4;
    const double error = abs(result - area);

    cout << result << endl;
    cout << area << endl;
    cout << error << endl;
}
