// ./programming-with-shared-variables/producers-and-consumers/n-queens-consumer.cpp

#include <cerrno>
#include <cstdlib>
#include <iostream>

using namespace std;

constexpr int MAX_N = 20;

bool isSafe(int board[MAX_N][MAX_N], const int row, const int col, const int N) {
    for (int i = 0; i < row; i++) {
        if (board[i][col] == 1)
            return false;
    }
    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
        if (board[i][j] == 1)
            return false;
    }
    for (int i = row - 1, j = col + 1; i >= 0 && j < N; i--, j++) {
        if (board[i][j] == 1)
            return false;
    }
    return true;
}

void solve(int board[MAX_N][MAX_N], const int row, const int N) {
    if (row == N) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << board[i][j] << " ";
            }
        }
        cout << endl;
        return;
    }

    for (int col = 0; col < N; col++) {
        if (isSafe(board, row, col, N)) {
            board[row][col] = 1;
            solve(board, row + 1, N);
            board[row][col] = 0;
        }
    }
}

int main(const int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <N>" << endl;
        return 1;
    }

    char* endptr;
    errno = 0;
    const long val = strtol(argv[1], &endptr, 10);

    if (endptr == argv[1] || *endptr != '\0' || errno == ERANGE) {
        cerr << "Ошибка: неверное числовое значение для N: " << argv[1] << endl;
        return 1;
    }

    const int N = static_cast<int>(val);
    if (N <= 0 || N > MAX_N) {
        cerr << "Ошибка: N должно быть в диапазоне от 1 до " << MAX_N << endl;
        return 1;
    }

    int board[MAX_N][MAX_N] = {};

    solve(board, 0, N);
    return 0;
}
