#include <iostream>
using namespace std;

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
        length = -1;
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

int main() {
    // LinkedList<int> l = LinkedList<int>();
    // l.insert(1);
    // l.insert(2);
    // cout << l.start->data;

    // int length = 1;
    // LinkedList<int> *l2 = new LinkedList<int>[20];
    // l2[0] = l;

    Graph g = Graph("1 4 -1 0 2 4 -1 1 3 -1 2 4 5 -1 0 1 3 -1 3 -2");
    g.printList();
    // cout << g.adjacencyList->data.data;
    // cout << g.adjacencyList->data.next->data;
    return 0;
}