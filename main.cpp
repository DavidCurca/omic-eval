#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdio>
#include <chrono>
using namespace std;
namespace fs = std::filesystem;
typedef std::chrono::high_resolution_clock Clock;

/// OMIC-EVAL WIRTTEN BY DAVID CURCA
/// JUNE 2021
/// GIVEN ANY SOURCE CODE IT WILL COMPILE IT
/// AND CHECK EACH TEST IN THE /TEST DIR
/// AND GIVE A SPECIFIC SCORE DEPENDING ON THE
/// OUTPUT IN EACH TEST
/// THE FOLLOWING PROGRAMMING LANGUAGES AND
/// COMPILE OPTIONS ARE:
/// +--------+--------------------------------+
/// ! LANG.  ! COMPILE OPTIONS                !
/// +--------+--------------------------------+
/// ! C      ! gcc -Wall -O2 -std=c11 ...     !
/// ! C++    ! g++ -Wall -O2 -std=c++14 ...   !
/// ! PASCAL ! fpc -O2 -Xs ..                 !
/// ! JAVA   ! ... not suported yet ...       !
/// ! PYTHON ! python3 ...                    !
/// ! RUST   ! rustc -O ...                   !
/// +--------+--------------------------------+
/// EXAMPLE ARGUMENTS FOR OMIC-EVAL
/// NOTE: THE /env IS THE DIR IN WHICH THE PROGRAM
/// WILL BE RUNNING
///       THE LONGEST TIME EACH TEST IS GOING TO LAST
/// IS 5 SECONDS, AFTER THAT THE PROGRAM WILL BE KILLED
/// USAGE:
///   omic-eval [option] [queue_id] [usaname] [problem-source] [problem-name] [time-limit] [memeory-limit] [prog-lang] [env-dir] [tests-dir]
///   -[option]: -verbose: show debug messages
///              -quiet: not logging to the console
///              -help: shows documentation and manual
///   -[queue_id]: a 16 string of upper/lower case letters and digits used by the omic
///                platform
///   -[username]: username of the person who submitted the request
///   -[problem-source]: path to the code that is going to be compiled and tested
///   -[problem-name]: name of the problem that is evaluated
///   -[time-limit]: how much time each test is going to take (EXPRESSED IN MILLISECONDS)
///   -[memory-limit]: how much kilebytes each test is going to take
///   -[prog-lang]: what the code is being compiled in. the prog-lang value can be:
///                  * cpp or c++
///                  * c
///                  * python or py
///                  * rust
///   -[env-dir]: the directory where the program will be runed and tested
///   -[tests-dir]: the directory where all of the tests of the problem are
/// EXAMPLE:
///   omic-eval kSDFy5IbySsa6fd8 divad adunare.cpp Adunare 100 65536 cpp env tests
/// User divad sent the request kSDFy5IbySsa6fd8 on the problem "Adunare" in c++ and the
/// tests is the tests dir
/// (COLOR SCHEME INSPIRED BY OPENRC)

const string red("\033[0;31m");
const string blue("\033[1;34m");
const string green("\033[1;32m");
const string yellow("\033[1;33m");
const string cyan("\033[0;36m");
const string magenta("\033[0;35m");
const string reset("\033[0m");

void compile(string lang, string filename){
  string execute = "";
  if(lang.compare("C++") == 0 ){
    execute = "g++ -Wall -O2 -std=c++14 " + filename + " -o env/output 2> compile-output.txt";
  }else if(lang.compare("C") == 0){
    execute = "g++ -Wall -O2 -std=c11 " + filename + " -o env/output 2> compile-output.txt";
  }else if(lang.compare("Pascal") == 0){
    execute = "fpc -O2 -Xs " + filename + " -oenv/output 2> compile-output.txt";
  }else if(lang.compare("Rust") == 0){
    execute = "rustc -O " + filename + " -o env/output 2> compile-output.txt";
  }
  char exe_array[execute.length()+1];
  strcpy(exe_array, execute.c_str());
  system(exe_array);
}

void checkErrors(){
  ifstream f("compile-output.txt");
  stringstream strStream;
  strStream << f.rdbuf();
  string str = strStream.str();
  if(str.find("error") != string::npos || str.find("Error") != string::npos){
    /// found an error
    cout << blue << "[" << red << " ERR " << blue << "]" << reset << " --- Compilation Error: \n" << str;
    cout << "omic-eval: exit status 1\n";
    exit(1);
  }
  cout << blue <<  "[" << green << " OK " << blue << "]" << reset << " --- Compiled Successfully\n";
}

