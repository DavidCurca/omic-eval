#include <fstream>
using namespace std;
int a,b;

ifstream fin("adunare.in");
ofstream fout("adunare.out");

int main(){
	fin >> a >> b;
	fout << a + b << "\n";
	return 0;
}
