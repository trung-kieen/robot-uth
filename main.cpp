#include <algorithm>
#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <unistd.h>

// Refresh screen frame
#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

#define DELAY_SECOND 1
#define END_PATH -1
// IN CLOCKWISE FOR MOVE VALUES
#define MOVE_UP 0
#define MOVE_RIGHT 1
#define MOVE_DOWN 2
#define MOVE_LEFT 3

// Rule for game mode
#define SINGLE 1
#define PAIR_NOT_OVERRIDE 2
#define PAIR_ALLOWED_OVERRIDE 3
/*
Authur: Nguyen Khac Trung Kien
Email: trungkien@gmail.com
Github-name:trung-kieen
________________________________
Description of this file.
*/
class Robot {
private:
  int x;
  int y;
  int rows;
  int cols;
  std::vector<int> point;
  // in MODE 2 used variable visited in main function.
  std::vector<std::vector<bool>> visited;
  int sign;
  std::string name;

public:
  Robot(int initialX, int initialY, char initialSign, int numRows, int numCols,
        std::vector<std::vector<int>> &matrixInput,
        std::vector<std::vector<bool>> &mainVisitedMap,
        std::string initialName) {
    x = initialX;
    y = initialY;
    name = initialName;
    rows = numRows;
    cols = numCols;
    sign = initialSign;
    visited.resize(numRows, std::vector<bool>(numCols));

    // Set default values as not visited all square
    for (int i = 0; i < numRows; i++) {
      for (int j = 0; j < numCols; j++) {
        visited[i][j] = false;
      }
    }
    point.push_back(matrixInput[x][y]);
    visited[x][y] = true;
    mainVisitedMap[x][y] = true;
  }

