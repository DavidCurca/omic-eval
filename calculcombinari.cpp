#include <iostream>
#define NMAX 502
using namespace std;
int v[NMAX][10001];
int n,k;
void sum(int a[],int b[],int c[]){
int i,t=0;
for(i=1;i<=a[0]||i<=b[0];i++)
{
    int k=a[i]+b[i]+t;
    c[i]=k%10;
     t=k/10;
}
c[0]=i-1;
if(t>0){
    c[i]=t;
    c[0]=i;
}

}
int main()
{
    //cin >> n >> k;
    v[0][0] = v[0][1] = 1;
    v[1][0]=v[1][1]=1;
    for(int i = 2; i <= n; i++){
        v[i][0]=v[i][1] = 1;
        for(int j = i-1; j >= 1; j--){
            ///v[j] = v[j]+v[j-1];
            sum(v[j],v[j-1],v[j]);
        }
    }
    ///cout << v[k] << "\n";
    //for(int i=v[k][0];i>=1;i--)
    //     cout<<v[k][i];
    return 0;
}
