#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 9999
#define MAGIC 0x4C

const char *led_names[] = {
    "LED0_Red",
    "LED0_Green",
    "LED0_Blue"
};

void set_led(const char *name, int brightness) {
    char path[128];

    // Установим режим none → позволяет вручную управлять яркостью
    snprintf(path, sizeof(path), "/sys/class/leds/%s/trigger", name);
    FILE *trigger = fopen(path, "w");
    if (trigger) {
        fputs("none", trigger);
        fclose(trigger);
    }

    // Устанавливаем яркость
    snprintf(path, sizeof(path), "/sys/class/leds/%s/brightness", name);
    FILE *bright = fopen(path, "w");
    if (bright) {
        fprintf(bright, "%d", brightness);
        fclose(bright);
    }
}

int main() {
    int sock;
    struct sockaddr_in addr;
    unsigned char buf[8];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return 1;
    }

    while (1) {
        ssize_t len = recv(sock, buf, sizeof(buf), 0);
        if (len < 4 || buf[0] != MAGIC) continue;

        for (int i = 0; i < 3; i++) {
            set_led(led_names[i], buf[i + 1]);
        }
    }

    close(sock);
    return 0;
}

