#pragma once

namespace esphome {
namespace tm1650 {

const uint8_t TM1650_UNKNOWN_CHAR = 0b11111111;
const uint8_t TM1650_DOT_SEGMENT  = 0b10000000;

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D   P
// PCEDAFGB
const uint8_t TM1650_ASCII_TO_RAW[] PROGMEM = {
    0b00000000,           // ' ', ord 0x20
    0b11000001,           // '!', ord 0x21
    0b00000101,           // '"', ord 0x22
    TM1650_UNKNOWN_CHAR,  // '#', ord 0x23
    TM1650_UNKNOWN_CHAR,  // '$', ord 0x24
    0b00011010,           // '%', ord 0x25
    TM1650_UNKNOWN_CHAR,  // '&', ord 0x26
    0b00000100,           // ''', ord 0x27
    0b00111100,           // '(', ord 0x28
    0b01011001,           // ')', ord 0x29
    0b00001000,           // '*', ord 0x2A
    TM1650_UNKNOWN_CHAR,  // '+', ord 0x2B
    0b01000000,           // ',', ord 0x2C
    0b00000010,           // '-', ord 0x2D
    0b10000000,           // '.', ord 0x2E
    TM1650_UNKNOWN_CHAR,  // '/', ord 0x2F
    0b01111101,           // '0', ord 0x30
    0b01000001,           // '1', ord 0x31
    0b00111011,           // '2', ord 0x32
    0b01011011,           // '3', ord 0x33
    0b01000111,           // '4', ord 0x34
    0b01011110,           // '5', ord 0x35
    0b01111110,           // '6', ord 0x36
    0b01001001,           // '7', ord 0x37
    0b01111111,           // '8', ord 0x38
    0b01011111,           // '9', ord 0x39
    0b00011000,           // ':', ord 0x3A
    0b01011000,           // ';', ord 0x3B
    TM1650_UNKNOWN_CHAR,  // '<', ord 0x3C
    TM1650_UNKNOWN_CHAR,  // '=', ord 0x3D
    TM1650_UNKNOWN_CHAR,  // '>', ord 0x3E
    0b00101011,           // '?', ord 0x3F
    0b00111111,           // '@', ord 0x40
    0b01101111,           // 'A', ord 0x41
    0b01110110,           // 'B', ord 0x42
    0b00111100,           // 'C', ord 0x43
    0b01110011,           // 'D', ord 0x44
    0b00111110,           // 'E', ord 0x45
    0b00101110,           // 'F', ord 0x46
    0b01111100,           // 'G', ord 0x47
    0b01100111,           // 'H', ord 0x48
    0b01000001,           // 'I', ord 0x49
    0b01110001,           // 'J', ord 0x4A
    TM1650_UNKNOWN_CHAR,  // 'K', ord 0x4B
    0b00110100,           // 'L', ord 0x4C
    TM1650_UNKNOWN_CHAR,  // 'M', ord 0x4D
    0b01100010,           // 'N', ord 0x4E
    0b01111101,           // 'O', ord 0x4F
    0b00101111,           // 'P', ord 0x50
    0b11111101,           // 'Q', ord 0x51
    0b00100010,           // 'R', ord 0x52
    0b01011110,           // 'S', ord 0x53
    0b00100110,           // 'T', ord 0x54
    0b01110101,           // 'U', ord 0x55
    0b01110101,           // 'V', ord 0x56
    0b01110111,           // 'W', ord 0x57
    TM1650_UNKNOWN_CHAR,  // 'X', ord 0x58
    0b00100111,           // 'Y', ord 0x59
    0b00111011,           // 'Z', ord 0x5A
    0b00111100,           // '[', ord 0x5B
    TM1650_UNKNOWN_CHAR,  // '\', ord 0x5C
    0b01011001,           // ']', ord 0x5D
    TM1650_UNKNOWN_CHAR,  // '^', ord 0x5E
    0b00010000,           // '_', ord 0x5F
    0b00000001,           // '`', ord 0x60
    0b01101111,           // 'a', ord 0x61
    0b01110110,           // 'b', ord 0x62
    0b00110010,           // 'c', ord 0x63
    0b01110011,           // 'd', ord 0x64
    0b00111110,           // 'e', ord 0x65
    0b00101110,           // 'f', ord 0x66
    0b01111100,           // 'g', ord 0x67
    0b01100110,           // 'h', ord 0x68
    0b01000000,           // 'i', ord 0x69
    0b01110001,           // 'j', ord 0x6A
    TM1650_UNKNOWN_CHAR,  // 'k', ord 0x6B
    0b00110100,           // 'l', ord 0x6C
    TM1650_UNKNOWN_CHAR,  // 'm', ord 0x6D
    0b01100010,           // 'n', ord 0x6E
    0b01110010,           // 'o', ord 0x6F
    0b00101111,           // 'p', ord 0x70
    TM1650_UNKNOWN_CHAR,  // 'q', ord 0x71
    0b00100010,           // 'r', ord 0x72
    0b01011110,           // 's', ord 0x73
    0b00100110,           // 't', ord 0x74
    0b01110000,           // 'u', ord 0x75
    0b01110000,           // 'v', ord 0x76
    TM1650_UNKNOWN_CHAR,  // 'w', ord 0x77
    TM1650_UNKNOWN_CHAR,  // 'x', ord 0x78
    0b00100111,           // 'y', ord 0x79
    TM1650_UNKNOWN_CHAR,  // 'z', ord 0x7A
    0b01000011,           // '{', ord 0x7B
    0b00100100,           // '|', ord 0x7C
    0b00100110,           // '}', ord 0x7D
    0b00001111,           // '~', ord 0x7E (degree symbol)
};

}  // namespace tm1650
}  // namespace esphome
