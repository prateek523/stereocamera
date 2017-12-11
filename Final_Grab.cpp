#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "videoInput.h"
#include "leopardUtil.h"

#define XRES 640
#define YRES 480
#define SIZE 921600
#define BPP 16

//26mm 8x6

int main(int argc, char *argv[])
{
	videoInput VI;
	int device1 = 1;
	int count = 0;
	VI.setupDevice(device1, XRES, YRES, 3);
	
	int height = YRES;
	int width = XRES*2;

	unsigned char * pixBuff = new unsigned char[SIZE];
	unsigned char * pixBuffOut = new unsigned char[SIZE];

	while (true) {

		// Capture image buffer from camera
		int idx = VI.isFrameNew(device1);

		if (idx == 1) {
			VI.getPixels(device1, pixBuff, false, false);               //fills pixels as a BGR (for openCV) unsigned char array - no flipping
																	  // Leopard Imaging conversion

			raw_to_bmp_mono(pixBuff, pixBuffOut, XRES, YRES, BPP, false, 1);

			cv::Mat frame = cv::Mat(YRES, XRES, CV_8UC3, pixBuffOut);
			cv::Size dsize(width, height);
			cv::Mat scale;
			cv::Mat image;
			cv::resize(frame, scale, dsize, 0, 0, 1);
			cv::Mat r_frame, l_frame;
			r_frame = scale(cv::Rect(width / 2, 0, width / 2, height));
			l_frame = scale(cv::Rect(0, 0, width / 2, height));
			cv::Mat lr_frame;
			cv::hconcat(l_frame, r_frame, lr_frame);
			//cv::imshow("r", r_frame);
			//cv::imshow("l", l_frame);
			cv::imshow("final image", lr_frame);

			int c = cv::waitKey(1);
			if ((char)c == 'q') {
				printf("Exitting program...");
				break;
			}

			else if ((char)c == 'c') {
				printf("Capturing\n");
				stringstream l_name, r_name;
				l_name << "imageleft" << count << ".jpg";
				r_name << "imageright" << count << ".jpg";
				imwrite(l_name.str(),l_frame);
				imwrite(r_name.str(),r_frame);
				count++;
			}
			else if ((char)c == 'p') {
				printf("Pausing");
				while (true) {
					c = cv::waitKey(1);
					if ((char)c == 'p') {
						break;
					}
				}
			}
		}
	}

	return 0;
}