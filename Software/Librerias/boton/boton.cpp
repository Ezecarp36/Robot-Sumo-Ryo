#include "boton.h"

Boton::Boton(int p){
pin = p;
pinMode(pin, INPUT_PULLUP);
}

void Boton::SetFlanco(bool f){
    flanco = f;
    estado_anterior = !flanco;

}

bool Boton::GetIsPress(){
    bool estado_actual = digitalRead(pin);
    bool estado = (estado_anterior != estado_actual) && estado_actual == flanco;
    delay(100);
    return estado;
}