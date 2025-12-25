// programming-with-shared-variables/producers-and-consumers/n-queens.cpp

#include <iostream>

using namespace std;

// подключение stb_image_write для сохранения PNG
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// создание папок для сохранения картинок решений
#include <sys/stat.h> // mkdir
#define MKDIR(path) mkdir(path, 0777)

constexpr int MAX_N = 20;
int solutionCount = 0;
char outputDir[256];

bool isSafe(int board[MAX_N][MAX_N], int row, int col, int N);

void printQueenPositions(int board[MAX_N][MAX_N], int N);

void saveBoardAsImage(int board[MAX_N][MAX_N], int N);

void solve(int board[MAX_N][MAX_N], int row, int N);

/*
 * Функция: isSafe
 * Назначение: проверяет, безопасно ли поставить ферзя в клетку (row, col)
 * Аргументы:
 *   board - матрица доски NxN (1 - ферзь, 0 - пусто)
 *   row   - строка, куда пытаемся поставить ферзя
 *   col   - столбец, куда пытаемся поставить ферзя
 *   N     - размер доски
 * Возвращает:
 *   true, если в этой клетке нет конфликтов по столбцу и двум верхним диагоналям
 */
bool isSafe(int board[MAX_N][MAX_N], const int row, const int col, const int N) {
    for (int i = 0; i < row; i++) {
        // идем по строкам выше текущей
        if (board[i][col] == 1) {
            // если в том же столбце уже есть ферзь
            return false; // то клетка небезопасна
        }
    }

    for (int i = row - 1, j = col - 1; // стартуем слева-сверху от (row, col)
         i >= 0 && j >= 0; // пока не вышли за границы
         i--, j--) {
        // двигаемся по диагонали влево-вверх
        if (board[i][j] == 1) {
            // если нашли ферзя на диагонали
            return false; // клетка небезопасна
        }
    }

    for (int i = row - 1, j = col + 1; // стартуем справа-сверху от (row, col)
         i >= 0 && j < N; // пока не вышли за границы
         i--, j++) {
        // двигаемся по диагонали вправо-вверх
        if (board[i][j] == 1) {
            // если нашли ферзя на диагонали
            return false; // клетка небезопасна
        }
    }

    return true; // конфликтов нет - клетка безопасна
}


/*
 * Функция: printQueenPositions
 * Назначение: печатает координаты ферзей найденного решения в формате:
 *   k: (i0, j0), (i1, j1), ... (все в одной строке)
 * Аргументы:
 *   board - матрица доски
 *   N     - размер доски
 */
void printQueenPositions(int board[MAX_N][MAX_N], const int N) {
    cout << solutionCount << ": "; // печатаем номер решения и двоеточие
    bool first = true; // флаг для расстановки запятых между координатами

    for (int i = 0; i < N; i++) {
        // идем по строкам
        for (int j = 0; j < N; j++) {
            // идем по столбцам
            if (board[i][j] == 1) {
                // если в клетке (i, j) стоит ферзь
                if (!first) {
                    cout << ", "; // если это не первая пара - добавляем запятую и пробел
                }

                cout << "(" << i << ", " << j << ")"; // печатаем пару (i, j)
                first = false; // отмечаем, что первая пара уже выведена
                break; // в строке ровно один ферзь - идем к следующей строке
            }
        }
    }

    cout << endl;
    cout.flush(); // очистить буфер, чтобы вывод было "по мере готовности"
}


/*
 * Функция: saveBoardAsImage
 * Назначение: сохраняет текущее решение как PNG-картинку в папку outputDir
 * Имя файла: solution_0001.png, solution_0002.png, ...
 * Аргументы:
 *   board - матрица доски
 *   N     - размер доски
 */
void saveBoardAsImage(int board[MAX_N][MAX_N], const int N) {
    constexpr int size = 256;
    constexpr int colors = 3;
    auto *pixels = new unsigned char[size * size * colors]; // буфер пикселей

    const unsigned char white[3] = {255, 255, 255};
    const unsigned char black[3] = {0, 0, 0};
    const unsigned char queen[3] = {255, 0, 0};

    for (int y = 0; y < size; y++) {
        const int r = y * N / size; // номер строки клетки на доске

        for (int x = 0; x < size; x++) {
            const int c = x * N / size; // номер столбца клетки на доске
            const unsigned char *color; // указатель на выбранный цвет

            if (board[r][c] == 1) {
                // если в этой клетке ферзь
                color = queen;
            } else {
                color = ((r + c) % 2 == 0) ? white : black; // иначе клетка бел/чер
            }

            const int idx = (y * size + x) * colors; // индекс пикселя в буфере
            pixels[idx + 0] = color[0];
            pixels[idx + 1] = color[1];
            pixels[idx + 2] = color[2];
        }
    }

    char filename[512]; // буфер имени файла
    sprintf(filename, "%s/solution_%04d.png", outputDir, solutionCount);

    if (stbi_write_png(filename, size, size, colors, pixels, size * colors)) {
        cout << "   Сохранено: " << filename << endl;
    } else {
        cout << "   Ошибка сохранения: " << filename << endl;
    }

    delete[] pixels; // освободить память
}


/*
 * Функция: solve
 * Назначение: рекурсивно расставляет ферзей по строкам, фиксируя все корректные решения
 * Аргументы:
 *   board - матрица доски
 *   row   - текущая строка, в которую пытаемся поставить ферзя
 *   N     - размер доски
 */
void solve(int board[MAX_N][MAX_N], const int row, const int N) {
    if (row == N) {
        // если расставили ферзей во все N строк
        solutionCount++; // увеличиваем номер решения
        printQueenPositions(board, N); // печатаем координаты ферзей
        saveBoardAsImage(board, N); // сохраняем картинку решения
        return; // возвращаемся, чтобы искать остальные решения
    }

    for (int col = 0; col < N; col++) {
        // перебираем все столбцы в текущей строке
        if (isSafe(board, row, col, N)) {
            // если можно поставить ферзя в (row, col)
            board[row][col] = 1; // ставим ферзя
            solve(board, row + 1, N); // рекурсивно идем в следующую строку
            board[row][col] = 0; // снимаем ферзя (откат) и пробуем следующий столбец
        }
    }
}


/*
 * Функция: main
 * Назначение: читает N, по мере нахождения печатает решения, сохраняет PNG и завершает работу
 */
int main() {
    int N;

    cout << "Введите размер доски N: ";
    cin >> N;

    MKDIR("./solves");
    sprintf(outputDir, "./solves/%d", N);
    MKDIR(outputDir);


    // Инициализируем доску с запасом, что бы на этапе компиляции не родить
    int board[MAX_N][MAX_N];

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            board[i][j] = 0;
        }
    }

    solve(board, 0, N);
    cout << "Найдено решений: " << solutionCount << endl;

    return 0;
}
