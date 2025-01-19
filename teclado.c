#include "pico/stdlib.h"
#include <stdio.h>

#define ROWS 4
#define COLS 4

#define LED_G 11
#define LED_B 12
#define LED_R 13

#define BUZZER_PIN 19

// Pinos do teclado matricial
const uint row_pins[ROWS] = {2, 3, 4, 5};  // GPIO para linhas
const uint col_pins[COLS] = {6, 7, 8, 9};  // GPIO para colunas

// Mapeamento de teclas do teclado matricial
const char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void init_pins() {
    // Configurar pinos das linhas como saída
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 1);
    }

    // Configurar pinos das colunas como entrada com pull-up
    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_up(col_pins[i]);
    }

    // Configurar pino do buzzer como saída
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);

    // Configurar pino do led verde como saída
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_put(LED_G, 0);

    // Configurar pino do led azul como saída
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);
    gpio_put(LED_B, 0);

    // Configurar pino do led vermelho como saída
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_put(LED_R, 0);
}

char scan_keypad() {
    for (int row = 0; row < ROWS; row++) {
        // Ativar linha atual
        gpio_put(row_pins[row], 0);

        for (int col = 0; col < COLS; col++) {
            if (!gpio_get(col_pins[col])) { // Verificar se a tecla foi pressionada
                sleep_ms(20); // Debounce
                if (!gpio_get(col_pins[col])) { // Confirmar tecla pressionada
                    gpio_put(row_pins[row], 1);
                    return keys[row][col];
                }
            }
        }

        // Desativar linha atual
        gpio_put(row_pins[row], 1);
    }

    return 0; // Nenhuma tecla pressionada
}

/*Função buzz:

Calculo de atraso em microssegundos: int atraso_us = 1000000 / (2 * frequencia_hz);
Calculo do número de ciclos: int ciclos = (duracao_ms * 1000) / (2 * atraso_us);
Laço for para alternar o estado do pino do buzzer: for (int i = 0; i < ciclos; i++) { gpio_put(BUZZER_PIN, 1); sleep_us(atraso_us); gpio_put(BUZZER_PIN, 0); sleep_us(atraso_us); }

*/
void buzz(int duracao_ms, int frequencia_hz) {
    int atraso_us = 1000000 / (2 * frequencia_hz);
    int ciclos = (duracao_ms * 1000) / (2 * atraso_us);
    for (int i = 0; i < ciclos; i++) {
        gpio_put(BUZZER_PIN, 1);
        sleep_us(atraso_us);
        gpio_put(BUZZER_PIN, 0);
        sleep_us(atraso_us);
    }
}

//Função control_components:
//Chamada da função buzz ao pressionar a tecla #: buzz(250, 400);
void control_components(char key, bool pressed) {
    switch (key) {
        case 'A': // Controla LED verde
            gpio_put(LED_G, pressed);
            break;
        case 'B': // Controla LED azul
            gpio_put(LED_B, pressed);
            break;
        case 'C': // Controla LED vermelho
            gpio_put(LED_R, pressed);
            break;
        case 'D':
            gpio_put(LED_G, pressed);
            gpio_put(LED_B, pressed);
            gpio_put(LED_R, pressed);
            break;
        case '#': // Controla o buzzer
            if (pressed) {
                buzz(250, 400); // 400 Hz por 250 ms
            }
            break;
        default:
            break;
    }
}

int main() {
    stdio_init_all();
    init_pins();

    while (true) {
        char key = scan_keypad();
        if (key) {
            printf("Tecla pressionada: %c\n", key);
            control_components(key, true);
        } else {
            // Desliga LEDs quando nenhuma tecla está pressionada
            gpio_put(LED_G, 0);
            gpio_put(LED_B, 0);
            gpio_put(LED_R, 0);
            gpio_put(BUZZER_PIN, 0);
        }
        sleep_ms(100);
    }
}
