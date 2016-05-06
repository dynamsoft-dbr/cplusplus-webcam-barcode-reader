# Real-time Webcam Barcode Detection with OpenCV and C/C++

The samples demonstrate how to use [Dynamsoft Barcode Reader SDK](http://www.dynamsoft.com/Products/Dynamic-Barcode-Reader.aspx) to build a barcode reader application on Windows and Raspberry Pi with a USB webcam.

![webcam barcode scanner for Windows and Raspberry Pi](http://www.codepool.biz/wp-content/uploads/2016/05/dbr_opencv_cplusplus.png)

## Getting Started
### Windows 10
1. Install [OpenCV for Windows](http://opencv.org/downloads.html).
2. Open the project with Visual Studio 2015.
3. Add the **include** and **lib** directories of Dynamsoft Barcode Reader and OpenCV to the project properties.
4. Build the project and copy the relevant **\*.dll** libraries to the output directory

### Raspberry Pi Raspbian Jessie
1. Contact [support@dynamsoft.com](mailto:support@dynamsoft.com) to get the download link.
2. Extract the package and generate a symbolic link:

    ```
    sudo ln -s $(DynamsoftBarcodeReader)/lib/libDynamsoftBarcodeReader.so /usr/lib/libDynamsoftBarcodeReader.so
    ```
3. Visit [license page](http://www.dynamsoft.com/CustomerPortal/Account/GetTrialLicense.aspx?Product=DBR) to get a trial license.
4. Follow the [article](http://www.pyimagesearch.com/2015/10/26/how-to-install-opencv-3-on-raspbian-jessie/) to install OpenCV.
5. Build the source code:

   ```
   g++ -ggdb -I$(DynamsoftBarcodeReader)/include -o barcodereader barcodereader.cpp -lDynamsoftBarcodeReader `pkg-config --libs opencv`
   ```
