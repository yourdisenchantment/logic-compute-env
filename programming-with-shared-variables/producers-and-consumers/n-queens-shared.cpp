// ./programming-with-shared-variables/producers-and-consumers/n-queens-shared-threaded.cpp

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

// подключение stb_image_write для сохранения PNG
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// создание папок для сохранения картинок решений
#include <sys/stat.h> // mkdir
#define MKDIR(path) mkdir(path, 0777)

constexpr int MAX_N = 20;
char outputDir[256];

bool isSafe(int board[MAX_N][MAX_N], int row, int col, int N);
void printQueenPositions(int board[MAX_N][MAX_N], int N);
void saveBoardAsImage(int board[MAX_N][MAX_N], int N);
void solve(int board[MAX_N][MAX_N], int row, int N);

// Класс для передачи конфигурации доски между потоками
class SharedBoard {
    int board[MAX_N][MAX_N]{};
    int boardSize;
    bool hasData;
    bool finished;
    mutex mtx;
    condition_variable cv;

public:
    SharedBoard() : boardSize(0), hasData(false), finished(false) {
        // Инициализация доски
        for (auto& i : board) {
            for (int& j : i) {
                j = 0;
            }
        }
    }

    // Производитель кладет найденную конфигурацию
    void putBoard(int srcBoard[MAX_N][MAX_N], const int N) {
        unique_lock lock(mtx);

        // ВАЖНО: производитель ждет, пока потребитель заберет предыдущую доску
        while (hasData) {
            cv.wait(lock);
        }

        // Копируем доску
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                board[i][j] = srcBoard[i][j];
            }
        }

        boardSize = N;
        hasData = true;
        cv.notify_one(); // уведомляем потребителя
    }

    // Потребитель забирает конфигурацию
    bool getBoard(int dstBoard[MAX_N][MAX_N], int& N) {
        unique_lock lock(mtx);

        // Ждем, пока появятся данные или производитель завершится
        while (!hasData && !finished) {
            cv.wait(lock);
        }

        // Если есть данные - обрабатываем их
        if (hasData) {
            // Копируем доску
            for (int i = 0; i < boardSize; i++) {
                for (int j = 0; j < boardSize; j++) {
                    dstBoard[i][j] = board[i][j];
                }
            }

            N = boardSize;
            hasData = false;
            cv.notify_one();
            return true;
        }

        // Иначе (finished == true && hasData == false) - работа завершена
        return false;
    }

    // Производитель сигнализирует о завершении работы
    void setFinished() {
        unique_lock lock(mtx);
        finished = true;
        cv.notify_one();
    }
};

bool isSafe(int board[MAX_N][MAX_N], const int row, const int col, const int N) {
    for (int i = 0; i < row; i++) {
        if (board[i][col] == 1) {
            return false;
        }
    }

    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
        if (board[i][j] == 1) {
            return false;
        }
    }

    for (int i = row - 1, j = col + 1; i >= 0 && j < N; i--, j++) {
        if (board[i][j] == 1) {
            return false;
        }
    }

    return true;
}

void printQueenPositions(int board[MAX_N][MAX_N], const int N, const int solutionNum) {
    cout << solutionNum << ": ";
    bool first = true;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == 1) {
                if (!first) {
                    cout << ", ";
                }
                cout << "(" << i << ", " << j << ")";
                first = false;
                break;
            }
        }
    }

    cout << endl;
    cout.flush();
}

void saveBoardAsImage(int board[MAX_N][MAX_N], const int N, const int solutionNum) {
    constexpr int size = 256;
    constexpr int colors = 3;
    auto* pixels = new unsigned char[size * size * colors];

    const unsigned char white[3] = {255, 255, 255};
    const unsigned char black[3] = {0, 0, 0};
    const unsigned char queen[3] = {255, 0, 0};

    for (int y = 0; y < size; y++) {
        const int r = y * N / size;

        for (int x = 0; x < size; x++) {
            const int c = x * N / size;
            const unsigned char* color;

            if (board[r][c] == 1) {
                color = queen;
            } else {
                color = ((r + c) % 2 == 0) ? white : black;
            }

            const int idx = (y * size + x) * colors;
            pixels[idx + 0] = color[0];
            pixels[idx + 1] = color[1];
            pixels[idx + 2] = color[2];
        }
    }

    char filename[512];
    sprintf(filename, "%s/solution_%04d.png", outputDir, solutionNum);

    if (stbi_write_png(filename, size, size, colors, pixels, size * colors)) {
        cout << "   Сохранено: " << filename << endl;
    } else {
        cout << "   Ошибка сохранения: " << filename << endl;
    }

    delete[] pixels;
}

// Рекурсивный поиск решений (производитель)
void solve(int board[MAX_N][MAX_N], const int row, const int N, SharedBoard& sharedData) {
    if (row == N) {
        // Найдено решение - отправляем потребителю
        // Производитель ЗАСНЁТ здесь, если потребитель ещё не забрал предыдущую доску
        sharedData.putBoard(board, N);
        return;
    }

    for (int col = 0; col < N; col++) {
        if (isSafe(board, row, col, N)) {
            board[row][col] = 1;
            solve(board, row + 1, N, sharedData);
            board[row][col] = 0;
        }
    }
}

int main() {
    int N;

    cout << "Введите размер доски N: ";
    cin >> N;

    MKDIR("./solves");
    sprintf(outputDir, "./solves/%d", N);
    MKDIR(outputDir);

    SharedBoard sharedData;
    int solutionCount = 0;

    // Поток-производитель: ищет решения
    thread producer([&] {
        int board[MAX_N][MAX_N];

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                board[i][j] = 0;
            }
        }

        solve(board, 0, N, sharedData);
        sharedData.setFinished(); // сигнализируем о завершении поиска
    });

    // Поток-потребитель: выводит и сохраняет решения
    thread consumer([&] {
        int board[MAX_N][MAX_N];
        int boardSize;

        while (sharedData.getBoard(board, boardSize)) {
            solutionCount++;
            printQueenPositions(board, boardSize, solutionCount);
            saveBoardAsImage(board, boardSize, solutionCount);
        }
    });

    producer.join();
    consumer.join();

    cout << "Найдено решений: " << solutionCount << endl;

    return 0;
}
