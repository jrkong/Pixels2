# Pixels2

This program is developed to allow cross platform conversion of media files into [ASCII art](https://en.wikipedia.org/wiki/ASCII_art).
Examples of media resources that could serve as input:

1. Images (incoming).
2. Live video stream.
3. Pre-recorded video files.

For supported formats please refer to [images](http://amin-ahmadi.com/2016/09/24/list-of-image-formats-supported-by-opencv/) and [videos](http://www.fourcc.org/codecs.php).

## Dependencies
> 1. [OpenCV](https://opencv.org/)
> 2. pkg-config (Linux only)
> 3. Make (Linux only)

## Usage (Linux)
Once OpenCV is installed, just run `make` inside the project folder. The executable will be created inside `output/` folder.
> Note: make assumes that there is OpenCV v4 installed. If this is not the case, you can update Makefile accordingly.

## Usage (Windows)
Once OpenCV is installed, it is recommended to use Visual Studio because the config for it is already present. To run the program, you need to set `OPENCV_DIR` env variable. Refer to [link](https://docs.opencv.org/2.4/doc/tutorials/introduction/windows_install/windows_install.html#set-the-opencv-enviroment-variable-and-add-it-to-the-systems-path) for more information.

## That's it!