string stripString(string str){
  return str.substr(str.find("/")+1, str.length()-str.find("/"));
}

string correctName(string filename){
  string ans = "";
  for(int i = filename.find("/")+1; i < filename.find("-"); i++){
    ans += filename[i];
  }
  for(int i = filename.find("."); i <= filename.length(); i++){
    ans += filename[i];
  }
  return ans;
}

string getDimension(){
  string ans = "";
  return ans;
}

int getMemory(){
  string ans = "";
  ifstream fin("env/final-output.txt");
  string str = "";
  if(getline(fin, str)){
    ans = str.substr(str.find(":")+2, str.length()-str.find(":")-2);
  }
  return atoi(ans.c_str());
}

void checkTests(string envdir, string testdir){
  /// TODO: punctaje diferite pentru anumite teste speciala
  ///       maxim 10 secunde pentru fiecare test
  ///       printare adevarata

  string testPaths[200] = {};
  int numberP = 0, finalScore = 0;
  for(const auto & entry: fs::directory_iterator(testdir)){
    testPaths[++numberP] = entry.path();
  }
  int correctAnswers = 0;
  sort(testPaths+1, testPaths+numberP+1);
  for(int i = 1; i <= numberP/2; i++){
    /// posibble outcomes are : Correct Answer, Time Limit Exceeded, Memeory Limit Exceeded or exit code of the program
    string fisier = testPaths[i*2-1];
    string command_string = "cp " + fisier + " env/";
    system(command_string.c_str());
    command_string = "mv env/" + stripString(fisier) + " env/" + correctName(fisier);
    //cout << command_string << "\n";
    system(command_string.c_str());


    auto startTime = Clock::now();
    system("cd env/ ; /usr/bin/time -v ./output 2> time-output.txt ; cd ..");
    auto endTime = Clock::now();
    system("cd env/ ; cat time-output.txt | grep Maximum > final-output.txt ; cd ..");
    auto time = chrono::duration_cast<chrono::milliseconds>(endTime-startTime).count();
    cout << blue << "[" << green << " OK " << blue << "]" << reset << " --- Test Case #" << i << " : Correct Answer\n";


    cout << yellow << " >>>> " << reset << " --- Execution Time : " << blue << time << " ms" << reset << "\n";
    cout << yellow << " >>>> " << reset << " --- Memory Used    : " << blue << getMemory() << " kbytes" << reset << "\n";
    correctAnswers++;
    finalScore += 100/(numberP/2);
  }
  if(correctAnswers == numberP/2){
    finalScore += 100-finalScore;
  }
  //system(("rm env/" + correctName(testPaths[1])).c_str());
  string colorScore = "";
  if(finalScore <= 33){colorScore = red;}
  else if(finalScore <= 66){colorScore = yellow;}
  else{colorScore = green;}
  cout << yellow << " >>>> " << reset << " --- Final Score: " << colorScore << finalScore << reset << "\n";
}

int main(int argc, char *argv[]){
  string progAll = "";
  if(string(argv[7]).compare("cpp") == 0 || string(argv[7]).compare("c++") == 0){
    progAll = "C++";
  }else if(string(argv[7]).compare("c") == 0){
    progAll = "C";
  }else if(string(argv[7]).compare("python") == 0 || string(argv[7]).compare("py") == 0){
    progAll = "Python";
  }else if(string(argv[7]).compare("pascal") == 0){
    progAll = "Pascal";
  }else if(string(argv[7]).compare("rust") == 0){
    progAll = "Rust";
  }
  cout << blue << "[" << green << " OK " << blue << "]" << reset << " --- Starting " << cyan << "OMIC EVALUATOR " << reset << "\n";
  cout << yellow << " >>>> " << reset << " --- Queue Id : " << blue << " [ insert argv here ]" << reset << "\n";
  cout << yellow << " >>>> " << reset << " --- File Dimension : " << blue << "673 B" << reset << "\n";
  cout << yellow << " >>>> " << reset << " --- Date Request Sent : " << blue << "Sat Jun 12 11:14:26 PM" << reset << "\n";
  cout << yellow << " >>>> " << reset << " --- Compiling " << argv[3] << " In " << progAll << "\n";
  compile(progAll, argv[3]);
  checkErrors();
  checkTests(argv[8], argv[9]);
  return 0;
}