  void move(int direction, std::vector<std::vector<int>> &matrixInput,
            std::vector<std::vector<bool>> &mainVisitedMap) {
    if (direction == END_PATH) {

      std::cout << "Robot " << name << ":\n";
      showPoints();
      drawMapVisited(matrixInput);
      return;
    }

    switch (direction) {
    case MOVE_UP:
      // Because read matrix up to down so it reverse how to move up and down
      x += -1;
      break;
    case MOVE_DOWN:
      x += 1;
      break;
    case MOVE_LEFT:
      y += -1;
      break;
    case MOVE_RIGHT:
      y += 1;
      break;
    default:
      // Handle default case if necessary
      std::cout << "ERROR: unknown direction " << direction << std::endl;
      exit(3);
      break;
    }

    // Update point base on new position
    point.push_back(matrixInput[x][y]);
    visited[x][y] = true;
    mainVisitedMap[x][y] = true;
    std::cout << "Robot " << name << ":\n";
    showPoints();
    drawMapVisited(matrixInput);
  }
  bool isPossibleMove(int moveDirection, int mode,
                      std::vector<std::vector<bool>> &mainVisitedMap) {

    int newX = x;
    int newY = y;
    switch (moveDirection) {
    case MOVE_UP:
      newX += -1;
      break;
    case MOVE_DOWN:
      newX += 1;
      break;
    case MOVE_LEFT:
      newY += -1;
      break;
    case MOVE_RIGHT:
      newY += 1;
      break;
    default:
      std::cout << "ERROR: Unknown move direction " << moveDirection << '\n';
      exit(2);
      break;
    }
    // Check if not overflow the range of map
    // Keep in mind that array is zero base so ranger from 0 to n - 1
    bool isOverFlow =
        (newX < 0 || newX > rows - 1 || newY < 0 || newY > cols - 1) ? true
                                                                     : false;
    if (isOverFlow) {
      return false;
    }

    // Check not moved to old  position
    if (mode == SINGLE || mode == PAIR_ALLOWED_OVERRIDE) {
      if (visited[newX][newY])
        return false;
    }
    // In mode 2 not moved to other robot moved
    else if (mode == PAIR_NOT_OVERRIDE) {
      if (mainVisitedMap[newX][newY])
        return false;
    }

    return true;
  }
  int getNumPossiblePaths(int mode,
                          std::vector<std::vector<bool>> &mainVisitedMap) {
    int counter = 0;
    // Irrerate over 4 direction mode (see define directive on top of file)
    for (int moveDirection = 0; moveDirection < 4; moveDirection++) {
      if (this->isPossibleMove(moveDirection, mode, mainVisitedMap))
        counter++;
    }
    return counter;
  }
  int rankPointMovement(int moveDirection,
                        std::vector<std::vector<int>> &inputMatrix) {
    // For build rank which move direction get heightest point.

    int newX = x;
    int newY = y;
    int result;
    switch (moveDirection) {
    case MOVE_UP:
      newX += -1;
      break;
    case MOVE_DOWN:
      newX += 1;
      break;
    case MOVE_LEFT:
      newY += -1;
      break;
    case MOVE_RIGHT:
      newY += 1;
      break;
    default:
      std::cout << "ERROR: Unknown move direction " << moveDirection << '\n';
      exit(2);
      break;
    }
    bool isOverFlow =
        (newX < 0 || newX > rows - 1 || newY < 0 || newY > cols - 1) ? true
                                                                     : false;
    if (isOverFlow) {
      // Just return some value because in the function determineMove will know
      // and treat for this case
      return 0;
    }

    result = inputMatrix[newX][newY];
    return result;
  }
  int determineMove(int mode, std::vector<std::vector<bool>> &mainVisitedMap,
                    std::vector<std::vector<int>> &inputMatrix) {
    // Rank path and check if possible to move then return the best move
    //  direction.
    // If number of possible path is use direct.
    if (getNumPossiblePaths(mode, mainVisitedMap) == 0) {
      return END_PATH;
    }
    int highestMovePoint;
    int bestMoveIndex;

    for (int moveDirection = 0; moveDirection < 4; moveDirection++) {
      if (this->isPossibleMove(moveDirection, mode, mainVisitedMap)) {
        highestMovePoint = this->rankPointMovement(moveDirection, inputMatrix);
        bestMoveIndex = moveDirection;
      }
    }
    for (int moveDirection = 0; moveDirection < 4; moveDirection++) {
      if (this->rankPointMovement(moveDirection, inputMatrix) >=
              highestMovePoint &&
          this->isPossibleMove(moveDirection, mode, mainVisitedMap)) {
        bestMoveIndex = moveDirection;
        highestMovePoint =
            (this->rankPointMovement(moveDirection, inputMatrix));
      }
    }
    return bestMoveIndex;
  }
  void writeToFile(std::string fileName) {
    // Write append to file
    std::ofstream outputFile(fileName, std::ios::app);
    if (outputFile.is_open()) {
      outputFile << '\n' << point.size() << '\n';
      for (int i = 0; i < point.size(); i++) {
        outputFile << point[i] << '\t';
      }
    }
    outputFile.close();
  }
  int getTotalPoint() {
    // For print result
    int sum = 0;
    for (int i = 0; i < point.size(); i++)
      sum += point[i];
    return sum;
  }
  void drawMapVisited(std::vector<std::vector<int>> &matrixInput) {
    // Sign is the character repersent robot path
    std::cout << '\n';
    for (int i = 0; i < cols; i++) {
      std::cout << "----------------";
    }
    std::cout << "-\n";
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        if (j == 0) {
          std::cout << "|\t";
        }
        if (visited[i][j]) {
          std::cout << (char)sign << "\t|\t";
        } else {
          std::cout << matrixInput[i][j] << "\t|\t";
        }
      }
      std::cout << '\n';
      for (int i = 0; i < cols; i++) {
        std::cout << "----------------";
      }
      std::cout << "-\n";
    }
  }
  void showPoints() {
    std::cout << "Points: ";
    for (int i = 0; i < point.size(); i++) {
      if (i == 0) {
        std::cout << point[i];
      } else {
        std::cout << ">" << point[i];
      }
    }
    std::cout << '\n';
  }
  std::string getName() { return name; }
};

void inputMatrix(std::vector<std::vector<int>> &matrixInput, int numRows,
                 int numCols, std::ifstream &inputFile) {
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      inputFile >> matrixInput[i][j];
    }
  }
}
void drawMap(std::vector<std::vector<int>> matrix, int rows, int cols) {
  std::cout << '\n';
  for (int i = 0; i < cols; i++) {
    std::cout << "----------------";
  }
  std::cout << '\n';
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (j == 0) {
        std::cout << "|\t";
      }
      std::cout << matrix[i][j] << "\t|\t";
    }
    std::cout << '\n';
    for (int i = 0; i < cols; i++) {
      std::cout << "----------------";
    }
    std::cout << '\n';
  }
}
void clearConsole() { system(CLEAR_SCREEN); }

