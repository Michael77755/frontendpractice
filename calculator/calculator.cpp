#include "calculator.h"

bool checkIfValidOperator(char op) {
    switch (op) {
        case '+':
        case '-':
        case '/':
        case '*':
            return true;
        default:
            return false;
    }
}

double calc(int numberone, char op, int numbertwo) {
    switch (op) {
        case '+': return numberone + numbertwo;
        case '-': return numberone - numbertwo;
        case '*': return numberone * numbertwo;
        case '/': return static_cast<double>(numberone) / numbertwo;
        default:  return 0.0;
    }
}