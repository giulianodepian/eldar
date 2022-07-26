#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char rangeLow[8 + 1];
    char rangeHigh[8 + 1];
    unsigned char len;
    int id;
} range_t;

typedef struct {
    char label[12 + 1];
    int id;
} card_t;

void valueToInteger(char *fvalue, char *value, int valueSize);
bool checkCardNumber(char *card);
bool checkCard(char *card, char *cardLabel);

int main() {
    char value[13 + 1];
    char card[19 + 1];
    char cardLabel[12 + 1];
    char securityCode[3 + 1];
    bool cardCheckStatus;
    char ivalue[12 + 1];
    char trash;
    memset(ivalue, 48, 12 + 1);
    printf("Inserte monto de compra con dos decimales para centavos: ");
    fgets(value, sizeof(value), stdin);
    //Si se inserto un monto mayor a 13 digitos, se vacia el buffer stdin, ignorando los digitos de mas
    //Se pone un caracter \n en el stdin por si se escibe un monto valido, el usuario no tenga que presionar dos veces enter.
    ungetc('\n', stdin);
    while((trash = getchar()) != '\n' && trash != EOF);

    //Se elimina el newline del string del monto en caso de que haya uno.
    if (value[strlen(value) - 1] == '\n') value[strlen(value) - 1] = '\0';
    
    //Convertir el monto en el formato que corresponde al request
    valueToInteger(ivalue, value, sizeof(ivalue));
    
    //Se elimina el newline del string del numero de tarjeta en caso de que haya uno.
    printf("Inserte numero de tarjeta: ");
    fgets(card, sizeof(card), stdin);
    if (card[strlen(card) - 1] == '\n') card[strlen(card) - 1] = '\0';

    //checkCardNumber valida que el string solo tenga digitos, en caso de que tenga caracteres que no son digitos devuelve false.
    while(strlen(card) < 13 || !checkCardNumber(card)) {
        printf("Numero de tarjeta invalido, insertelo nuevamente: ");
        fgets(card, sizeof(card), stdin);
        if (card[strlen(card) - 1] == '\n') card[strlen(card) - 1] = '\0';
    }

    //Checkea si la tarjeta corresponde a una tarjeta de la lista de ranges.dat
    //Devuelve true si se encuentra una correspondencia, falso en caso contrario
    cardCheckStatus = checkCard(card, cardLabel);
    if (!cardCheckStatus) {
        printf("TARJETA NO SOPORTADA\n");
        return 1;
    }
    printf("%s\n", cardLabel);

    printf("Inserte el codigo de seguridad: ");
    fgets(securityCode, sizeof(securityCode), stdin);
    //Se elimina el newline del string del ncodigo de seguridad en caso de que haya uno.
    if(securityCode[strlen(securityCode) - 1] == '\n') securityCode[strlen(securityCode) - 1] = '\0';
    while (strlen(securityCode) < 3) {
        printf("El numero de seguridad debe ser de 3 digitos: ");
        fgets(securityCode, sizeof(securityCode), stdin);
    }
    
    //Formacion del mensaje request con el formato que corresponde
    char requestMessage[4+2+19+12+3+1];
    char cardlen[2 + 1];
    sprintf(cardlen, "%ld", strlen(card));
    strcat(requestMessage, "0020");
    strcat(requestMessage, cardlen);
    strcat(requestMessage, card);
    strcat(requestMessage, ivalue);
    strcat(requestMessage, securityCode);
    printf("%s\n", requestMessage);
    
    /*
    Inicio de las operaciones del socker
    int socket = sockerCreate();
    sockerConnect(socket, IP, Port);
    sockerWrite(socket, requestMessage)
    char responseMessage[4+2+1];
    if (socketRead(socker, responseMessage, 5000) != 0) {
        printf("ERROR DE COMUNICACION\n");
        return 1;
    }
    if (responseMessage[4] == '0' && responseMessage[5] == '0') {
        printf("APROBADO\n");
    } else {
        printf("RECHAZADO\n")
    }
    */

    return 0;
}

void valueToInteger(char *ivalue, char *value, int valueSize) {
    int i = 0;
    bool pointDetected = false;
    int counterDecimals = 0;

    //Elimino lo decimales que hay demas.
    while (value[i] != '\0') {
        if (pointDetected) counterDecimals++;
        if (value[i] == '.') pointDetected = true;
        else if (counterDecimals >= 2) {
            value[i+1] = '\0';
            break;
        }
        i++;
    }
    ivalue[valueSize-1] = '\0';
    i = 0;
    int k = valueSize - (strlen(value) - 1) - 2;
    
    //Inserto el monto en orden invertido e elimino el punto para que quede en el formato pedido.
    //ivalue esta inizializado con todos 0 para cumplir con el formato.
    while (value[i] != '\0') {
        if (value[i] == '.') k--;
        else ivalue[k] = value[i];
        i++;
        k++;
    }

    //En caso de que haya decimales, tengo que correr todos los numeros a la derecha. 
    if (pointDetected) {
        for (int j = valueSize - 2; j >= 0; j--) {
            ivalue[j] = ivalue[j-1];
            if (ivalue[j-1] == '0') break;
        }
    }
}

bool checkCardNumber(char *card) {
    for (int i = 0; i < strlen(card); i++) {
        if (!isdigit(card[i])) return false;
    }
    return true;
}

bool checkCard(char *card, char *cardLabel) {
    char *firstEight = malloc(9);

    //Pongo los primeros 8 digitos de la tarjeta en un nuevo arreglo
    for (int i = 0; i < 8; i++) {
        firstEight[i] = card[i];
    }

    //al final del arreglo un Null Terminator
    firstEight[8] = '\0';
    FILE *rangeFile;
    bool matchFound = false;
    range_t rangeInfo;
    rangeFile = fopen("ranges.dat", "r");
    if (rangeFile == NULL) {
        printf("Error al intentar abrir el archivo ranges.dat");
        return false;
    }

    //Leo el archivo ranges.dat para corrobar que haya una tarjeta que corresponga a la ingresada 
    while(!matchFound && fread(&rangeInfo, sizeof(range_t), 1, rangeFile)) {
        if (strcmp(firstEight, rangeInfo.rangeLow) >= 0 && strcmp(firstEight, rangeInfo.rangeHigh) <= 0) {
            if (strlen(card) == rangeInfo.len) {
                matchFound = true;
            }
        }
    }
    fclose(rangeFile);
    free(firstEight);

    //Si se encontro una tarjeta que corresponde a la ingresada, se busca por ID dentro del archivo cards.dat para obtener el Label.
    if (matchFound) {
        FILE *cardFile;
        card_t cardInfo;
        bool cardFound = false;
        cardFile = fopen("cards.dat", "r");
        if (cardFile == NULL) {
            printf("Error al intentar abrir el archivo cards.dat");
            return false;
        }
        while(!cardFound && fread(&cardInfo, sizeof(card_t), 1, cardFile)) {
            if (rangeInfo.id == cardInfo.id) {
                cardFound = true;
                strcpy(cardLabel, cardInfo.label);
            }
        }
        fclose(cardFile);
    }
    return matchFound;
}