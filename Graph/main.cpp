#include <iostream>
#include <pdcurses/curses.h>
using namespace std;



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
            if (next) delete next;
        }
    };
    Node *start;
    Node *last;
    LinkedList(): start(NULL), last(NULL) {};
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
    }
};

class Graph {
    public:
    int length;
    LinkedList<int> *adjacencyList;
    Graph() {};
    Graph(string str) {
        //1 4 -1 0 2 4 -1 1 3 -1 2 4 5 -1 0 1 3 -1 3 -2
        //equivale a [[1,4],[0,2,4],[1,3],[2,4,5],[0,1,3],[3]]
        // adjacencyList = new Node<Node<int>>();
        length = 0;
        for (int i = 0; i < str.length(); i++) {if (str[i]=='-') length++;}
        adjacencyList= new LinkedList<int>[length];
        LinkedList<int> temp = LinkedList<int>();
        int number = 0;
        bool sign = false;
        int count = 0;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == -2) break;
            else if (str[i] == ' ') {
                if (number == -1) {
                    adjacencyList[count] = temp;
                    temp = LinkedList<int>();
                    count++;
                }
                else {
                    temp.insert(number);
                }
                number = 0;
                sign = false;
            }
            else if (str[i] == '-') sign = true;
            else number = number*10 + (sign?-1:1)*(str[i]-0x30);
        }
    }
    void printList() {
        for (int i = 0; i < length; i++) {
            adjacencyList[i].print();
            if (i<length-1) cout << "\n";
        }
    }
};

class Point {
    public:
        double x, y;
        Point(): x(x), y(y) {};
};


void drawPoints(Point* p, int length) {
    int width, height;
    get_terminal_size(width, height);
    double minX = -1, minY = -1;
    double maxX = 1, maxY = 1;

    // return;
    for (int i = 0; i < length; i++) {
        mvaddch(height*(p[i].y - minY)/(maxY - minY),
                width *(p[i].x - minX)/(maxX - minX),i+0x30);
    }
}

int main() {
    // LinkedList<int> l = LinkedList<int>();
    // l.insert(1);
    // l.insert(2);
    // cout << l.start->data;

    // int length = 1;
    // LinkedList<int> *l2 = new LinkedList<int>[20];
    // l2[0] = l;

    Graph g = Graph("1 4 -1 0 2 4 -1 1 3 -1 2 4 5 -1 0 1 3 -1 3 -2");
    // g.printList();

    Point *points = new Point[g.length];
    for (int i = 0; i < g.length; i++) {
        points[i].x = (rand()%100)/100.0;
        points[i].y = (rand()%100)/100.0;
    }


    double k_elet = .0001;
    initscr();
    for (int it = 0; it < 10000; it++) {
        for (int i = 0; i < g.length; i++) {

            for (int j = 0; i < g.length; i++) {
                if (i == j) continue;
                double d = (points[i].x-points[j].x)*(points[i].x-points[j].x) + (points[i].y-points[j].y)*(points[i].y-points[j].y);
                points[i].x += k_elet*(points[i].x-points[j].x)/d;
                points[i].y += k_elet*(points[i].y-points[j].y)/d;
            }
        }
        drawPoints(points, g.length);
        refresh();
    }
    endwin();

    return 0;
}