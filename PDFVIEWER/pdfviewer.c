#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <pdf-file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    // Seek to the end to get file size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    // Allocate buffer to hold the entire file
    char *buffer = (char *)malloc(filesize);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    // Read the entire file into buffer
    size_t read_size = fread(buffer, 1, filesize, file);
    if (read_size != filesize) {
        perror("Failed to read the entire file");
        free(buffer);
        fclose(file);
        return 1;
    }

    fclose(file);

    // For demonstration, just print first few bytes as hex
    printf("First 128 bytes of the PDF file:\n");
    for (int i = 0; i < 128 && i < filesize; i++) {
        printf("%02X ", (unsigned char)buffer[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }

    free(buffer);
    return 0;
}
