#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>

#define PORT 9999
#define MAGIC 0x4C

void send_packet(int sock, struct sockaddr_in *addr, unsigned char r, unsigned char g, unsigned char b) {
    unsigned char buf[4] = {MAGIC, r, g, b};
    sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)addr, sizeof(*addr));
}

void delay_ms(int ms) {
    struct timespec req = {
        .tv_sec = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000L
    };
    nanosleep(&req, NULL);
}

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));

    // === 1. Включение/выключение поочерёдно каждого LED ===
    for (int i = 0; i < 3; ++i) {
        unsigned char r = 0, g = 0, b = 0;
        if (i == 0) r = 255;
        else if (i == 1) g = 255;
        else if (i == 2) b = 255;

        send_packet(sock, &addr, r, g, b);
        delay_ms(500);
        send_packet(sock, &addr, 0, 0, 0);
        delay_ms(250);
    }

    // === 2. Блинк всеми ===
    for (int i = 0; i < 3; ++i) {
        unsigned char r = (i == 0) ? 255 : 0;
        unsigned char g = (i == 1) ? 255 : 0;
        unsigned char b = (i == 2) ? 255 : 0;

        for (int j = 0; j < 3; ++j) {
            send_packet(sock, &addr, r, g, b);
            delay_ms(100);
            send_packet(sock, &addr, 0, 0, 0);
            delay_ms(100);
        }
    }

    // === 3. Радуга (плавный градиент) ===
    // HSV → RGB с шагом по H
    for (int h = 0; h < 360; h += 3) {
        float s = 1.0f, v = 1.0f;
        float C = v * s;
        float X = C * (1 - fabsf(fmodf(h / 60.0f, 2) - 1));
        float m = v - C;

        float r_, g_, b_;
        if (h < 60)      { r_ = C; g_ = X; b_ = 0; }
        else if (h < 120){ r_ = X; g_ = C; b_ = 0; }
        else if (h < 180){ r_ = 0; g_ = C; b_ = X; }
        else if (h < 240){ r_ = 0; g_ = X; b_ = C; }
        else if (h < 300){ r_ = X; g_ = 0; b_ = C; }
        else             { r_ = C; g_ = 0; b_ = X; }

        unsigned char r = (unsigned char)((r_ + m) * 255);
        unsigned char g = (unsigned char)((g_ + m) * 255);
        unsigned char b = (unsigned char)((b_ + m) * 255);

        send_packet(sock, &addr, r, g, b);
        delay_ms(50);
    }

    // Завершение: выключаем всё
    send_packet(sock, &addr, 0, 0, 0);
    close(sock);
    return 0;
}

