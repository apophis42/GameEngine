#include "filereader.h"

bool FileReader::initialize(wchar_t* filename)
{
	m_filename = filename;
	m_file.clear();
	m_file.open(m_filename);

	m_bufferlines1 = 0;
	m_bufferlines2 = 0;
	m_buffer = &m_buffer1;
	m_bufferlines = &m_bufferlines1;

	m_isOpen = !m_file.fail();
	return m_isOpen;
}

FileReader::~FileReader()
{

}

wchar_t* FileReader::readline()
{
	if(!m_isOpen)
		return L"FILE IS NOT OPEN";
	loadBuffer();
	wchar_t* line = (*m_buffer).at(m_buffer->size() - *m_bufferlines - 1);
	(*m_bufferlines)--;

	//See if the buffer is empty
	if(*m_bufferlines == 0)
	{
		if(m_bufferlines1 == 0 && m_bufferlines2 == 0)
			m_isOpen = false;
		else if(m_bufferlines == &m_bufferlines1)
		{
			m_buffer = &m_buffer2;
			m_bufferlines = &m_bufferlines2;
		}
		else if(m_bufferlines == &m_bufferlines2)
		{
			m_buffer = &m_buffer1;
			m_bufferlines = &m_bufferlines1;
		}
		else
			return L"NEITHER BUFFER WORKING";
	}

	return line;
}

void FileReader::loadBuffer()
{
	if(m_file.fail())
		return;
	if(m_bufferlines2 < LINESTONEXTBUFFER && !m_file.eof())
	{
		wchar_t* line;
		std::streamsize size = m_file.rdbuf()->in_avail();
		for(m_bufferlines1 = 0; m_bufferlines1 < MAXLINESPERBUFFER && !m_file.eof(); m_bufferlines1++)
		{
			m_file.getline(line, size);
			if(m_buffer1.size() > m_bufferlines1)
				m_buffer1.at(m_bufferlines1) = line;
			else
				m_buffer1.push_back(line);
		}
	}
	if(m_bufferlines1 < LINESTONEXTBUFFER && !m_file.eof())
	{
		wchar_t* line;
		std::streamsize size = m_file.rdbuf()->in_avail();
		for(m_bufferlines2 = 0; m_bufferlines2 < MAXLINESPERBUFFER && !m_file.eof(); m_bufferlines2++)
		{
			m_file.getline(line, size);
			if(m_buffer2.size() > m_bufferlines2)
				m_buffer2.at(m_bufferlines2) = line;
			else
				m_buffer2.push_back(line);
		}
	}

	if(m_file.eof())
		m_file.close();
}
