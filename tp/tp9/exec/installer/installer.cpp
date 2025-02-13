#include "installer.h"


uint16_t installProgram()
{
        // Initialize USART registers to receive info
    setUsartRegisters();
        // Get program size
    uint16_t programSize = readProgramSize();
        // Write to memory as you receive info
    writeToMemoryLoop(programSize);
    return programSize;
}

void writeToMemoryLoop(uint16_t programSize)
{
        // The 2 size bytes include themselves,
        // We must subtract them before reading
           const uint8_t sizeBytes     = 2;
    static const uint8_t transferDelay = 3;
           Memoire24CXXX mem;
    
    for (uint16_t i = 0; i < programSize - sizeBytes; i++)
    {
            // Read and write the instruction 
            // / operand at address i
        mem.ecriture(i, receiveChar());
        _delay_ms(transferDelay);
    }
}

uint16_t readProgramSize()
{
    uint8_t  numberOfBits = 8;
    uint16_t progSize     = 0;

        // Concatenation of first 2 reads
    progSize  = receiveChar() << numberOfBits;
    progSize += receiveChar();

    return progSize;
}
