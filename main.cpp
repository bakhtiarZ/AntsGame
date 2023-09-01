#include <fstream>
#include <array>

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <set>
#include <numeric>
#include <thread>
#include <mutex>
using namespace std;


class Ant {
private:
    bool backtracking{ false };
    bool diagonal{ false };
    std::mt19937 gen{};
    std::set<pair<int, int>> availableMoves{ {1,0}, {0,1}, {-1,0}, {0,-1}, {1,1}, {1,-1}, {-1,-1}, {-1,1} };
    std::set<pair<int, int>> NoDiag{ {1,0}, {0,1}, {-1,0}, {0,-1} };
public:
    Ant() = default;
    ~Ant() = default;
    int x, prevx, y, prevy, height, width;

    Ant(int x, int y, std::mt19937 _gen, int height = 8, int width = 8, bool diagonal = false, bool backtracking = false) {
        this->x = x;
        this->y = y;
        this->prevx = x;
        this->prevy = y;
        this->height = height;
        this->width = width;
        this->backtracking = backtracking;
        this->diagonal = diagonal;
        gen = _gen;
    }
    std::pair<int, int> getPos()  {
        return std::make_pair(x, y);
    }

    std::pair<int, int> getPrevPos()  {
        return std::make_pair(prevx, prevy);
    }

    void setPos(std::pair<int, int> pos) {
        prevx = x;
        prevy = y;
        x = pos.first;
        y = pos.second;
    }

    std::pair<int, int> move() {
        std::set<pair<int, int>> setp;
        if (diagonal) setp = availableMoves;
        else setp = NoDiag;
        if (!backtracking) setp.erase(make_pair(prevx - x, prevy - y));
        if (x == 0) {
            setp.erase(make_pair(-1, 0));
            if (diagonal) {
                setp.erase(make_pair(-1, -1));
                setp.erase(make_pair(-1, 1));
            }
        }
        if (y == 0) {
            setp.erase(make_pair(0, -1));
            if (diagonal) {
                setp.erase(make_pair(-1, -1));
                setp.erase(make_pair(1, -1));
            }
        }
        if (x == width - 1) {
            setp.erase(make_pair(1, 0));
            if (diagonal) {
                setp.erase(make_pair(1, -1));
                setp.erase(make_pair(1, 1));
            }
        }
        if (y == height - 1) {
            setp.erase(make_pair(0, 1));
            if (diagonal) {
                setp.erase(make_pair(-1, 1));
                setp.erase(make_pair(1, 1));
            }
        }
        std::uniform_int_distribution<> distrib(0, setp.size() - 1);
        int idx = distrib(gen);
        auto move = *std::next(setp.begin(), idx);
        return make_pair(move.first + x, move.second + y);
    }

};

class Chessboard {
private:
    Ant a{};
    Ant b{};
    int width{ 0 };
    int height{ 0 };
    bool backtracking{ false };
    bool diagonal{ false };
    uint32_t moveCount;
public:

    bool metyet{ false };
    bool crossed{ false };

    explicit Chessboard(int _width = 8, int _height = 8, bool _backtracking = false, bool _diagonal = false) : moveCount(0), width(_width), height(_height), backtracking(_backtracking), diagonal(_diagonal)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        a = Ant(0, 0, gen, 8, 8, diagonal, backtracking);
        b = Ant(width - 1, height - 1, gen, 8, 8, diagonal, backtracking);

    }

    void moveAnts() {
        moveCount++;
        auto aNP = a.move();
        auto bNP = b.move();
        a.setPos(aNP);
        b.setPos(bNP);

        if(a.getPos() == b.getPos()){
            metyet = true;
        }
        if (a.getPrevPos() == b.getPrevPos()) {
            crossed = true;
        }
    }
    [[nodiscard]] uint32_t getMoveCount() const {
        return moveCount;
    }
};

std::mutex metarrmutex;
std::mutex crossarrmutex;

void simu(std::array<int, 4000>& metArr, std::array<int, 4000>& crossArr) {
    for (uint32_t i = 0; i < 20000; i++) {
        Chessboard* game = new Chessboard(8,8,false,false);
        while (!game->metyet || !game->crossed) {
            game->moveAnts();
            if (game->metyet) {
                std::lock_guard<std::mutex> guard(metarrmutex);
                metArr[game->getMoveCount()]++;
            }
            if (game->crossed) {
                std::lock_guard<std::mutex> guard(crossarrmutex);
                crossArr[game->getMoveCount()]++;
            }
        }
        delete game;
    }
}




int main() {

    std::array<int, 4000> metarr{};
    std::array<int, 4000> crossarr{};
    for (auto& e : metarr) e = 0;
    for (auto& e : crossarr) e = 0;
    std::thread t1(simu, std::ref(metarr), std::ref(crossarr));
    std::thread t2(simu, std::ref(metarr), std::ref(crossarr));
    std::thread t3(simu, std::ref(metarr), std::ref(crossarr));
    std::thread t4(simu, std::ref(metarr), std::ref(crossarr));
    std::thread t5(simu, std::ref(metarr), std::ref(crossarr));
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    std::ofstream myFile("nobnod2");
    int a = 0;
    for (int i = 0; i < metarr.size(); i++) {
        if (metarr[i] != 0){
            a += metarr[i];
            myFile << i << "," << metarr[i] << "\n";
        }
    }
    myFile << "\n\n now cross: \n\n";
    for (int i = 0; i < crossarr.size(); i++) {
        if (crossarr[i] != 0) {
            myFile << i << "," << crossarr[i] << "\n";
        }
    }

    //Close the file
    myFile.close();
    return a;
}