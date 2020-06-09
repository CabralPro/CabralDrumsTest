# Build and run
make buildRun

# Build
make build

# Run
make Run

# Compile comand
g++ -Wall -Wextra -std=c++17 -pthread -ggdb -Iinclude -Llib main.cpp -o bin/main -lncurses -lasound

