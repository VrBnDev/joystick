#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include <math.h>
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"

// ======= CONFIGURAÇÕES ======= //
#define HOST "192.168.137.228"  // Substitua pelo IP do servidor
#define PORT 5000
#define INTERVALO_MS 1000
#define LED_BLUE 12
#define LED_RED 13
#define CENTER_VALUE 2048
// ============================= //

float calcular_angulo(uint16_t x_raw, uint16_t y_raw) {
    const float max_adc = 4095.0;
    // Calcular deslocamento em relação ao centro
        int deltaX = x_raw - CENTER_VALUE;
        int deltaY = y_raw - CENTER_VALUE;

        // Ignorar ruído quando o joystick está parado
        if (abs(deltaX) < 50 && abs(deltaY) < 50) {
            printf("Centro\n");
        } else {
            // Calcular ângulo
            float angle_rad = atan2((float)deltaY, (float)deltaX);
            float angle_deg = angle_rad * 180.0f / M_PI;

            if (angle_deg < 0.0f)
                angle_deg += 360.0f;

            return angle_deg;
        }

}

int main() {
    stdio_init_all();
    
    adc_init();
    adc_gpio_init(26); // X conectado ao GPIO26 (ADC0)
    adc_gpio_init(27); // Y conectado ao GPIO27 (ADC1)

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
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Falha na conexão Wi-Fi\n");
        return 1;
    }

    printf("Conectado! IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    while (1) {
        // Leia o eixo X
        adc_select_input(0); // GPIO26 (ADC0)
        uint16_t eixo_x = adc_read();

        // Leia o eixo Y
        adc_select_input(1); // GPIO27 (ADC1)
        uint16_t eixo_y = adc_read();
        
        // Calcula o ângulo com X e Y corretos
        float angulo = calcular_angulo(eixo_x, eixo_y);

        // Monta a URL com o valor do ângulo
        char path_buffer[100];
        sprintf(path_buffer, "/angulo?valor=%.2f", angulo);

        printf("Ângulo: %.2f\n", angulo);
        printf("Enviando comando: %s\n", path_buffer);

        EXAMPLE_HTTP_REQUEST_T req = {0};
        req.hostname = HOST;
        req.url = path_buffer;
        req.port = PORT;
        req.headers_fn = http_client_header_print_fn;
        req.recv_fn = http_client_receive_print_fn;

        int result = http_client_request_sync(cyw43_arch_async_context(), &req);

        if (result == 0) {
            printf("Comando enviado com sucesso!\n");
        } else {
            printf("Erro ao enviar comando: %d\n", result);
        }

        sleep_ms(INTERVALO_MS);
    }
}
