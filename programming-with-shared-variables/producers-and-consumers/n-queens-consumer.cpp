// programming-with-shared-variables/producers-and-consumers/n-queens-producer.cpp

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <sys/stat.h>
#include "stb_image_write.h"
#define MKDIR(path) mkdir(path, 0777)

#ifndef PRODUCER_PATH
#error "PRODUCER_PATH не определён! Используйте CMake для сборки."
#endif

using namespace std;

constexpr int MAX_N = 20;
char outputDir[256];

void printQueenPositions(int board[MAX_N][MAX_N], const int N, const int solutionNum) {
    cout << solutionNum << ": ";
    bool first = true;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == 1) {
                if (!first)
                    cout << ", ";
                cout << "(" << i << ", " << j << ")";
                first = false;
                break;
            }
        }
    }
    cout << endl;
}

void saveBoardAsImage(int board[MAX_N][MAX_N], const int N, const int solutionNum) {
    constexpr int size = 256;
    constexpr int colors = 3;
    auto *pixels = new unsigned char[size * size * colors];

    const unsigned char white[3] = {255, 255, 255};
    const unsigned char black[3] = {0, 0, 0};
    const unsigned char queen[3] = {255, 0, 0};

    for (int y = 0; y < size; y++) {
        const int r = y * N / size;
        for (int x = 0; x < size; x++) {
            const int c = x * N / size;
            const unsigned char *color;

            if (board[r][c] == 1) {
                color = queen;
            } else {
                color = (r + c) % 2 == 0 ? white : black;
            }

            const int idx = (y * size + x) * colors;
            pixels[idx + 0] = color[0];
            pixels[idx + 1] = color[1];
            pixels[idx + 2] = color[2];
        }
    }

    char filename[512];
    sprintf(filename, "%s/solution_%04d.png", outputDir, solutionNum);

    if (!stbi_write_png(filename, size, size, colors, pixels, size * colors)) {
        cerr << "   Ошибка сохранения: " << filename << endl;
    }

    delete[] pixels;
}

int main() {
    int N;
    cout << "Введите размер доски N: ";
    cin >> N;

    if (N <= 0 || N > MAX_N) {
        cerr << "N должно быть в диапазоне от 1 до " << MAX_N << endl;
        return 1;
    }

    MKDIR("./solves");
    sprintf(outputDir, "./solves/%d", N);
    MKDIR(outputDir);

    const string command = string(PRODUCER_PATH) + " " + to_string(N);
    cout << "Запуск дочернего процесса: " << command << endl;

    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        cerr << "Не удалось запустить producer!" << endl;
        return 1;
    }

    int board[MAX_N][MAX_N];
    int solutionCount = 0;

    char lineBuffer[4096];
    while (fgets(lineBuffer, sizeof(lineBuffer), pipe) != nullptr) {
        stringstream ss(lineBuffer);
        bool boardReadSuccessfully = true;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (!(ss >> board[i][j])) {
                    cerr << "Ошибка: некорректные данные из producer." << endl;
                    boardReadSuccessfully = false;
                    break;
                }
            }
            if (!boardReadSuccessfully)
                break;
        }

        if (boardReadSuccessfully) {
            solutionCount++;
            printQueenPositions(board, N, solutionCount);
            saveBoardAsImage(board, N, solutionCount);
        }
    }

    pclose(pipe);

    cout << "Найдено решений: " << solutionCount << endl;

    return 0;
}
