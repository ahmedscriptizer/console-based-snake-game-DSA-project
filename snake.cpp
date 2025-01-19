#include <bits/stdc++.h>
#include <conio.h>   // For keyboard input
#include <windows.h> // For console manipulation

using namespace std;

// Directions
const char DIR_UP = 'U';
const char DIR_DOWN = 'D';
const char DIR_LEFT = 'L';
const char DIR_RIGHT = 'R';

int consoleWidth, consoleHeight;

// Function to initialize screen dimensions
void initScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

// Point structure to represent positions on the board
struct Point {
    int xCoord;
    int yCoord;
    Point() {}
    Point(int x, int y) : xCoord(x), yCoord(y) {}
};

// Node structure for the snake's body (linked list)
struct Node {
    Point data;
    Node* next;
    Node(Point p) : data(p), next(nullptr) {}
};

// Snake class using a linked list
class Snake {
    Node* head; // Head of the snake
    Node* tail; // Tail of the snake
    char direction;
    int length;

public:
    Snake(int x, int y) {
        head = new Node(Point(x, y));
        tail = head;
        direction = DIR_RIGHT;
        length = 1;
    }

    ~Snake() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    int getLength() {
        return length;
    }

    void changeDirection(char newDirection) {
        if ((newDirection == DIR_UP && direction != DIR_DOWN) ||
            (newDirection == DIR_DOWN && direction != DIR_UP) ||
            (newDirection == DIR_LEFT && direction != DIR_RIGHT) ||
            (newDirection == DIR_RIGHT && direction != DIR_LEFT)) {
            direction = newDirection;
        }
    }

    bool move(Point food, vector<Point>& obstacles) {
        // Compute the new head position
        int newX = head->data.xCoord;
        int newY = head->data.yCoord;
        switch (direction) {
            case DIR_UP:    newY--; break;
            case DIR_DOWN:  newY++; break;
            case DIR_LEFT:  newX--; break;
            case DIR_RIGHT: newX++; break;
        }

        // Check if the snake bites itself
        for (Node* current = head; current != nullptr; current = current->next) {
            if (current->data.xCoord == newX && current->data.yCoord == newY) {
                return false;
            }
        }

        // Check if the snake hits an obstacle
        for (Point obstacle : obstacles) {
            if (obstacle.xCoord == newX && obstacle.yCoord == newY) {
                return false;
            }
        }

        // Add the new head
        Node* newHead = new Node(Point(newX, newY));
        newHead->next = head;
        head = newHead;

        // Check if the snake eats the food
        if (food.xCoord == newX && food.yCoord == newY) {
            length++;
        } else {
            // Remove the tail
            Node* temp = head;
            while (temp->next != tail) {
                temp = temp->next;
            }
            delete tail;
            tail = temp;
            tail->next = nullptr;
        }

        return true;
    }

    Node* getBody() {
        return head;
    }
};

// Board class to manage the game environment
class Board {
    Snake* snake;
    const char SNAKE_BODY = 'O';
    Point food;
    const char FOOD = 'o';
    vector<Point> obstacles; // List of obstacles
    const char OBSTACLE = '#';
    int score;
    int level;
    int baseSpeed;
    int mode; // Mode chosen by the user

public:
    Board(int userMode) : mode(userMode) {
        spawnFood();
        snake = new Snake(10, 10);
        score = 0;
        level = 1;
        baseSpeed = 100; // Initial speed
        if (mode == 4 || mode == 5) {
            spawnObstacles(level); // Initial obstacles for modes with obstacles
        }
    }

    ~Board() {
        delete snake;
    }

    int getScore() {
        return score;
    }

    void spawnFood() {
        food = Point(rand() % consoleWidth, rand() % consoleHeight);
    }

    void spawnObstacles(int count) {
        obstacles.clear();
        for (int i = 0; i < count; i++) {
            obstacles.push_back(Point(rand() % consoleWidth, rand() % consoleHeight));
        }
    }

    void displayInfo() {
        gotoxy(0, 0);
        cout << "Score: " << score << " | Level: " << level;
    }

    void gotoxy(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void draw() {
        system("cls");

        // Draw the snake
        for (Node* current = snake->getBody(); current != nullptr; current = current->next) {
            gotoxy(current->data.xCoord, current->data.yCoord);
            cout << SNAKE_BODY;
        }

        // Draw the food
        gotoxy(food.xCoord, food.yCoord);
        cout << FOOD;

        // Draw the obstacles
        for (Point obstacle : obstacles) {
            gotoxy(obstacle.xCoord, obstacle.yCoord);
            cout << OBSTACLE;
        }

        displayInfo();
    }

    bool update() {
        bool isAlive = snake->move(food, obstacles);
        if (!isAlive) return false;

        // Check if food is eaten
        if (snake->getBody()->data.xCoord == food.xCoord &&
            snake->getBody()->data.yCoord == food.yCoord) {
            score++;
            spawnFood();

            // Update based on mode
            if (mode == 2 || mode == 5) {
                if (score % 3 == 0) {
                    baseSpeed = max(50, baseSpeed - 10); // Increase speed
                }
            }
            if (mode == 4 || mode == 5) {
                if (score % 3 == 0) {
                    level++;
                    spawnObstacles(level); // Add more obstacles
                }
            }
        }
        return true;
    }

    void getInput() {
        if (kbhit()) {
            char key = getch();
            if (key == 'w' || key == 'W') {
                snake->changeDirection(DIR_UP);
            } else if (key == 'a' || key == 'A') {
                snake->changeDirection(DIR_LEFT);
            } else if (key == 's' || key == 'S') {
                snake->changeDirection(DIR_DOWN);
            } else if (key == 'd' || key == 'D') {
                snake->changeDirection(DIR_RIGHT);
            }
        }
    }

    int getSpeed() {
        return baseSpeed;
    }
};

// Main function
void displayMenu() {
    cout << "Snake Game Modes:\n";
    cout << "1. Simple Level\n";
    cout << "2. Speed increases after every 3 scores\n";
    cout << "3. Obstacles increase after every 3 scores\n";
    cout << "4. Both obstacles and speed increase\n";
    cout << "0. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    srand(time(0));
    initScreen();

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;

        if (choice == 0) {
            cout << "Exiting the game. Goodbye!\n";
            break;
        }

        if (choice < 1 || choice > 4) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        Board* board = new Board(choice);

        while (board->update()) {
            board->getInput();
            board->draw();
            Sleep(board->getSpeed());
        }

        system("cls");
        cout << "Game Over\n";
        cout << "Final Score: " << board->getScore() << endl;

        delete board;
    }

    return 0;
}
