#include <assert.h>
#include <sstream>
#include "qna_tool.h"

using namespace std;

void swap(Obj &a, Obj &b) {
    Obj temp = a;
    b = a;
    b = temp;
}

QNA_tool::QNA_tool(){
    // Implement your function here 
    gen_dict = new Dict();
    dictionary = new Dict();

    ifstream file("unigram_freq.csv");
    string line;
    getline(file, line);
    stringstream ss(line);
    string word;
    getline(ss, word, ',');
    string freq;
    getline(ss, freq);
    if(freq!="count") {
        long long frequency = stoll(freq);
        gen_dict->insert(word, frequency);
    }
    while (getline(file, line)) {
        stringstream ss(line);
        string word;
        getline(ss, word, ',');
        string freq;
        getline(ss, freq, ',');
        long long frequency = stoll(freq);
        gen_dict->insert(word, frequency);
    } 

}

QNA_tool::~QNA_tool(){
    // Implement your function here  
    for(int i=0;i<vec.size();i++){
        if (vec[i] != nullptr) {
            delete vec[i]; vec[i] = nullptr;
        }
    }

    delete gen_dict; gen_dict = nullptr;
    delete dictionary; dictionary = nullptr;
}

void QNA_tool::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence){
    // Implement your function here 
    if(vec.size()==0){
        Obj* b = new Obj(book_code,page,paragraph,sentence);
        vec.push_back(b);
        dictionary->insert_sentence(book_code,page,paragraph,0,sentence);
        return;
    } 

    Obj* a = vec.back();
    if(a->book_code==book_code && a->page_no==page && a->para_no==paragraph){
        a->D.append(sentence);
    }
    else{
        Obj* b = new Obj(book_code,page,paragraph,sentence);
        vec.push_back(b);
    }

    dictionary->insert_sentence(book_code,page,paragraph,0,sentence);
    return;
}

void heapify(vector<HeapNode> &heap, int index){
    int minindex=index;
    int left = 2*index+1;
    int right=2*index+2;

    if(left>=heap.size()){return;}

    if(heap[left].score<heap[minindex].score){
        minindex=left;
    }

    if(right<heap.size() && heap[right].score<heap[minindex].score){
        minindex=right;
    }

    if(minindex!=index){
        swap(heap[minindex],heap[index]);
        heapify(heap,minindex);
    }
}

Node* QNA_tool::get_top_k_para(string question, int k) {
    // Implement your function here
    vector<string> words;
    vector<double> num;

    string word = "";
    
    for(int i=0;i<question.length();i++){
        char ch = question[i];
        if(ch>='A' && ch<='Z'){
            ch = ch+32;
            word = word+ch;
        }
        //seperators   . , - : ! " ' ( ) ? [ ] ; @
        else if(gen_dict->isSpecialCharacter(ch)){
            if(word!=""){
                words.push_back(word); num.push_back((double)(dictionary->get_word_count(word) + 1) / ((double)(gen_dict->get_word_count(word) + 1)));
                word="";
                
            }
        }
        else{
            word = word+ch;
        }
    }
    
    if(word!=""){
        words.push_back(word); num.push_back((double)(dictionary->get_word_count(word) + 1) / ((double)(gen_dict->get_word_count(word) + 1)));

        word="";
    }

    //all words and their counts in csv are stored at this point
    vector<HeapNode> heap;
    int flg=0;
    for(int i=0;i<vec.size();i++){
        HeapNode temp = HeapNode(vec[i]->book_code,vec[i]->page_no,vec[i]->para_no);
        Dict d;
        d.insert_sentence(0,0,0,0,vec[i]->D);
        for(int j=0;j<words.size();j++){
            temp.score += (double)((double)(d.get_word_count(words[j])) * (double)(num[j]));
        }
        
        if(heap.size() < k){
            heap.push_back(temp);
        }
        else if( flg==0 && heap.size() == k){
            for(int x = k/2-1;x>=0;x--){
                heapify(heap,x);
            }
            flg=1;
        }
        else if(temp.score > heap[0].score){
            swap(heap[0],temp);
            heapify(heap,0);
        }
    }

    Node* curr = new Node(heap[0].book_code, heap[0].page_no, heap[0].para_no, 0, 0);
    swap(heap[0], heap[k-1]); heap.pop_back(); heapify(heap, 0);
    for (int i = 1; i < k; i++) {
        Node* temp = new Node(heap[0].book_code, heap[0].page_no, heap[0].para_no, 0, 0);
        curr->left = temp; temp->right = curr; curr = curr->left;
        swap(heap[0], heap[heap.size() - 1]);
        heap.pop_back();
        heapify(heap, 0);
    }
    return curr;
}

void QNA_tool::query(string question, string filename){
    // Implement your function here  
    int k=11;
    Node* root = get_top_k_para_(question,k);
    string api = "YOUR CHAT GPT API_KEY";
    query_llm(filename,root,k,api,question);
    return;
}

