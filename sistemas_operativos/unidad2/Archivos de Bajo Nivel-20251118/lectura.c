#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
int main()
{  int descriptor,leidos, cont=0;
   char carac, cad[25];
       
   descriptor=open("ciudades.txt",O_RDONLY);  
   if (descriptor==0)
      printf("El archivo no se pudo abrir");
else
{  lseek(descriptor,19,SEEK_SET);
  do
  { cont=0;
    cad[0]='\x0';
    do
    {  leidos=read(descriptor,&carac,1);
       cad[cont]=carac;
       printf("%c\n",cad[cont]);
       cont++;
       
    }
  while(leidos!=0 && carac!='\n' );
    cad[cont]='\x0'; 
    printf("%s",cad);
  
  }while(leidos!=0);
}
return(0);
}