#include <string> 
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;
#define len 36

struct node {
    char val = '0';
    node* child[len] = {nullptr};
    long long int count = 0;

    node(){

    }

    node(char ch) {
        val = ch;
    }

    ~node(){
        val='0';
        for(int i=0;i<len;i++){
            if(child[i]!=nullptr){
                delete child[i]; child[i]=nullptr;
            }
        }
        count=0;
    }
};

class Dict {
public:
    node* root;
    int fun(char ch){
        if(ch>='a' && ch<='z'){
            return ch-'a';
        }
        else if(ch>='A' && ch<='Z'){
            return ch-'A';
        }
        else if(ch>='0' && ch<='9'){
            return ch-22;
        }
        return 0;
    }

    bool isSpecialCharacter(char ch) {
    switch (ch) {
        case ' ':
        case '.':
        case ',':
        case '-':
        case ':':
        case '!':
        case '"':
        case '\'':
        case '(':
        case ')':
        case '?':
        case '[':
        case ']':
        case ';':
        case '@':
        case '#':
        case '$':
        case '%':
        case '^':
        case '&':
        case '*':
        case '+':
        case '=':
        case '<':
        case '>':
        case '/':
        case '\\':
        case '|':
        case '~':
        case '`':
        case '{':
        case '}':
        case '_':
            return true;
        default:
            return false;
    }
}

public: 
    /* Please do not touch the attributes and 
    functions within the guard lines placed below  */
    /* ------------------------------------------- */
    Dict();

    ~Dict();

    void insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence);

    long long int get_word_count(string word);

    void dump_dictionary(string filename);

    void insert(string word,long long int count);

    /* -----------------------------------------*/
};