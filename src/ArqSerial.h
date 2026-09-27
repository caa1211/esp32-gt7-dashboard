#ifndef __ARQSERIAL_H__
#define __ARQSERIAL_H__
//#define TESTFAIL

#ifndef StreamRead
#define StreamRead Serial.read
#define StreamFlush Serial.flush
#define StreamWrite Serial.write
#define StreamPrint Serial.print
#define StreamAvailable Serial.available
#endif

#include <Arduino.h>
#include <RingBuf.h>

const uint8_t crc_table_crc8[256] PROGMEM = { 0,213,127,170,254,43,129,84,41,252,86,131,215,2,168,125,82,135,45,248,172,121,211,6,123,174,4,209,133,80,250,47,164,113,219,14,90,143,37,240,141,88,242,39,115,166,12,217,246,35,137,92,8,221,119,162,223,10,160,117,33,244,94,139,157,72,226,55,99,182,28,201,180,97,203,30,74,159,53,224,207,26,176,101,49,228,78,155,230,51,153,76,24,205,103,178,57,236,70,147,199,18,184,109,16,197,111,186,238,59,145,68,107,190,20,193,149,64,234,63,66,151,61,232,188,105,195,22,239,58,144,69,17,196,110,187,198,19,185,108,56,237,71,146,189,104,194,23,67,150,60,233,148,65,235,62,106,191,21,192,75,158,52,225,181,96,202,31,98,183,29,200,156,73,227,54,25,204,102,179,231,50,152,77,48,229,79,154,206,27,177,100,114,167,13,216,140,89,243,38,91,142,36,241,165,112,218,15,32,245,95,138,222,11,161,116,9,220,118,163,247,34,136,93,214,3,169,124,40,253,87,130,255,42,128,85,1,212,126,171,132,81,251,46,122,175,5,208,173,120,210,7,83,134,44,249 };
#define updateCrc(currentCrc, value) pgm_read_byte(&crc_table_crc8[currentCrc ^ value]);

typedef void(*IdleFunction) (bool);

class ARQSerial
{
private:

	byte partialdatabuffer[32];
	int Arq_LastValidPacket = 255;
	RingBuf<uint8_t, 32> DataBuffer;
	IdleFunction idleFunction = 0;

#ifdef TESTFAIL
	int testfailidx = 0;
	int testfailidx2 = 0;
#endif

    uint8_t rxStage = 0, rxId = 0, rxLength = 0, rxOffset = 0, rxCrc = 0;
    uint32_t rxLastByte = 0;

    void ProcessIncomingData() {
        if (rxStage && uint32_t(millis() - rxLastByte) > 100) {
            rxStage = 0;
            SendNAcq(Arq_LastValidPacket, 0x05);
        }
        // Decode one ARQ frame at a time. Never overwrite unconsumed payload.
        if (DataBuffer.size() != 0) return;
        while (StreamAvailable() > 0) {
            const int value = StreamRead();
            if (value < 0) return;
            const uint8_t c = static_cast<uint8_t>(value);
            rxLastByte = millis();
            if (rxStage == 0) { if (c == 1) rxStage = 1; }
            else if (rxStage == 1) { rxStage = c == 1 ? 2 : 0; }
            else if (rxStage == 2) {
                rxId = c; rxCrc = updateCrc(0, c); rxStage = 3;
            } else if (rxStage == 3) {
                if (c == 0 || c > sizeof(partialdatabuffer)) {
                    rxStage = 0; SendNAcq(Arq_LastValidPacket, 0x02); continue;
                }
                rxLength = c; rxOffset = 0;
                rxCrc = updateCrc(rxCrc, c); rxStage = 4;
            } else if (rxStage == 4) {
                partialdatabuffer[rxOffset++] = c;
                rxCrc = updateCrc(rxCrc, c);
                if (rxOffset == rxLength) rxStage = 5;
            } else {
                rxStage = 0;
                if (c != rxCrc) { SendNAcq(Arq_LastValidPacket, 0x04); continue; }
                const int nextId = Arq_LastValidPacket > 127 ? 0 : Arq_LastValidPacket + 1;
                if (rxId == nextId || rxId == 255) {
                    for (unsigned i = 0; i < rxLength; ++i) DataBuffer.push(partialdatabuffer[i]);
                    Arq_LastValidPacket = rxId;
                }
                SendAcq(rxId);
                return;
            }
        }
    }

