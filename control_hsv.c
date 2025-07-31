// send_hsv.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DEST_PORT 9999
#define DEST_IP "255.255.255.255"

struct HSV {
    float h; // 0..360
    float s; // 0..1
    float v; // 0..1
};

struct RGB {
    int r; // 0..255
    int g;
    int b;
};

// Конвертация HSV -> RGB
struct RGB hsv_to_rgb(struct HSV hsv) {
    float h = hsv.h;
    float s = hsv.s;
    float v = hsv.v;

    float c = v * s;
    float x = c * (1 - fabsf(fmodf(h / 60.0f, 2) - 1));
    float m = v - c;

    float r1, g1, b1;

    if (h < 60) {
        r1 = c; g1 = x; b1 = 0;
    } else if (h < 120) {
        r1 = x; g1 = c; b1 = 0;
    } else if (h < 180) {
        r1 = 0; g1 = c; b1 = x;
    } else if (h < 240) {
        r1 = 0; g1 = x; b1 = c;
    } else if (h < 300) {
        r1 = x; g1 = 0; b1 = c;
    } else {
        r1 = c; g1 = 0; b1 = x;
    }

    struct RGB rgb;
    rgb.r = (int)((r1 + m) * 255);
    rgb.g = (int)((g1 + m) * 255);
    rgb.b = (int)((b1 + m) * 255);

    if (rgb.r < 0) rgb.r = 0; if (rgb.r > 255) rgb.r = 255;
    if (rgb.g < 0) rgb.g = 0; if (rgb.g > 255) rgb.g = 255;
    if (rgb.b < 0) rgb.b = 0; if (rgb.b > 255) rgb.b = 255;

    return rgb;
}

void usage(const char *prog) {
    printf("Usage:\n");
    printf("  %s --hue <0-360> --sat <0-100> --brightness <0-100> [--demo]\n", prog);
    printf("Examples:\n");
    printf("  %s --hue 210 --sat 50 --brightness 80\n", prog);
    printf("  %s --demo\n", prog);
}

// Отправка UDP пакета с RGB
int send_packet(int r, int g, int b) {
    int sockfd;
    struct sockaddr_in addr;
    unsigned char packet[4];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("setsockopt");
        close(sockfd);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DEST_PORT);
    addr.sin_addr.s_addr = inet_addr(DEST_IP);

    packet[0] = 0x4C; // magic byte
    packet[1] = (unsigned char)r;
    packet[2] = (unsigned char)g;
    packet[3] = (unsigned char)b;

    int sent = sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (sent != sizeof(packet)) {
        perror("sendto");
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 0;
}

// Демонстрационный режим радуги
void demo_rainbow() {
    printf("Starting demo rainbow...\n");
    for (int deg = 0; deg < 360; deg += 5) {
        struct HSV hsv = { .h = (float)deg, .s = 1.0f, .v = 1.0f };
        struct RGB rgb = hsv_to_rgb(hsv);
        printf("Hue=%3d -> RGB(%3d,%3d,%3d)\n", deg, rgb.r, rgb.g, rgb.b);
        send_packet(rgb.r, rgb.g, rgb.b);
        usleep(50000); // 50 ms
    }
    printf("Demo finished.\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    int demo_mode = 0;
    int hue = -1;
    int sat = -1;
    int brightness = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--hue") == 0 && i + 1 < argc) {
            hue = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--sat") == 0 && i + 1 < argc) {
            sat = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--brightness") == 0 && i + 1 < argc) {
            brightness = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--demo") == 0) {
            demo_mode = 1;
        } else {
            usage(argv[0]);
            return 1;
        }
    }

    if (demo_mode) {
        demo_rainbow();
        return 0;
    }

    if (hue < 0 || hue > 360 || sat < 0 || sat > 100 || brightness < 0 || brightness > 100) {
        fprintf(stderr, "Invalid or missing parameters.\n");
        usage(argv[0]);
        return 1;
    }

    struct HSV hsv;
    hsv.h = (float)hue;
    hsv.s = ((float)sat) / 100.0f;
    hsv.v = ((float)brightness) / 100.0f;

    struct RGB rgb = hsv_to_rgb(hsv);

    printf("Sending HSV(%d, %d%%, %d%%) -> RGB(%d,%d,%d)\n",
           hue, sat, brightness, rgb.r, rgb.g, rgb.b);

    if (send_packet(rgb.r, rgb.g, rgb.b) != 0) {
        fprintf(stderr, "Failed to send packet\n");
        return 1;
    }

    return 0;
}
