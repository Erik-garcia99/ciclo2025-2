#include<stdio.h>

#define N 10
#define true 1
#define false 0

int main(){

	int vector[N] = {1,6,3,5,6,8,3,2,7,2};

	int i;
	int j;       
	for(i=0;i<N-1;i++){

		for(j=i+1;j<N;j++){
			
			int actual = vector[i];
			int cmp = vector[j]; 

			if(actual > cmp){
			    vector[i]= cmp;            
		    	vector[j]= actual;
			}   

			else if(actual == cmp){                                               
				continue;
            
			}

		}
	}

	for(int i=0; i< N; i++){
	
		printf("%d - ", vector[i]);
	}


	
	return 0;
}



