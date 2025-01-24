#include "bitwriter.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct BitWriter {
    FILE *underlying_stream;
    uint8_t byte;
    uint8_t bit_position;
};

BitWriter *bit_write_open(const char *filename) {
    // Allocate memory for BitWriter
    BitWriter *bitWriter = (BitWriter *) malloc(sizeof(BitWriter));
    if (bitWriter == NULL) {
        return NULL; // Allocation error
    }

    // Open the file for writing in binary mode
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        free(bitWriter);
        return NULL; // File opening error
    }

    // Store the file stream in BitWriter
    bitWriter->underlying_stream = f;

    // Reset fields to 0
    bitWriter->byte = 0;
    bitWriter->bit_position = 0;

    return bitWriter;
}

void bit_write_close(BitWriter **pbuf) {
    if (*pbuf != NULL) {
        BitWriter *bitWriter = *pbuf;
        if ((*pbuf)->bit_position > 0) {
            // Write the byte to the underlying_stream using fputc()
            if (fputc(bitWriter->byte, bitWriter->underlying_stream) == EOF) {
                exit(1);
            }
        }
        // Close the underlying_stream
        if (fclose(bitWriter->underlying_stream) == EOF) {
            exit(1);
        }
        // Free the BitWriter
        free(bitWriter);

        // Set *pbuf to NULL
        *pbuf = NULL;
    }
}

void bit_write_bit(BitWriter *buf, uint8_t bit) {
    if (buf->bit_position > 7) {
        // Write the byte to the underlying_stream using fputc()
        if (fputc(buf->byte, buf->underlying_stream) == EOF) {
            exit(1);
        }
        // Clear the byte and bit_position fields of the BitWriter to 0
        buf->byte = 0;
        buf->bit_position = 0;
    }

    // Set the bit at bit_position of the byte to the value of bit
    buf->byte |= (bit << buf->bit_position);
    buf->bit_position += 1;
}

void bit_write_uint8(BitWriter *buf, uint8_t x) {
    for (int i = 0; i < 8; i++) {
        uint8_t bit = (x >> i) & 1; // Extract the i-th bit of x
        bit_write_bit(buf, bit); // Write the bit using bit_write_bit()
    }
}

void bit_write_uint16(BitWriter *buf, uint16_t x) {
    for (int i = 0; i < 16; i++) {
        uint8_t bit = (x >> i) & 1;
        bit_write_bit(buf, bit);
    }
}

void bit_write_uint32(BitWriter *buf, uint32_t x) {
    for (int i = 0; i < 32; i++) {
        uint8_t bit = (x >> i) & 1;
        bit_write_bit(buf, bit);
    }
}
