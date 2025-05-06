#include <stdio.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "hardware/adc.h"
#include <math.h>
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"

// ======= CONFIGURAÇÕES ======= //
#define HOST "192.168.210.70"  // Substitua pelo IP do servidor
#define PORT 5000
#define INTERVALO_MS 50
#define LED_BLUE 12
#define LED_RED 13
// ============================= //

float calcular_angulo(uint16_t x_raw, uint16_t y_raw) {
    const float max_adc = 4095.0;
    float x = ((float)x_raw - (max_adc / 2)) / (max_adc / 2);
    float y = ((float)y_raw - (max_adc / 2)) / (max_adc / 2);

    float rad = atan2(y, x);
    float deg = rad * 180.0 / M_PI + 90.0;
    if (deg < 0) deg += 360.0;
    return deg;
}

int main() {
    stdio_init_all();
    
    adc_init();
    adc_gpio_init(26); // X
    adc_gpio_init(27); // Y

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);

    printf("\nIniciando cliente HTTP...\n");

    if (cyw43_arch_init()) {
        printf("Erro ao inicializar Wi-Fi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    printf("Conectando a %s...\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, 
                                           CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Falha na conexão Wi-Fi\n");
        return 1;
    }

    printf("Conectado! IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    while (1) {
        adc_select_input(0);
        uint16_t eixo_y = adc_read();
        adc_select_input(1);
        uint16_t eixo_x = adc_read();
        
        float angulo = calcular_angulo(eixo_x, eixo_y);
        char path_buffer[100]; // espaço para guardar a URL com o ângulo
        sprintf(path_buffer, "/angulo?valor=%.2f", angulo); // monta a URL com o valor do ângulo
        printf("Ângulo: %2.f\n", angulo);

        EXAMPLE_HTTP_REQUEST_T req = {0};
        req.hostname = HOST;
        req.url = path_buffer;
        req.port = PORT;
        req.headers_fn = http_client_header_print_fn;
        req.recv_fn = http_client_receive_print_fn;


            printf("Enviando comando: %s\n", angulo);
            int result = http_client_request_sync(cyw43_arch_async_context(), &req);

            if (result == 0) {
                printf("Comando enviado com sucesso!\n");
            } else {
                printf("Erro ao enviar comando: %d\n", result);
            }

            sleep_ms(1000);  // Evita múltiplos envios rápidos
        }

        sleep_ms(INTERVALO_MS);
}
