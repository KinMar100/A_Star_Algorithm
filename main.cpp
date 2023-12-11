#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <windows.h>
#include <algorithm>

using namespace std;

const int ROWS = 20;
const int COLUMNS = 20;
const int OBSTACLE = 5;
const int START_ROW = 4;
const int START_COL = 5;
const int GOAL_ROW = 17;
const int GOAL_COL = 17;


struct Node {
    // zmienne dotyczące wierszy i kolumn
    int row, col;
    // funkcje dotyczące przeliczania heurystyki
    int g, h, f;
    Node* parent;

    Node(int r, int c) : row(r), col(c), g(0), h(0), f(0), parent(nullptr) {}

    // Funkcja przeliczająca heurystykę
    void calcHeuristic()
    {
        h = int(hypot(GOAL_ROW - row, GOAL_COL - col));
    }
};

//Tablica otwarta
vector<Node*> openList;
//Tablica zamknięta
vector<Node*> closedList;

// Funkcja sprawdzająca, czy dana pozycja jest na mapie i czy nie jest przeszkodą (piątką)
bool isValidate(int row, int col, int** grid) {
    if (row >= 0 && row < ROWS && col >= 0 && col < COLUMNS && grid[row][col] != OBSTACLE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Funkcja sprawdzająca czy node (węzeł) jest w otwartej liście
bool isInOpenList(int row, int col)
{
    for (const Node* n : openList) {
        if (n->row == row && n->col == col) {
            return true;
        }
    }
    return false;
}

// Funkcja sprawdzająca, czy node (węzeł) jest w zamkniętej liście
bool isInClosedList(int row, int col) {
    for (const Node* n : closedList) {
        if (n->row == row && n->col == col) {
            return true;
        }
    }
    return false;
}

// Funkcja do przerobienia sciezki od celu do startu
void reconstructPath(Node* current, int** grid) {
    cout << "Optymalna sciezka (punkty): ";
    while (current != nullptr) {
        cout << "[" << current->row << "," << current->col << "] ";
        grid[current->row][current->col] = 3;
        current = current->parent;
    }
    cout << endl << endl;
}

// Funkcja implementująca algorytm A*
void AStar(int** G) {
    openList.clear();
    closedList.clear();
    Node* startNode = new Node(START_ROW, START_COL);

    if (G[GOAL_ROW][GOAL_COL] == OBSTACLE || !isValidate(START_ROW, START_COL, G) || !isValidate(GOAL_ROW, GOAL_COL, G))
    {
        cout << "Nie mozna znalesc sciezki, element docelowy jest przeszkoda!"<< endl;
        return;
    }

    // Przeliczenie heurystyki od węzła początkowego - Node(0,0)
    startNode->calcHeuristic();
    // Dodanie węzła na otwartą listę
    openList.push_back(startNode);

    // Póki lista otwarta nie jest pusta, ma się wykonywać przejscie węzła z jednego na drugi
    while (!openList.empty())
    {
        //rozpoczęcie od pierwszego elementu na otwartej liście
        Node* current = openList[0];
        for (Node* node : openList)
        {
            //sprawdzenie dla zgodności heurytyki, czy można dany węzeł
            // w pętli umieścić na aktualnym - current
            if (node->f < current->f || (node->f == current->f && node->h < current->h))
            {
                current = node;
            }
        }

        openList.erase(remove(openList.begin(), openList.end(), current), openList.end());
        // dodanie na zamkniętą liste węzła (elementu)
        closedList.push_back(current);

        if (current->row == GOAL_ROW && current->col == GOAL_COL) {
            // gdy cel jest osiągnięty, przekształca ścieżkę
            reconstructPath(current, G);
            break;
        }
        // for (int i = -1; i <= 1; ++1)
        // Generacja sąsiadów
        for (int i = 1; i >= -1; --i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if ((i == 0 && j == 0) || (i != 0 && j != 0))
                {
                    continue;
                }

                //zaznaczenie kolejnego elementu (węzła) dla pozycji i, j
                int newRow = current->row + i;
                int newCol = current->col + j;

                // wykona się jeśli element jest na mapie - jest poprawny
                if (isValidate(newRow, newCol, G)) {
                    // jeśli element jest w zamknietej liscie to idzie dalej
                    if (isInClosedList(newRow, newCol)) continue;

                    // powiekszenie kosztu o 1
                    int tentativeG = current->g + 1;

                    // węzeł określający "następnika"
                    Node* successor;

                    // przeliczenie heurystyki
                    if (!isInOpenList(newRow, newCol)) {
                        successor = new Node(newRow, newCol);
                        successor->calcHeuristic();
                        //dodanie następnika na otwartą listę
                        openList.push_back(successor);
                    }
                    else {
                        auto it = find_if(openList.begin(), openList.end(),
                                          [=](const Node* n) { return n->row == newRow && n->col == newCol; });
                        successor = *it;

                        //dalsza droga po ścieżce
                        if (tentativeG >= successor->g) continue;
                    }

                    // Aktualizacja, że następnik po rodzicu w następnej iteracji
                    // pętli while będzie tym aktualnym - current
                    successor->parent = current;
                    // próbna funkcja g dla następnika
                    successor->g = tentativeG;
                    // próbna funkcja f bedzie wykorzytsaniem heurystyki przy następnym przejściu przez ścieżke
                    successor->f = successor->g + successor->h;
                }
            }
        }
    }

    // Wyczyszczenie pamięci po użyciu algorytmu
    for (Node* node : openList) {
        delete node;
    }
    for (Node* node : closedList){
        delete node;
    }
}

void displayGrid(int** grid) {
    cout << endl;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLUMNS; ++j) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
}

