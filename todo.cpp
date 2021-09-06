#include <iostream>  //for using cout, cin
#include <fstream>   //for handling file IO
#include <unordered_map> //for using unordered maps for mapping commands to integers
#include <vector>   //for using vectors for storing text file content as vector of strings
#include <time.h>   //for getting date
using namespace std;


//Reads the file (given by fileName) and returns its content in the form of vector of strings 
//where each string corresponds to a single line of the given file
vector<string> readFromFile(string fileName){
    ifstream inputFile(fileName);
    vector<string> data;
    string line;
    while(getline(inputFile, line))
        data.emplace_back(line);
    inputFile.close();
    return data;
}

//Writes to the file (given by fileName).
//It accepts data (to be written to the file) in the form of vector of strings 
//where each string corresponds to a single line to be outputted in the given file
void writeToFile(string fileName, vector<string> data){
    ofstream outputFile(fileName);
    for(auto line:data) outputFile << line << "\n";
    outputFile.close();
}

//Returns current date in the form 'yyyy-mm-dd' as a string
const string currentDate() {
    time_t now = time(0);
    tm  tstruct;
    char date[80];
    // tstruct = *localtime(&now);  //if we need time in local time zone instead of UTC
    tstruct = *gmtime(&now);
    strftime(date, sizeof(date), "%Y-%m-%d", &tstruct);
    return date;
}


class Todo{
    //Todos are pending tasks
    //Dones are completed tasks

    //declaring variables for holding all current Todos and current Dones
    vector<string> allTodos, allDones;

    //for mapping commands to integers
    unordered_map<string, int> commandToInt;

    //returns all pending tasks
    vector<string> getAllTodos(){
        return readFromFile("todo.txt");
    }

    //returns all tasks that are done
    vector<string> getAllDones(){
        return readFromFile("done.txt");
    }

    //propogates the changes in allTodos to the todo text file
    void setAllTodos(){
        writeToFile("todo.txt", allTodos);
    }

    //propogates the changes in allDones to the done text file
    void setAllDones(){
        writeToFile("done.txt", allDones);
    }
    
    public:

    //initializes all class variables
    Todo(){
        allTodos = getAllTodos();
        allDones = getAllDones();
        
        //mapping commands to integers
        commandToInt["help"] = 0;
        commandToInt["ls"] = 1;
        commandToInt["add"] = 2;
        commandToInt["del"] = 3;
        commandToInt["done"] = 4;
        commandToInt["report"] = 5;
    }

    //propagate any changes in the variables to the text files before the end of program execution
    ~Todo(){
        setAllTodos();
        setAllDones();
    }

    //calls the appropriate function based on the given command
    void executeCommand(int argCnt, const char* command[]){
        if(argCnt == 1){  //when on command is provided
            printUsage();
            return;
        }
        if(commandToInt.find(command[1]) == commandToInt.end()){  //for handling invalid/unknown command
            //print error followed by instructions on how to use the program
            cout << "Error: Command '"<< command[1] <<"' not found\n"; 
            printUsage();
            return;
        }
        switch(commandToInt[command[1]]){
            case 0: {
                printUsage();
                break;
            }
            case 1:{
                listTodos();
                break;
            }
            case 2:{
                addTodo(command[2]);
                break;
            }
            case 3:{
                deleteTodo(command[2]);
                break;
            }
            case 4:{
                doneTodo(command[2]);
                break;
            }
            case 5:{
                printReport();
                break;
            }
        }
    }
    
    //prints usage instruction on screen
    void printUsage(){
        cout << 
        "Usage :-\n"
        "$ ./todo add \"todo item\"  # Add a new todo\n"
        "$ ./todo ls               # Show remaining todos\n"
        "$ ./todo del NUMBER       # Delete a todo\n"
        "$ ./todo done NUMBER      # Complete a todo\n"
        "$ ./todo help             # Show usage\n"
        "$ ./todo report           # Statistics" << "\n";
    }

    //prints all todos on the screen with most recent todo at the top
    void listTodos(){
        if(allTodos.size() == 0){
            cout << "There are no pending todos!\n";
            return;
        }
        for(int i = allTodos.size() - 1; i >= 0; --i)
            cout << "[" << i + 1 << "] " << allTodos[i] << "\n";
    }

    //adds a todo
    void addTodo(const char* todo){
        if(todo == NULL){
            cout << "Error: Missing todo string. Nothing added!\n";
            return;
        }
        allTodos.emplace_back(todo);
        cout << "Added todo: \"" << todo << "\"\n";
    }

    //deletes todo based on the given number
    void deleteTodo(const char* num){
        if(num == NULL){
            cout << "Error: Missing NUMBER for deleting todo.\n";
            return;
        }
        
        //try to delete the todo given by the number
        //print error if the number is not a valid number or does not have task associated with it
        try{
            int index = atoi(num) - 1;
            if(index >= 0 and index < (int)allTodos.size()){
                allTodos.erase(allTodos.begin() + index);
                cout << "Deleted todo #" << index + 1 << "\n";  
            }
            else
                throw -1;
        } 
        catch(int errorCode){
            cout << "Error: todo #" << num <<" does not exist. Nothing deleted.\n";
        }
    }

    //moves todo given by the number form todo-list to done-list
    void doneTodo(const char* num){
        if(num == NULL){
            cout << "Error: Missing NUMBER for marking todo as done.\n";
            return;
        }

        //try to move the todo given by the number from todo-list to done-list
        //print error if the number is not a valid number or does not have task associated with it
        try{
            int index = atoi(num) - 1;
            if(index >= 0 and index < (int)allTodos.size()){
                allDones.emplace_back("x " + currentDate() + " " + allTodos[index]);
                allTodos.erase(allTodos.begin() + index);
                cout << "Marked todo #" << index + 1 <<" as done.\n";  
            }
            else
                throw -1;
        } 
        catch(int errorCode){
            cout << "Error: todo #" << num <<" does not exist.\n";
        }
    }

    //prints report on the screen
    void printReport(){
        cout << currentDate() << " Pending : " << allTodos.size() << " Completed : " << allDones.size() << "\n";
    }
};


int main(int argc, const char* argv[])
{
    Todo myTodo;  //creating todo object
    myTodo.executeCommand(argc, argv);  //executing the given command
    return 0;
}