cv2_yuv2rgb:
	g++ -g cv2_yuv2rgb.cpp -O3 -Wall -lopencv_core -lopencv_imgproc -lopencv_highgui -std=c++11 -o cv2_yuv2rgb

all: cv2_yuv2rgb
	echo "Compile cv2_yuv2rgb over."

clean:
	rm -rf cv2_yuv2rgb
