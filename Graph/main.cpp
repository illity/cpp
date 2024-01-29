#include <iostream>
#include <pdcurses/curses.h>
#include <bits/stdc++.h>
using namespace std;

#define minX -5.0
#define minY -5.0
#define maxX 5.0
#define maxY 5.0


#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__linux__)
#include <sys/ioctl.h>
#endif // Windows/Linux

void get_terminal_size(int& width, int& height) {
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = (int)(csbi.srWindow.Right-csbi.srWindow.Left+1);
    height = (int)(csbi.srWindow.Bottom-csbi.srWindow.Top+1);
#elif defined(__linux__)
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    width = (int)(w.ws_col);
    height = (int)(w.ws_row);
#endif // Windows/Linux
}

class Point {
    public:
        double x, y;
        Point() {};
        Point(double x, double y): x(x), y(y) {};
};

void drawPoint(double x, double y, double val) {
    int width, height;
    get_terminal_size(width, height);
    mvprintw(height*(y - minY)/(maxY - minY),
                width *(x - minX)/(maxX - minX), "(%.0f)", val);
}

void drawPoints(Point* p, int length) {
    int width, height;
    get_terminal_size(width, height);
    for (int i = 0; i < length; i++) {
        mvaddch(height*(p[i].y - minY)/(maxY - minY),
                width *(p[i].x - minX)/(maxX - minX),i+1+0x30);
    }
}

void drawLine(Point p0, Point pf, char c) {
    int width, height;
    get_terminal_size(width, height);
    int x0 = width *(p0.x - minX)/(maxX - minX);
    int y0 = height*(p0.y - minY)/(maxY - minY);
    int xf = width *(pf.x - minX)/(maxX - minX);
    int yf = height*(pf.y - minY)/(maxY - minY);
    int t = max(abs(yf-y0),abs(xf-x0));
    for (int i = 0; i<t; i++) {
        mvaddch(y0+i*(yf-y0)/t,
                x0+i*(xf-x0)/t,c);
    }
}

double sqrt(double x) {
    double tol = 1e-8*x;
    double x0 = x;
    while (x0-x/x0>tol || x0-x/x0<-tol) {
        x0 -= (x0-x/x0)/2;
    }
    return x0;
}

template <class T>
class LinkedList {
    public:
    class Node {
        public:
        T data;
        Node* next;
        Node() {};
        Node(T data): data(data), next(NULL) {};
        bool insert(T data) {
            next = new Node(data);
            return true;
        }
        ~Node() {
            if (next) {delete next;};
        }
    };
    Node *start;
    Node *last;
    LinkedList(): start(NULL), last(NULL) {};
    LinkedList(T d): LinkedList() {insert(d);};
    ~LinkedList() {
        if(start) delete start;
    }
    void insert(T d) {
        Node *temp = new Node(d);
        if (start == NULL) {
            start = temp;
            last = temp;
        } else {
            last->next = temp;
            last = temp;
        }
    }
    void print() {
        Node *temp = start;
        while(temp) {
            cout << temp->data;
            temp = temp->next;
            if (temp) cout << ", ";
        }
        cout << "\n";
    }
};


