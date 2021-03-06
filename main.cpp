#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <ctime>
#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdio>
#include <chrono>
using namespace std;
namespace fs = std::filesystem;
typedef std::chrono::high_resolution_clock Clock;
int globalMem,globalTime;
string queueId = "",problemName = "",preposition = "",arg_test = "";

ofstream fout;

const string red("\033[1;31m");
const string blue("\033[1;34m");
const string green("\033[1;32m");
const string yellow("\033[1;33m");
const string cyan("\033[0;36m");
const string magenta("\033[0;35m");
const string reset("\033[0m");

/// TODO:
///   - make 2 secondl time out       [DONE]
///   - check mem and time boundaries [DONE]
///   - index tests dir               [DONE]
///   - give compact final result     [DONE]
///   - clear any taces after         [DONE]
///   - delete env-dir argument       [DONE]
///   - make code run safely

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
    fout << blue << "[" << red << " ERR " << blue << "]" << reset << " --- Compilation Error: \n" << str;
    fout << "omic-eval: exit status 1\n";
    fout.close();
    exit(1);
  }
  fout << blue <<  "[" << green << " OK " << blue << "]" << reset << " --- Compiled Successfully\n";
}

string stripString(string str){
  return str.substr(str.find_last_of("/")+1, str.length()-str.find_last_of("/"));
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

char * getTime(){
  auto end = chrono::system_clock::now();
  time_t end_time = chrono::system_clock::to_time_t(end);
  return ctime(&end_time);
}

string getDimension(string filename){
  string ans = "";
  ifstream f(filename, ios::binary);
  f.seekg(0, ios::end);
  int size = f.tellg();
  ans = to_string(size) + " B";
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

bool isEmpty(string str){
  return str.find_first_not_of("\t\n ") == string::npos;
}

string strip(string str){
  if(isEmpty(str)){
    return "";
  }
  string ans = str.substr(str.find_first_not_of(' '), str.find_last_not_of(' ')+1);
  ans = ans.substr(0, ans.find_last_not_of(' ')+1);
  return ans;
}

string checkTest(int index, int time, int mem){
  string ans = "Correct Answer";
  ifstream fTest(((string)("env/" + preposition + ".out")).c_str());
  ifstream fCorrect(((string)("tests/" + problemName + "/" + preposition + "-" + to_string(index) + ".out")).c_str());
  /// citeste fiecare linie dupa ce ii da strip si apoi verifica
  /// daca sunt egaleASAsa
  /// couaS
  bool ok = true;
  if(globalTime < time){
   return "Time Out";
  }else if(globalMem < mem){
   return "Memory Exceeded";
  }
  while(!fTest.eof() && !fCorrect.eof()){
   string str1 = "", str2 = "";
   getline(fTest, str1); str1 = strip(str1);
   getline(fCorrect, str2); str2 = strip(str2);
   if(!(str1.compare(str2) == 0)){
      ok = false;
      break;
    }
  }
  if(fTest.eof() && fCorrect.eof()){
    /// reached end of both filesSAS
    if(ok){
       ans = "Correct.";
    }else{
       ans = "Incorrect.";
    }
  }else{
    ans = "Incorrect.";
  }
  return ans;
}

void checkTests(string testdir){
  string testPaths[200] = {};
  int numberP = 0;
  double finalScore = 0;
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
    //cout << correctName(fisier) << "\n";
    command_string = "mv env/" + stripString(fisier) + " env/" + preposition + ".in";
    //cout << command_string << "\n";
    system(command_string.c_str());
    //system("cat env/adunare.in");
    //system("cd env/ ; ./output ; cd ..");
    //system("cat env/adunare.out");

    auto startTime = Clock::now();
    system("cd env/ ; /usr/bin/time -v timeout 2 ./output 2> time-output.txt ; cd ..");
    auto endTime = Clock::now();
    system("cd env/ ; cat time-output.txt | grep Maximum > final-output.txt ; cd ..");
    auto time = chrono::duration_cast<chrono::milliseconds>(endTime-startTime).count();
    int mem = getMemory();
    string testResult = checkTest(i, time, mem);
    if(testResult.compare("Correct.") == 0){
      fout << blue << "[" << green << " OK " << blue << "]" << reset << " --- Test Case #" << i << "   : " << green << "Correct." << reset << "\n";
      correctAnswers++;
      finalScore += 100/(numberP/2);
    }else{
      fout << blue << "[" << red   << " !! " << blue << "]" << reset << " --- Test Case #" << i << "   : " << red   << testResult << reset << "\n";
    }


    fout << yellow << " >>>> " << reset << " --- Execution Time : " << blue << time << " ms" << reset << "\n";
    fout << yellow << " >>>> " << reset << " --- Memory Used    : " << blue << mem << " kbytes" << reset << "\n";
  }
  if(correctAnswers == numberP/2){
    finalScore += 100-finalScore;
  }
  //system(("rm env/" + correctName(testPaths[1])).c_str());
  string colorScore = "";
  if(finalScore <= 33){colorScore = red;}
  else if(finalScore <= 66){colorScore = yellow;}
  else{colorScore = green;}
  fout << yellow << " >>>> " << reset << " --- Final Score: " << colorScore << finalScore << reset << "\n";
}

void lovercase(string &str){
  for_each(str.begin(), str.end(), [](char &c){
    c = ::tolower(c);
  });
}

int main(int argc, char *argv[]){
  queueId = argv[1];
  string endSecq = "-eval.txt";
  string outputFile = argv[1] + endSecq;
  fout.open(outputFile.c_str());
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
  system("mkdir env/");
  globalTime = atoi(argv[5]);
  globalMem = atoi(argv[6]);
  fout << blue << "[" << green << " OK " << blue << "]" << reset << " --- Starting " << reset << "OMIC EVALUATOR " << reset << "\n";
  fout << yellow << " >>>> " << reset << " --- Queue Id : " << blue << argv[1] << reset << "\n";
  fout << yellow << " >>>> " << reset << " --- User In Question : " << blue << argv[2] << reset << "\n";
  fout << yellow << " >>>> " << reset << " --- Problem Name : " << blue << argv[4] << reset << "\n"; problemName = argv[4];
  fout << yellow << " >>>> " << reset << " --- File Dimension : " << blue << getDimension(argv[3]) << reset << "\n";
  fout << yellow << " >>>> " << reset << " --- Date Request Sent : " << blue << getTime() << reset;
  fout << yellow << " >>>> " << reset << " --- Compiling " << argv[3] << " In " << progAll << "\n";
  preposition = problemName;
  lovercase(preposition);
  //cout << preposition << "\n";
  compile(progAll, argv[3]);
  checkErrors();
  checkTests(string(argv[8]) + "/" + problemName + "/");
  system("rm -R env/ ; rm compile-output.txt");
  fout.close();
  return 0;
}
