#include <chrono>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <algorithm>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::min;
using namespace std::chrono;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

int rows;
int cols;

unsigned char** reds;
unsigned char** greens;
unsigned char** blues;

void RGB_Allocate(unsigned char**& color) {
    color = new unsigned char*[rows];
    for (int i = 0; i < rows; i++)
        color[i] = new unsigned char[cols];
}

void RGB_Allocates()
{
  RGB_Allocate(reds);
  RGB_Allocate(greens);
  RGB_Allocate(blues);
}

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
  std::ifstream file(fileName);

  if (file)
  {
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else
  {
    cout << "File" << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer)
{
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          reds[i][j] = fileReadBuffer[end - count];
          break;
        case 1:
          greens[i][j] = fileReadBuffer[end - count];
          break;
        case 2:
          blues[i][j] = fileReadBuffer[end - count];
          break;
        }
        count++;
      }
  }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize)
{
  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          fileBuffer[bufferSize - count] = reds[i][j];
          break;
        case 1:
          fileBuffer[bufferSize - count] = greens[i][j];
          break;
        case 2:
          fileBuffer[bufferSize - count] = blues[i][j];
          break;
        }
        count++;
      }
  }
  write.write(fileBuffer, bufferSize);
}

int smoothe_block(unsigned char **channel, int posx, int posy)
{ 
  int sum=0;
  for(int i=posx-1; i<=posx+1; i++){
    for(int j=posy-1; j<=posy+1; j++){
      sum += channel[i][j];
    }
  }
  return sum/9;
}

void smoothing_filter()
{
  for(int i=1; i< rows-1; i++){
    for(int j=1; j< cols-1; j++){
      reds[i][j] = smoothe_block(reds, i, j);
      greens[i][j] = smoothe_block(greens, i, j);
      blues[i][j] = smoothe_block(blues, i, j);
    }
  }
}

void sepia_filter()
{
  for(int i=0; i< rows; i++){
    for(int j=0; j< cols; j++){
      unsigned char red = min((int)(reds[i][j] * 0.393) + (int)(greens[i][j] * 0.769) + (int)(blues[i][j] * 0.189), 255);
      unsigned char green = min((int)(reds[i][j] * 0.349) + (int)(greens[i][j] * 0.686) + (int)(blues[i][j] * 0.168), 255);
      unsigned char blue = min((int)(reds[i][j] * 0.272) + (int)(greens[i][j] * 0.534) + (int)(blues[i][j] * 0.131), 255);

      reds[i][j] = red;
      greens[i][j] = green;
      blues[i][j] = blue;
    }
  }
}

int mean_channel()
{
  int sum1=0, sum2=0, sum3=0;
  for(int i=0; i< rows; i++){
    for(int j=0; j< cols; j++){
      sum1 += reds[i][j];
      sum2 += greens[i][j];
      sum3 += blues[i][j];
    }
  }
  return (sum1 + sum2 + sum3)/(3 * rows * cols);
}

void overall_filter()
{
  int mean_ch = mean_channel();
  for(int i=0; i< rows; i++){
    for(int j=0; j< cols; j++){
      reds[i][j] = reds[i][j] * 0.4 + mean_ch * 0.6;
      greens[i][j] = greens[i][j] * 0.4 + mean_ch * 0.6;
      blues[i][j] = blues[i][j] * 0.4 + mean_ch * 0.6;
    }
  }
}

void add_cross_sign()
{
  for(int i=1; i< rows-1; i++){
    reds[i][i] = reds[i][i+1] = reds[i][i-1] = 255;
    greens[i][i] = greens[i][i+1] = greens[i][i-1] = 255;
    blues[i][i] = blues[i][i+1] = blues[i][i-1] = 255;

    int j = (rows-1) - i;
    reds[i][j] = reds[i][j-1] = reds[i][j+1] = 255;
    greens[i][j] = greens[i][j-1] = greens[i][j+1] = 255;
    blues[i][j] = blues[i][j-1] = blues[i][j+1] = 255;
  }
}


int main(int argc, char *argv[])
{  
  clock_t t_total;
  t_total = clock();

  char *fileBuffer;
  int bufferSize;
  char *fileName = argv[1];
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
  {
    cout << "File read error" << endl;
    return 1;
  }


  clock_t t;
  t = clock();
  RGB_Allocates();
  t = clock() - t;
  cout << "alocating space time: " << ((double)t)/CLOCKS_PER_SEC *1000 << endl;


  // read input file
  t = clock();
  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer);
  t = clock() - t;
  cout << "get input time: " << ((double)t)/CLOCKS_PER_SEC * 1000 << endl;


  // apply filters
  t = clock();
  smoothing_filter();
  t = clock() - t;
  cout << "smoothing filter time: " << ((double)t)/CLOCKS_PER_SEC * 1000 << endl;

  t = clock();
  sepia_filter();
  t = clock() - t;
  cout << "sepia filter time: " << ((double)t)/CLOCKS_PER_SEC * 1000 << endl;

  t = clock();
  overall_filter();
  t = clock() - t;
  cout << "overall filter time: " << ((double)t)/CLOCKS_PER_SEC * 1000 << endl;

  t = clock();
  add_cross_sign();
  t = clock() - t;
  cout << "add cross sign filter time: " << ((double)t)/CLOCKS_PER_SEC * 1000 << endl;

  // write output file
  t = clock();
  writeOutBmp24(fileBuffer, "output.bmp" ,bufferSize);
  t = clock() - t;
  cout << "write out time: " << ((double)t)/CLOCKS_PER_SEC * 1000 << endl;

  t_total = clock() - t_total;
  cout << "total time: " << ((double)t_total)/CLOCKS_PER_SEC * 1000 << endl;
  
  return 0;
}