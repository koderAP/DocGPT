# This is a tool, with which you can interact with you document, you can ask specific Questions, like tell me about a particular event/theory or personality, and if details about it exists in your document, then it returns the answer in context, with proper interation like experience.

 To run your query change the query in tester.cpp, for example 'What was Satyagraha Movement?'
 Also make sure to include you Chat GPT API key in qna_too.cpp file
 To complie, inculde all files in a single folder, you folder structure should be : 
         -DocGPT
              --All files, inculding .cpp and .h files
	         - txtfiles folder, which contain processed corpora of books on Mahatma Gandhi
         
 To compile run 'g++ Node.cpp dict.cpp qna_tool.cpp tester.cpp -o a.out' on terminal and then to get output run './a.out' on terminal.

![alt text](./Sample%20output.png)