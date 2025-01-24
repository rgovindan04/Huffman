#include "bitreader.h"

#include <stdio.h>
#include <stdlib.h>

struct BitReader {
    FILE *underlying_stream;
    uint8_t byte;
    uint8_t bit_position;
};

BitReader *bit_read_open(const char *filename) {
    // Allocate memory for BitReader
    BitReader *bitReader = (BitReader *) malloc(sizeof(BitReader));
    if (bitReader == NULL) {
        return NULL; // Allocation error
    }

    // Open the file for reading in binary mode
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        free(bitReader);
        return NULL; // File opening error
    }

    // Store the file stream in BitReader
    bitReader->underlying_stream = file;

    // Clear the byte field of BitReader to 0
    bitReader->byte = 0;

    // Set the bit_position field of BitReader to 8
    bitReader->bit_position = 8;

    return bitReader;
}

void bit_read_close(BitReader **pbuf) {
    if (*pbuf != NULL) {
        BitReader *bitReader = *pbuf;

        // Close the underlying_stream
        if (fclose(bitReader->underlying_stream) != 0) {
            exit(1);
        }
        // Free the BitReader
        free(bitReader);

        // Set *pbuf to NULL
        *pbuf = NULL;
    }
}

uint8_t bit_read_bit(BitReader *buf) {
    if (buf->bit_position > 7) {
        // Read a byte from the underlying_stream using fgetc()
        int readByte = fgetc(buf->underlying_stream);
        if (readByte == EOF) {
            return 1;
        } else {
            buf->byte = (uint8_t) readByte;
            buf->bit_position = 0;
        }
    }
    // Get the bit numbered bit_position from byte
    uint8_t bit = (buf->byte >> buf->bit_position) & 1;
    // Increment bit_position
    buf->bit_position += 1;
    return bit;
}

uint8_t bit_read_uint8(BitReader *buf) {
    uint8_t byte = 0x00;

    for (int i = 0; i < 8; i++) {
        uint8_t bit = bit_read_bit(buf); // Read a bit using bit_read_bit()
        byte |= (bit << i); // Set bit i of byte to the value of bit
    }

    return byte;
}

uint16_t bit_read_uint16(BitReader *buf) {
    uint16_t byte = 0x0000;

    for (int i = 0; i < 16; i++) {
        uint16_t bit = bit_read_bit(buf); // Read a bit using bit_read_bit()
        byte |= (bit << i); // Set bit i of byte to the value of bit
    }

    return byte;
}

uint32_t bit_read_uint32(BitReader *buf) {
    uint32_t byte = 0x00000000;

    for (int i = 0; i < 32; i++) {
        uint32_t bit = bit_read_bit(buf); // Read a bit using bit_read_bit()
        byte |= (bit << i); // Set bit i of byte to the value of bit
    }

    return byte;
}
