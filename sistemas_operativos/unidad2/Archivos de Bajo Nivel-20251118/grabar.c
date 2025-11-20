#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
   
int main()
{  int descriptor;
   int num,grabados;
   char buffer [255],otro;
   
   descriptor=open("num.txt",O_WRONLY|O_CREAT,0700); 
   if (descriptor==0)
      printf("El archivo no se pudo crear");
   else
{
      printf("El archivo tiene el descriptor %d", descriptor);
do
  {
   printf("\n escriba un numero:");
   scanf("%d",&num); 
   sprintf(buffer,"Numero: %d",num);
   puts(buffer);
   grabados=write(descriptor,buffer,strlen(buffer) );
   printf("%d bytes grabados, \nDesea grabar otra numero", grabados);
   fflush(stdin);
   scanf("%c",&otro);
 } while(otro!='n'); 
close(descriptor);
}
return(0);
}
