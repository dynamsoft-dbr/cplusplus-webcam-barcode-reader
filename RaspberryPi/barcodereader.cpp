#include "opencv2/opencv.hpp"
#include <stdio.h>
#include "If_DBRP.h"
#include <sys/time.h>

using namespace cv;
using namespace std;

typedef unsigned long       DWORD;
typedef long LONG;
typedef unsigned short      WORD;

typedef struct tagBITMAPINFOHEADER {
      DWORD biSize;
        LONG  biWidth;
          LONG  biHeight;
            WORD  biPlanes;
              WORD  biBitCount;
                DWORD biCompression;
                  DWORD biSizeImage;
                    LONG  biXPelsPerMeter;
                      LONG  biYPelsPerMeter;
                        DWORD biClrUsed;
                          DWORD biClrImportant;
} BITMAPINFOHEADER;

struct barcode_format
{
	const char * pszFormat;
	__int64 llFormat;
};

static struct barcode_format Barcode_Formats[] =
{
	{ "CODE_39", CODE_39 },
	{ "CODE_128", CODE_128 },
	{ "CODE_93", CODE_93 },
	{ "CODABAR", CODABAR },
	{ "ITF", ITF },
	{ "UPC_A", UPC_A },
	{ "UPC_E", UPC_E },
	{ "EAN_13", EAN_13 },
	{ "EAN_8", EAN_8 },
	{ "INDUSTRIAL_25",INDUSTRIAL_25 },
	{ "OneD", OneD },
	{ "QR_CODE", QR_CODE },
	{ "PDF417",PDF417 },
	{ "DATAMATRIX", DATAMATRIX }
};

const char * GetFormatStr(__int64 format)
{
	int iCount = sizeof(Barcode_Formats) / sizeof(Barcode_Formats[0]);

	for (int index = 0; index < iCount; index++)
	{
		if (Barcode_Formats[index].llFormat == format)
			return Barcode_Formats[index].pszFormat;
	}

	return "UNKNOWN";
}

void read(Mat &image, CBarcodeReader &reader)
{
	int width = image.cols, height = image.rows;
        int elemSize = image.elemSize();
	int size = image.total() * elemSize;
        char *imageData = (char *)image.data;
	// combine header info and image data
	char *total = (char *)malloc(size + 40);
	memset(total, 0, size + 40);
	BITMAPINFOHEADER bitmap_info = { 40, width, height, 0, 24, 0, size, 0, 0, 0, 0 };
	memcpy(total, &bitmap_info, 40);

	char *data = total + 40;

        width *= elemSize;
	for (int i = 1; i <= height; i++)
	{
		memcpy(data, imageData + width * (height - i), width);

		data += width;
	}

        struct timeval begin, end;
        long timeCost = 0;
        gettimeofday(&begin, NULL);
	int iRet = reader.DecodeBuffer((unsigned char*)total, size + 40);
        gettimeofday(&end, NULL);

        timeCost = ((end.tv_sec * 1000 * 1000 +  end.tv_usec) - (begin.tv_sec * 1000 * 1000 + begin.tv_usec)) / 1000; 
        printf("Total barcode(s) found: Total time spent: %d ms\n\n", 
                timeCost);
	// Output barcode result
	char * pszTemp = (char*)malloc(4096);
	if (iRet != DBR_OK && iRet != DBRERR_LICENSE_EXPIRED && iRet != DBRERR_QR_LICENSE_INVALID &&
		iRet != DBRERR_1D_LICENSE_INVALID && iRet != DBRERR_PDF417_LICENSE_INVALID && iRet != DBRERR_DATAMATRIX_LICENSE_INVALID)
	{
		sprintf(pszTemp, "Failed to read barcode: %s\r\n", DBR_GetErrorString(iRet));
		printf(pszTemp);
		free(pszTemp);

		free(total);
		return;
	}

	pBarcodeResultArray paryResult = NULL;
	reader.GetBarcodes(&paryResult);

	if (paryResult->iBarcodeCount > 0)
	{
		for (int iIndex = 0; iIndex < paryResult->iBarcodeCount; iIndex++)
		{
			sprintf(pszTemp, "Barcode %d:\r\n", iIndex + 1);
			printf(pszTemp);
			sprintf(pszTemp, "    Page: %d\r\n", paryResult->ppBarcodes[iIndex]->iPageNum);
			printf(pszTemp);
			sprintf(pszTemp, "    Type: %s\r\n", GetFormatStr(paryResult->ppBarcodes[iIndex]->llFormat));
			printf(pszTemp);
			char *pszTemp1 = (char*)malloc(paryResult->ppBarcodes[iIndex]->iBarcodeDataLength + 1);
			memset(pszTemp1, 0, paryResult->ppBarcodes[iIndex]->iBarcodeDataLength + 1);
			memcpy(pszTemp1, paryResult->ppBarcodes[iIndex]->pBarcodeData, paryResult->ppBarcodes[iIndex]->iBarcodeDataLength);
			sprintf(pszTemp, "    Value: %s\r\n", pszTemp1);
			printf(pszTemp);
			free(pszTemp1);
			sprintf(pszTemp, "    Region: {Left: %d, Top: %d, Width: %d, Height: %d}\r\n\r\n",
				paryResult->ppBarcodes[iIndex]->iLeft, paryResult->ppBarcodes[iIndex]->iTop,
				paryResult->ppBarcodes[iIndex]->iWidth, paryResult->ppBarcodes[iIndex]->iHeight);
			printf(pszTemp);
		}
	}

	free(pszTemp);
	reader.FreeBarcodeResults(&paryResult);

	free(total);
}

int main(int, char**)
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	Mat frame;
	// Initialize Dynamsoft Barcode Reader
	CBarcodeReader reader ;
	reader.InitLicense("38B9B94D8B0E2B41660D13B593BE6EF9");
	__int64 llFormat = (OneD | QR_CODE | PDF417 | DATAMATRIX);
	int iMaxCount = 0x7FFFFFFF;
	ReaderOptions ro = { 0 };
	ro.llBarcodeFormat = llFormat;
	ro.iMaxBarcodesNumPerPage = iMaxCount;
	reader.SetReaderOptions(ro);

        int i = 0;
	for (;;)
	{
		cap >> frame; // Get a new frame from camera
		imshow("reader", frame); // Display the new frame

		read(frame, reader);

		if (waitKey(30) >= 0) break;
	}
	
	return 0;
}
