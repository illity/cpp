#include <iostream>
#include <pdcurses/curses.h>
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
    LinkedList<float> *weightList;
    bool weighted = false;
    Graph() {};
    Graph(string str) {
        // 1 4 -1 0 2 4 -1 1 3 -1 2 4 5 -1 0 1 3 -1 3 -2
        // equivale a [[1,4],[0,2,4],[1,3],[2,4,5],[0,1,3],[3]]
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
    Graph(string str, string weights): Graph(str) {
        weighted = true;
        length = 0;
        for (int i = 0; i < weights.length(); i++) {if (weights[i]=='-') length++;}
        weightList = new LinkedList<float>[length];
        LinkedList<float> temp = LinkedList<float>();
        int number = 0;
        bool sign = false;
        int count = 0;
        for (int i = 0; i < weights.length(); i++) {
            if (weights[i] == -2) break;
            else if (weights[i] == ' ') {
                if (number == -1) {
                    weightList[count] = temp;
                    temp = LinkedList<float>();
                    count++;
                }
                else {
                    cout;
                    temp.insert(number);
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
            // adjacencyList[i].print();
            // weightList[i].print();
            if (i<length-1) cout << "\n";
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
                LinkedList<int>::Node* temp = adjacencyList[i].start;
                while(temp) {
                    int j = temp->data;
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
                    LinkedList<int>::Node* temp = adjacencyList[i].start;
                    LinkedList<float>::Node* temp2 = weightList[i].start;
                    
                    while(temp) {
                        int j = temp->data;
                        if (i<j) {
                            drawLine(points[i],points[j], ".,#-_=+*/"[ligacao]);
                            ligacao+=1;
                            cout << ligacao;
                        }
                        temp = temp->next;
                        temp2 = temp2->next;
                    }
                    temp = adjacencyList[i].start;
                    temp2 = weightList[i].start;
                    
                    while(temp) {
                        int j = temp->data;
                        if (weighted) drawPoint((points[i].x+points[j].x)/2, (points[i].y+points[j].y)/2, temp2->data);
                        temp = temp->next;
                        temp2 = temp2->next;
                    }
                }
                drawPoints(points, length);
            }
            refresh();
        }
            getchar();
        endwin();

 
    }
};


int main() {
    // LinkedList<int> l = LinkedList<int>();
    // l.insert(1);
    // l.insert(2);
    // cout << l.start->data;

    // int length = 1;
    // LinkedList<int> *l2 = new LinkedList<int>[20];
    // l2[0] = l;


    // 1 5
    // 0 2 6
    // 1 3
    // 2 4 6
    // 3 5 6
    // 0 4
    // 1 3 4
    Graph g = Graph("1 5 -1 0 2 6 -1 1 3 -1 2 4 6 -1 3 5 6 -1 0 4 -1 1 3 4 -2", "28 10 -1 28 16 14 -1 16 12 -1 12 22 18 -1 22 25 24 -1 10 25 -1 14 24 -2");
    g.printList();
    g.draw();
    // g.draw();

   return 0;
}