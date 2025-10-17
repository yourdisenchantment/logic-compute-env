
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <math.h>
#include <mutex>
#include <queue>
#include <thread>
using namespace std;

class Task {
public:
    double l, r, eps;
    void solve();
};

class QueueClosedException {};

class BlockingQueue {
    bool closed;
    queue<Task> q;
    mutex mx;
    condition_variable cv_empty;

public:
    BlockingQueue() {
        closed = false;
    }

    void close() {
        closed = true;

        cv_empty.notify_all();
    }

    void push(Task task) {
        unique_lock lk(mx);
        q.push(task);

        cv_empty.notify_one();
    }

    Task get() {
        unique_lock lk(mx);

        // wait for !q.empty()
        this->cv_empty.wait(lk, [this] { return !q.empty() || closed; });

        if (closed)
            throw QueueClosedException();

        Task t = q.front();
        q.pop();

        return t;
    }
};

class ThreadPool {
    int threadCount;
    BlockingQueue tasks;
    vector<thread> threads;

public:
    ThreadPool(int threadCount);

    void addTask(Task task);
    Task getTask();
    void close();
};

void threadPoolFunction(ThreadPool* pool) {
    try {
        while (true) {
            Task task = pool->getTask();
            task.solve();
        }
    } catch (QueueClosedException& e) {
    }
}

ThreadPool::ThreadPool(int threadCount) : threadCount(threadCount), threads(threadCount) {
    for (int i = 0; i < this->threadCount; i++)
        this->threads[i] = thread(threadPoolFunction, this);
}

void ThreadPool::addTask(Task task) {
    tasks.push(task);
}

Task ThreadPool::getTask() {
    return tasks.get();
}

void ThreadPool::close() {
    tasks.close();

    for (int i = 0; i < this->threadCount; i++)
        this->threads[i].join();
}

ThreadPool pool(8);

mutex answer_mutex;
double answer = 0.0;

condition_variable wait_parts;
mutex parts_mutex;
int parts;

double f(double x) {
    return sin(x);
}

void Task::solve() {
    double area = (f(this->l) + f(this->r)) / 2.0 * (this->r - this->l);

    double m = (this->l + this->r) / 2;
    double area2 = ((f(this->l) + 2 * f(m) + f(this->r)) * (this->r - this->l) / 4.0);

    if (fabs(area - area2) > this->eps) {
        Task t1, t2;
        t1.l = this->l;
        t1.r = m;
        t1.eps = this->eps;

        t2.l = m;
        t2.r = this->r;
        t2.eps = this->eps;

        {
            unique_lock lk(parts_mutex);
            parts += 2;
        }

        pool.addTask(t1);
        pool.addTask(t2);
    } else {
        {
            unique_lock lk(answer_mutex);
            answer += area2;
        }
    }

    {
        unique_lock lk(parts_mutex);
        parts -= 1;
        wait_parts.notify_one();
    }
}

int main() {
    Task start;
    start.l = 0;
    start.r = 1;
    start.eps = 1e-7;
    parts = 1;

    pool.addTask(start);

    {
        unique_lock lk(parts_mutex);
        wait_parts.wait(lk, [] { return parts == 0; });
    }

    printf("%.10lf\n", answer);

    pool.close();
}
