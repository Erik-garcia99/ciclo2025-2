#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
int main()
{  int descriptor,grabados;
   char cad[25]="";
       
   descriptor=open("ciudades.txt",O_RDWR);  
   if (descriptor==0)
      printf("El archivo no se pudo abrir");
else
{  printf("\n escriba un numero:");
   scanf("%s",cad); 
   lseek(descriptor,19,SEEK_SET);
   grabados=write(descriptor,cad,strlen(cad) );
   if (grabados >0)
       printf("Se grabaron %d bytes", grabados);
   else
       printf("No se pudo grabar");
   close(descriptor);
}
return(0);
}