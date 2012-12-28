#ifndef FILEREADER_H
#define FILEREADER_H

#define MAXLINESPERBUFFER 50
#define LINESTONEXTBUFFER 5

#include <fstream>
#include <vector>

class FileReader
{
public:
	FileReader() { m_isOpen = false; }
	~FileReader();

	bool initialize(wchar_t* filename);
	wchar_t* readline();
	void loadBuffer();

private:
	std::wifstream m_file;
	wchar_t* m_filename;
	bool m_isOpen;

	std::vector<wchar_t*>* m_buffer;
	std::vector<wchar_t*> m_buffer1;
	std::vector<wchar_t*> m_buffer2;
	unsigned int* m_bufferlines;
	unsigned int m_bufferlines1;
	unsigned int m_bufferlines2;
};

#endif