	void SendAcq(uint8_t packetId)
	{
		StreamWrite(0x03);
		StreamWrite(packetId);
		StreamFlush();
	}

	void SendNAcq(uint8_t lastKnownValidPacket, byte reason)
	{
		StreamWrite(0x04);
		StreamWrite(lastKnownValidPacket);
		StreamWrite(reason);
		StreamFlush();
	}

public:

	void setIdleFunction(IdleFunction function) {
		idleFunction = function;
	}

	void CustomPacketStart(byte packetType, uint8_t length) {
		StreamWrite(0x09);
		StreamWrite(packetType);
		StreamWrite(length);
	}

	void CustomPacketSendByte(byte data) {
		StreamWrite(data);
	}

	void CustomPacketEnd() {
		//Serial.write(0x00);
	}

	int read() {
		unsigned long fsr_startMillis = millis();
		do {
			if (idleFunction != 0) idleFunction(false);

			if (DataBuffer.size() > 0) {
				uint8_t res = 0;
				DataBuffer.pop(res);
				return (int)res;
			}

			ProcessIncomingData();
		} while (millis() - fsr_startMillis < 400 || DataBuffer.size() > 0);

		//DebugPrintLn("Read timeout !");
		return -1;
	}

	int Available() {
		if (idleFunction != 0) idleFunction(false);
		if (DataBuffer.size() == 0) {
			ProcessIncomingData();
		}
		return DataBuffer.size();
	}

	void Write(byte data) {
		StreamWrite(0x08);
		StreamWrite(data);
		StreamFlush();
	}

	void Print(char data)
	{
		Write((byte)data);
	}

	void Print(const char str[]) {
		int len = strlen(str);
		StreamWrite(0x06);
		StreamWrite(len);
		StreamWrite(str);
		StreamWrite(0x20);
		StreamFlush();
	}

	void WriteString(String& data)
	{
		int len = data.length();
		StreamWrite(0x06);
		StreamWrite(len);
		StreamPrint(data);
		StreamWrite(0x20);
		StreamFlush();
	}

	void PrintString(const char str[]) {
		int len = strlen(str);
		StreamWrite(0x06);
		StreamWrite(len);
		StreamWrite(str);
		StreamWrite(0x20);
		StreamFlush();
	}

	void PrintLn(const char str[]) {
		int len = strlen(str);
		StreamWrite(0x06);
		StreamWrite(len + 1);
		StreamWrite(str);
		StreamWrite('\n');
		StreamWrite(0x20);
		StreamFlush();
	}

	void PrintLn(String& data)
	{
		StreamWrite(0x06);
		StreamWrite(data.length() + 1);
		StreamPrint(data);
		StreamPrint('\n');
		StreamWrite(0x20);
		StreamFlush();
	}

	void PrintLn() {
		Write('\n');
	}

	String ReadStringUntil(char terminator1, char terminator2) {
		String ret;
		int c = read();
		while (c >= 0 && c != terminator1 && c != terminator2)
		{
			ret += (char)c;
			c = read();
		}
		return ret;
	}

	void ReadStringUntil(char buffer[], char terminator) {
		int pos = 0;

		int c = read();
		while (c >= 0 && c != terminator)
		{
			buffer[pos] = (char)c;
			c = read();
			pos++;
		}
		buffer[pos] = 0;
		
	}

	String ReadStringUntil(char terminator1) {
		String ret;
		int c = read();
		while (c >= 0 && c != terminator1)
		{
			ret += (char)c;
			c = read();
		}
		return ret;
	}

	void DebugPrintLn(String& data)
	{
		StreamWrite(0x07);
		StreamWrite(data.length() + 1);
		StreamPrint(data);
		StreamPrint('\n');
		StreamWrite(0x20);
		StreamFlush();
	}

	void DebugPrint(char data)
	{
		StreamWrite(0x07);
		StreamWrite(1);
		StreamPrint(data);
		StreamWrite(0x20);
		StreamFlush();
	}

	void DebugPrintLn(const char str[]) {
		StreamWrite(0x07);
		StreamWrite((byte)(strlen(str) + 1));
		StreamPrint(str);
		StreamPrint('\n');
		StreamWrite(0x20);
		StreamFlush();
	}
};

#endif