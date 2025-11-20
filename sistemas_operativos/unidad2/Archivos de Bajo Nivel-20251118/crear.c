#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
      
int main(int totarg, char *arg[])
{  int descriptor;

   if (totarg!=2)
   {   printf("Numero de parametros invalido");
      exit(0);
   }
   descriptor=creat(arg[1],0700);
   if (descriptor==0)
      printf("El archivo no se pudo crear");
   else
      printf("El archivo %s tiene el descriptor %d", arg[1], descriptor);
      
return(0);
}
