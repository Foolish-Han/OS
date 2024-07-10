#include <stdio.h>
int main() {
	int n;
	scanf("%d", &n);
	int s=n,q=0;
	while(n){
		q=q*10+n%10;
		n/=10;
	}
	if (s==q) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
