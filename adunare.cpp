#include <fstream>
using namespace std;

ifstream fin("adunare.in");
ofstream fout("adunare.out");

int main(){
	int a,b;
	fin >> a >> b;
	if((a+b)%2 == 0){
		fout << a+b << "\n";
	}else{
		fout << a+b-1 << "\n";
	}
	return 0;
}