int main() {
  clearConsole();
  std::cout << "Hello World!\n";
  // Load file input to memory
  std::ifstream inputFile("input.txt");
  if (!inputFile) {
    std::cerr << "Can't open input file";
    return 1;
  }
  int numRows, numCols;
  inputFile >> numRows >> numCols;
  std::vector<std::vector<int>> matrixInput(numRows, std::vector<int>(numCols));
  inputMatrix(matrixInput, numRows, numCols, inputFile);
  inputFile.close();
  // Setup base
  std::vector<std::vector<bool>> mainVisitedMap(numRows,
                                                std::vector<bool>(numCols));
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      mainVisitedMap[i][j] = false;
    }
  }
  std::cout << "Game mode\n";
  std::cout << "- Mode 1: single\n";
  std::cout << "- Mode 2: pair of robot, each robot CAN NOT move to other "
               "robot path\n";
  std::cout
      << "- Mode 3: pair of robot, each robot CAN move to other robot path\n";
  std::cout << "Choose mode to play (number): ";
  int mode;
  std::cin >> mode;
  if (!(mode >= 0 && mode <= 3)) {
    std::cout << "Mode not supported please input number from 1 to 3\n";
    std::cout << "Choose mode to play (number): ";
    std::cin >> mode;
  }

  drawMap(matrixInput, numRows, numCols);
  int x1 = -1;
  int y1 = -1;
  int x2 = -1;
  int y2 = -1;
  std::cout << "Map is is " << numRows << 'x' << numCols << '\n';
  std::cout << "=> Ranger of x: 0-" << numRows - 1 << '\n';
  std::cout << "=> Ranger of y: 0-" << numCols - 1 << '\n';
  while (!(x1 >= 0 && x1 <= numRows - 1 && y1 >= 0 && y1 <= numCols - 1)) {
    if (mode == SINGLE) {
      std::cout << "Input initial position for robot (x, y): ";
    } else {
      std::cout << "Input initial position for robot A (x, y): ";
    }
    std::cin >> x1 >> y1;
  }
  Robot firstRobot(x1, y1, 'x', numRows, numCols, matrixInput, mainVisitedMap,
                   "A");

  if (mode == PAIR_ALLOWED_OVERRIDE || mode == PAIR_NOT_OVERRIDE) {
    while (!(x2 >= 0 && x2 <= numRows - 1 && y2 >= 0 && y2 <= numCols - 1)) {
      std::cout << "Input initial position for robot B (x, y): ";
      std::cin >> x2 >> y2;
    }
  } else {
    x2 = 0;
    y2 = 0;
  }
  Robot secondRobot(x2, y2, 'o', numRows, numCols, matrixInput, mainVisitedMap,
                    "B");

  if (mode == SINGLE) {
    while (firstRobot.getNumPossiblePaths(mode, mainVisitedMap) > 0) {
      clearConsole();
      // Determind the best move then move to next square unitill no more path
      // to go.
      firstRobot.move(
          firstRobot.determineMove(mode, mainVisitedMap, matrixInput),
          matrixInput, mainVisitedMap);
      sleep(DELAY_SECOND);
    }
    std::cout << "Robot total points: " << firstRobot.getTotalPoint() << '\n';
    firstRobot.writeToFile("output.txt");
  }

  if (mode == PAIR_ALLOWED_OVERRIDE || mode == PAIR_NOT_OVERRIDE) {
    while (firstRobot.getNumPossiblePaths(mode, mainVisitedMap) > 0 ||
           secondRobot.getNumPossiblePaths(mode, mainVisitedMap) > 0) {
      clearConsole();
      firstRobot.move(
          firstRobot.determineMove(mode, mainVisitedMap, matrixInput),
          matrixInput, mainVisitedMap);
      secondRobot.move(
          secondRobot.determineMove(mode, mainVisitedMap, matrixInput),
          matrixInput, mainVisitedMap);
      sleep(DELAY_SECOND);
    }

    std::cout << firstRobot.getName()
              << " total points : " << firstRobot.getTotalPoint() << '\n';
    std::cout << secondRobot.getName()
              << " total points : " << secondRobot.getTotalPoint() << '\n';
    if (firstRobot.getTotalPoint() > secondRobot.getTotalPoint()) {
      std::cout << "The winter is robot " << firstRobot.getName() << ".\n";
    } else if (firstRobot.getTotalPoint() < secondRobot.getTotalPoint()) {
      std::cout << "The winter is second robot.";
      std::cout << "The winter is robot " << firstRobot.getName() << ".\n";
    } else {
      std::cout << "Draw.\n";
    }
    firstRobot.writeToFile("output.txt");
    secondRobot.writeToFile("output.txt");
    sleep(1);
  }

  return 0;
}
