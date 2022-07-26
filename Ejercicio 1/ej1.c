#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int main() {
    FILE *fptr;
    char string[15];
    long number;
    char *end;
    char trash;

    fptr = fopen("./file.txt", "w");
    printf("Ingresa un numero de 10 digitos maximo:\n");
    fgets(string, 12, stdin);
    
    //Convertir string a long
    number = strtol(string, &end, 10);

    //Verificar si el numero contenia caracteres que no eran digitos. end termina donde termino la conversion
    //La conversion termina si encuentra un caracter q no sea un digito o si encontro un null terminator.
    //strtol ignora espacios
    if (end == string || isalpha(*end)) number = -1;
    while (number != 0) {
        if (number > 9999999999) {
            printf("Debes insertar un numero que tenga como maximo 10 digitos!\n");
            //Limpia el buffer en caso de que se inserte un numero de mas de 10 digitos
            while((trash = getchar()) != '\n' && trash != EOF);
        }
        else if (number == -1) printf("Debes insertar un numero!\n");
        else fprintf(fptr, "%010ld\r\n", number);
        printf("Ingresa un numero de 10 digitos maximo:\n");
        fgets(string, 11, stdin);
        number = strtol(string, &end, 10);
        if (end == string || isalpha(*end)) number = -1;
    }
    
    fclose(fptr);
    return 0;
}