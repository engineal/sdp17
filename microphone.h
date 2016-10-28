#ifndef MICROPHONE_H
#define MICROPHONE_H

class Microphone {
public:
	char* buffA;
	char* buffB;
	char* buffC;
	char delay;
	
	Microphone();
	~Microphone();
private:
};

#endif /*MICROPHONE_H*/