void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetConsoleColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);  // 15 to domyślny kolor
}

void displayGridWithColors(int** grid) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLUMNS; ++j) {
            if (grid[i][j] == 0) {
                setConsoleColor(7); //bialy kolor
            } else if (grid[i][j] == 3) {
                setConsoleColor(10); // zielony kolor
            } else if (grid[i][j] == 5){
                setConsoleColor(12); // czerwony kolor
            }
            cout << grid[i][j] << " ";

            resetConsoleColor();
        }
        cout << std::endl;
    }
}

int main() {

    cout << "Wczytywanie danych z pliku" << endl;

    const string nazwa_pliku = "grid.txt";

    //teraz deklarujemy dynamicznie tablice, do ktorej wczytamy nasz plik
    int rows = COLUMNS + 1;
    int **G;
    G = new int*[rows];

    while(rows--)
    {
        G[rows] = new int[ROWS + 1];
    }

    cout << endl <<"Nacisnij ENTER aby wczytac tablice o nazwie " << nazwa_pliku;
    getchar();

    ifstream file(nazwa_pliku.c_str());

    for (unsigned int i = 0; i < ROWS; i++)
    {
        for (unsigned int j = 0; j < COLUMNS; j++)
        {
            file >> G[i][j];
        }
    }
    file.close();

    cout << endl;

    cout << "WYPISANIE TABLICY Z PLIKU: " << endl;
    cout << "0 - wolna trasa (biale), 5 - przeszkoda (czerwone), 3 - odnaleziona sciezka (zielone): " << endl << endl;

    // displayGrid(G);

    displayGridWithColors(G);

    cout << endl;

    cout << "WYPISANIE TABLICY PO UZYCIU NA NIEJ ALGORYTMU A*: " << endl << endl;

    AStar(G);

    // displayGrid(G);

    displayGridWithColors(G);

    //na koniec czyscimy pamiec po naszej tablicy
    for (int i = 0; i < COLUMNS; i++)
    {
        delete[] G[i];
    }//czyscimy wiersze

    delete[] G;//zwalniamy tablice wskaznikow do wierszy

    cout << endl << endl;

    cout << "Nacisnij ENTER aby zakonczyc" << endl;
    getchar();
    return 0;
}