class Graph {
    public:
    class Edge {
        public:
        int vertice;
        double weight = 0;
        Edge() {};
        Edge(int vertice): vertice(vertice) {};
        Edge(int vertice, double weight): vertice(vertice), weight(weight) {};
        
    };
    int length;
    LinkedList<Edge> *adjacencyList;
    bool weighted = false;
    Graph() {};
    Graph(string str) {
        // 1 4 -1 0 2 4 -1 1 3 -1 2 4 5 -1 0 1 3 -1 3 -2
        // equivale a [[1,4],[0,2,4],[1,3],[2,4,5],[0,1,3],[3]]
        length = -1;
        for (int i = 0; i < str.length(); i++) {if (str[i]=='-') length++;}
        adjacencyList= new LinkedList<Edge>[length];
        LinkedList<Edge> temp = LinkedList<Edge>();
        int number = 0;
        bool sign = false;
        int count = 0;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == ' ') {
                if (number == -1) {
                    adjacencyList[count ] = temp;
                    temp = LinkedList<Edge>();
                    count++;
                }
                else {
                    temp.insert(Edge(number));
                }
                number = 0;
                sign = false;
            }
            else if (str[i] == '-') sign = true;
            else number = number*10 + (sign?-1:1)*(str[i]-0x30);
        }
    }
    Graph(string str, string weights): Graph(str) {
        weighted = true;
        LinkedList<Edge>::Node *temp = adjacencyList[0].start;
        int number = 0;
        bool sign = false;
        int count = 0;
        for (int i = 0; i < weights.length(); i++) {
            if (weights[i] == ' ') {
                if (number == -1) {
                    // cout << count;
                    temp = adjacencyList[++count].start;
                }
                else {
                    temp->data.weight = number;
                    temp = temp->next;
                }
                number = 0;
                sign = false;
            }
            else if (weights[i] == '-') sign = true;
            else number = number*10 + (sign?-1:1)*(weights[i]-0x30);
        }
    }

    void printList() {
        for (int i = 0; i < length; i++) {
            adjacencyList[i].print();
        }
    }

    void draw() {
        Point *points = new Point[length];
        for (int i = 0; i < length; i++) {
            points[i].x = (rand()%100)/100.0;
            points[i].y = (rand()%100)/100.0;
        }


        double k_elas = .02;
        double k_elet = .1;
        double k_grav = .01;
        double comprimento = 1;
        initscr();
        for (int it = 0; it < 1000; it++) {
            for (int i = 0; i < length; i++) {         
                points[i].x -= k_grav * sqrt(points[i].x * points[i].x + points[i].y * points[i].y) * points[i].x;
                points[i].y -= k_grav * sqrt(points[i].x * points[i].x + points[i].y * points[i].y) * points[i].y;
                LinkedList<Edge>::Node* temp = adjacencyList[i].start;
                while(temp) {
                    int j = temp->data.vertice;
                    double d = (points[i].x-points[j].x)*(points[i].x-points[j].x) + (points[i].y-points[j].y)*(points[i].y-points[j].y);
                    d = sqrt(d) - 1;
                    points[i].x -= k_elas*(points[i].x-points[j].x)/d;
                    points[i].y -= k_elas*(points[i].y-points[j].y)/d;
                    temp = temp->next;
                }
                for (int j = 0; j < length; j++) {
                    if (i == j) continue;
                    double d = (points[i].x-points[j].x)*(points[i].x-points[j].x) + (points[i].y-points[j].y)*(points[i].y-points[j].y);
                    points[i].x += k_elet*(points[i].x-points[j].x)/d;
                    points[i].y += k_elet*(points[i].y-points[j].y)/d;
                }
            }
            {
                clear();
                int ligacao = 0;
                for (int i = 0; i < length; i++) {
                    LinkedList<Edge>::Node* temp = adjacencyList[i].start;
                    
                    while(temp) {
                        int j = temp->data.vertice;
                        if (i<j) {
                            drawLine(points[i],points[j], ".,#-_=+*/"[ligacao]);
                            ligacao+=1;
                        }
                        temp = temp->next;
                    }
                    temp = adjacencyList[i].start;
                    
                    while(temp) {
                        int j = temp->data.vertice;
                        if (weighted) drawPoint((points[i].x+points[j].x)/2, (points[i].y+points[j].y)/2, temp->data.weight);
                        temp = temp->next;
                    }
                }
                drawPoints(points, length);
            }
            refresh();
        }
        getchar();
        endwin();
    }

    void prim() {
        double min = adjacencyList[0].start->data.weight;
        set<int> visited = set<int>();
        int head, tail;
        LinkedList<Edge>* newAdjacencyList = new LinkedList<Edge>[length];

        for (int i = 0; i<length; i++) newAdjacencyList[i] = LinkedList<Edge>();
        for (int i = 0; i < length; i++) {
            LinkedList<Edge>::Node* temp = adjacencyList[i].start;
            int j = 0;
            while (temp) {
                if (temp->data.weight < min) {
                    min = temp->data.weight;
                    head = i;
                    tail = temp->data.vertice;
                }
                temp = temp->next;
                j++;
            }
        }
        newAdjacencyList[head].insert(Edge(tail, min));
        newAdjacencyList[tail].insert(Edge(head, min));
        visited.insert(head);
        visited.insert(tail);
        while(1) {
            LinkedList<Edge>::Node* temp = adjacencyList[head].start;
            int oldHead = head;
            int oldTail = tail;
            // Torna o novo mínimo qualquer um que não tenha sido visitado ainda
            while(temp) {
                if (!visited.count(temp->data.vertice)) {
                    min = temp->data.weight;
                    head = temp->data.vertice;
                    break;
                }
                temp = temp -> next;
            }
            // Visita todos os nós do head, para encontrar o menor entre eles, esse será o novo head, exceto se houver um candidato a novo tail que seja melhor
            while(temp) {
                if (!visited.count(temp->data.vertice) && temp->data.weight < min) {
                    min = temp->data.weight;
                    head = temp->data.vertice;
                }
                temp = temp -> next;
            }
            // Visita todos os nós do tail, para encontrar o menor entre eles, destrona o antigo head se encontrar algum.
            temp = adjacencyList[tail].start;
            while(temp) {
                if (temp->data.weight < min && !visited.count(temp->data.vertice)) {
                    min = temp->data.weight;
                    tail = temp->data.vertice;
                    head = oldHead;
                }
                temp = temp -> next;
            }
            if (head == oldHead) {
                // cout << oldTail << tail;
                newAdjacencyList[oldTail].insert(Edge(tail, min));
                newAdjacencyList[tail].insert(Edge(oldTail, min));
                visited.insert(tail);
            }
            else {
                // cout << oldHead << head;
                newAdjacencyList[oldHead] = LinkedList<Edge>(Edge(head, min));
                newAdjacencyList[head] = LinkedList<Edge>(Edge(oldHead, min));
            }
            if (visited.size() == length) break;
        }
        adjacencyList = newAdjacencyList;
        visited.size();
    }
};

template <class T>
std::ostream& operator<<(std::ostream& os, const LinkedList<T>& obj) {
    for (typename LinkedList<T>::Node *x = obj.start; x; x = x->next) os << x->data << (x->next?", ":" ");
    return os;
}

std::ostream& operator<<(std::ostream& os, const Graph::Edge& obj) {
    os << "(" << obj.vertice << ", " << obj.weight << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Graph& obj) {
    for (int i = 0; i < obj.length; i++) os << obj.adjacencyList[i] << "\n";
    return os;
}

int main() {
    Graph g = Graph("1 5 -1 0 2 6 -1 1 3 -1 2 4 6 -1 3 5 6 -1 0 4 -1 1 3 4 -1 -2", "28 10 -1 28 16 14 -1 16 12 -1 12 22 18 -1 22 25 24 -1 10 25 -1 14 18 24 -1 -2");
    cout << "Initial Graph: \n" << g << "\n";
    g.prim();
    cout << "Minimum cost spanning tree - Plim's algorithm\n" << g;
    // g.draw();
   return 0;
}