#include <fstream>
#include <cstring>
using namespace std;

ifstream fin("adunare.in");
ofstream fout("adunare.out");

int main(){
	int a,b;
	fin >> a >> b;
	fout << a+b << "\n";
	return 0;
}
