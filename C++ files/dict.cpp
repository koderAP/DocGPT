#include "dict.h"

Dict::Dict(){
    root = new node();
}

Dict::~Dict(){
    delete root;  root = nullptr;
}

void Dict::insert(string word, long long int count){
    node* temp = root;
    for (int i=0; i<word.length(); i++){
        int index = fun(word[i]);
        if(temp->child[index] == nullptr){
            temp->child[index] = new node(word[i]);
            temp = temp->child[index];
        }
        else{
            temp = temp->child[index];
        }
    }
    temp->count+=count;
    return;
}

void Dict::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence){
    string word = "";
    for(int i=0;i<sentence.length();i++){
        char ch = sentence[i];
        if(ch>='A' && ch<='Z'){
            ch = ch+32;
            word = word+ch;
        }
        //seperators   . , - : ! " ' ( ) ? [ ] ; @
        else if(isSpecialCharacter(ch)){
            if(word!=""){
                insert(word,1); 
                word="";
            }
        }
        else{
            word = word+ch;
        }
    }
    if(word!=""){
        insert(word,1);
        word="";
    }
    return;
}

long long int Dict::get_word_count(string word){
    node* temp = root;
    for (int i=0; i<word.size(); i++) {
        if (temp->child[fun(word[i])] == nullptr) {
            return 0;
        }
        temp = temp->child[fun(word[i])];
    }
    return temp->count;
}