std::string QNA_tool::get_paragraph(int book_code, int page, int paragraph){

    cout << "Book_code: " << book_code << " Page: " << page << " Paragraph: " << paragraph << endl;
    
    std::string filename = "txtfiles/mahatma-gandhi-collected-works-volume-";
    filename += to_string(book_code);
    filename += ".txt";

    std::ifstream inputFile(filename);

    std::string tuple;
    std::string sentence;

    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the input file " << filename << "." << std::endl;
        exit(1);
    }

    std::string res = "";

    while (std::getline(inputFile, tuple, ')') && std::getline(inputFile, sentence)) {
        // Get a line in the sentence
        tuple += ')';

        int metadata[5];
        std::istringstream iss(tuple);

        // Temporary variables for parsing
        std::string token;

        // Ignore the first character (the opening parenthesis)
        iss.ignore(1);

        // Parse and convert the elements to integers
        int idx = 0;
        while (std::getline(iss, token, ',')) {
            // Trim leading and trailing white spaces
            size_t start = token.find_first_not_of(" ");
            size_t end = token.find_last_not_of(" ");
            if (start != std::string::npos && end != std::string::npos) {
                token = token.substr(start, end - start + 1);
            }
            
            // Check if the element is a number or a string
            if (token[0] == '\'') {
                // Remove the single quotes and convert to integer
                int num = std::stoi(token.substr(1, token.length() - 2));
                metadata[idx] = num;
            } else {
                // Convert the element to integer
                int num = std::stoi(token);
                metadata[idx] = num;
            }
            idx++;
        }

        if(
            (metadata[0] == book_code) &&
            (metadata[1] == page) &&
            (metadata[2] == paragraph)
        ){
            res += sentence;
        }
    }

    inputFile.close();
    return res;
}

void QNA_tool::query_llm(string filename, Node* root, int k, string API_KEY, string question){

    // first write the k paragraphs into different files

    Node* traverse = root;
    int num_paragraph = 0;
    string sg="";
    while(num_paragraph < k){
        assert(traverse != nullptr);
        string p_file = "paragraph_";
        p_file += to_string(num_paragraph);
        p_file += ".txt";
        sg+=p_file + " ";
        // delete the file if it exists
        remove(p_file.c_str());
        ofstream outfile(p_file);
        string paragraph = get_paragraph(traverse->book_code, traverse->page, traverse->paragraph);
        assert(paragraph != "$I$N$V$A$L$I$D$");
        outfile << paragraph;
        outfile.close();
        traverse = traverse->right;
        num_paragraph++;
    }

    // write the query to query.txt
    ofstream outfile("query.txt");
    outfile << question;
    // You can add anything here - show all your creativity and skills of using ChatGPT
    outfile.close();
 
    // you do not need to necessarily provide k paragraphs - can configure yourself

    // python3 <filename> API_KEY num_paragraphs query.txt
    string command = "python3 ";
    command += filename;
    command += " ";
    command += API_KEY;
    command += " ";
    command += sg;
    command += " ";
    command += "query.txt";
    // cout<<command<<endl;
    system(command.c_str());
    return;
}

Node* QNA_tool::get_top_k_para_(string question, int k) {
    // Implement your function here
    vector<string> words;
    vector<double> num;

    string word = "";
    
    for(int i=0;i<question.length();i++){
        char ch = question[i];
        if(ch>='A' && ch<='Z'){
            ch = ch+32;
            word = word+ch;
        }
        //seperators   . , - : ! " ' ( ) ? [ ] ; @
        else if(gen_dict->isSpecialCharacter(ch)){
            if(word!=""){
                words.push_back(word); num.push_back(1 / ((double)(dictionary->get_word_count(word) + 1) * (double)(gen_dict->get_word_count(word) + 1)));
                word="";
            }
        }
        else{
            word = word+ch;
        }
    }
    
    if(word!=""){
        words.push_back(word); num.push_back(1 / ((double)(dictionary->get_word_count(word) + 1) * (double)(gen_dict->get_word_count(word) + 1)));

        word="";
    }

    //all words and their counts in csv are stored at this point
    vector<HeapNode> heap;
    int flg=0;
    for(int i=0;i<vec.size();i++){
        HeapNode temp = HeapNode(vec[i]->book_code,vec[i]->page_no,vec[i]->para_no);
        Dict d;
        d.insert_sentence(0,0,0,0,vec[i]->D);
        for(int j=0;j<words.size();j++){
            int frequ = (double)(d.get_word_count(words[j]));
            temp.score += (double)(frequ * (double)(num[j]));
        }
        
        if(heap.size() < k){
            heap.push_back(temp);
        }
        else if( flg==0 && heap.size() == k){
            for(int x = k/2-1;x>=0;x--){
                heapify(heap,x);
            }
            flg=1;
        }
        else if(temp.score > heap[0].score){
            swap(heap[0],temp);
            heapify(heap,0);
        }
    }

    Node* curr = new Node(heap[0].book_code, heap[0].page_no, heap[0].para_no, 0, 0);
    swap(heap[0], heap[k-1]); heap.pop_back(); heapify(heap, 0);
    for (int i = 1; i < k; i++) {
        Node* temp = new Node(heap[0].book_code, heap[0].page_no, heap[0].para_no, 0, 0);
        curr->left = temp; temp->right = curr; curr = curr->left;
        swap(heap[0], heap[heap.size() - 1]);
        heap.pop_back();
        heapify(heap, 0);
    }
    return curr;
}


