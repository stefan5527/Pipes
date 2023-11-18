#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

bool is_prime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

int main() {
    int n = 10;
    std::vector<int> pipes(n);
    for (int i = 0; i < n; ++i) {
        int fd[2];
        pipe(fd);
        pipes[i] = fd[0];
        if (fork() == 0) {
            close(fd[0]);
            for (int j = i * 1000; j < (i + 1) * 1000; ++j)
                if (is_prime(j)) write(fd[1], &j, sizeof(int));
            close(fd[1]);
            exit(0);
        }
        close(fd[1]);
    }
    for (int i = 0; i < n; ++i) {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            int prime;
            while (read(pipes[i], &prime, sizeof(int)) > 0)
                std::cout << prime << std::endl;
            close(pipes[i]);
        }
    }
    return 0;
}