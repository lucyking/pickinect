#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#define SIZE 385024
#include <math.h>


int i,j;

using namespace cv;
using namespace std;

struct sysinfo sys_info;
char uptimeInfo[15];
unsigned long uptime;

void createAlphaMat(Mat &mat ,unsigned int* val);
unsigned int get_raw(unsigned int raw[],unsigned char tmp[]);   // calculate the raw Y10B value to RGB(black&&white)


int main( int argc, char** argv )
{

	unsigned char data[SIZE];                               //date store the Y10B raw data
	unsigned int raw[640*480],min_depth=2047,min_x,min_y;   //raw store the pixel data

	FILE *camera;//*fo;                               //fo is used to store the raw Y10B data,if you need the raw Y10B data,uncomment the codes about fo
	camera=fopen("/dev/video0", "rb");
	Mat mat(480, 640, CV_8UC3);                       //  cv::Mat to store RGB picture


	while(1){                                         //infinite loop to get depth frames from kinect
		//sleep(3);

		sysinfo(&sys_info);                       //use uptime strings as picture name
		unsigned long uptime = sys_info.uptime;
		sprintf(uptimeInfo, "%ld", uptime);

		//	fo=fopen(uptimeInfo,"wb");           //fo is used to record the raw depth data from kinect

		fread(data, sizeof(data[0]), SIZE, camera);  // read raw Y10B data from kinect

		//   fwrite(data, sizeof(data[0]), SIZE, fo);
		//   fclose(fo);

		get_raw(raw,data);                         // trans Y10B to RGB
		for(i=0;i<640;i++){
			for(j=640*480-640*5+i;j>0;j=j-640){
				if(j>=640*480-640*5)
					continue;
				if(abs(raw[j]-raw[j-640])>50) {
					raw[j] = 0;
					break;
				}
			}
		}


		min_depth = 2047;                            // get the closest point
		min_x = 0, min_y = 0;

		for(i=0+640*5+5;i<640*480-640;i++){          // draw the threshold line
			if(raw[i]> 100 && raw[i]<min_depth){
				min_depth=raw[i];
				min_x=i/640;
				min_y=i%640;
			}
		}


		int point = min_x*640+min_y;
		for(i=0+640*5+5;i<640*480-640;i++){
			// draw x
			if(abs(i-point)<25){
				raw[i]=0;
			}

			// draw y
			if( (point-i)%640==0 && abs((point-i)/640)<25){
				raw[i]=0;
			}
		}


		printf("%s>[%d,%d]:%d\n",uptimeInfo,min_x,min_y,min_depth);

		createAlphaMat(mat,raw);                    // write the RGB picture to the filesystem
//		putText(mat, "kkk", Point(min_y, min_x), 1, 0.5, Scalar::all(255), 1, 7, true);
		unsigned int dis = 12.36 * tan(min_depth / 2842.5 + 1.1863);
		sprintf(uptimeInfo,"<%d,%d>::%d (cm)",min_y,min_x,dis);

		/* stastic tag*/
		putText(mat, uptimeInfo, Point(5, 480-10), 1, 2, Scalar::all(50), 1, 2, false);

		/* domatic tag
		if(min_y<320)
			putText(mat, uptimeInfo, Point(min_y-50, min_x-5), 1, 2, Scalar::all(50), 1, 2, false);
		else
			putText(mat, uptimeInfo, Point(min_y-450, min_x-5), 1, 2, Scalar::all(50), 1, 2, false);
		*/

		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(10);
		sprintf(uptimeInfo, "%ld.png", uptime);
		imwrite(uptimeInfo, mat, compression_params);
	}


	return 0;
}





void createAlphaMat(Mat &mat ,unsigned int* val)           //assign the RGB value to corresponding pixel of picture
{   int k=0+68+5;
	unsigned int pixel;
	for (int i = 0; i < mat.rows; i++) {
		for (int j = 0; j < mat.cols; j++) {
			pixel=val[k]/4;                    // 2^10 >> 255  , so value/4
			Vec3b& rgba = mat.at<Vec3b>(i, j);
			rgba[0] = pixel;
			rgba[1] = pixel;
			rgba[2] = pixel;
//			rgba[3] = 255;
			k++;
		}
	}

}





unsigned int get_raw(unsigned int raw[],unsigned char tmp[]) {    // trans Y10B to RGB value

	int k,index;
	index=0;
	k=0;
	for (k=0x240+1;k<384000;k=k+5){
		raw[index+0]=tmp[k+0]<<2;
		raw[index+0]=raw[index+0]|(tmp[k+1]>>6);
		raw[index+1]=(tmp[k+1]&0b00111111)<<4;
		raw[index+1]=raw[index+1]|(tmp[k+2]>>4);
		raw[index+2]=(tmp[k+2]&0b00001111)<<6;
		raw[index+2]=raw[index+2]|(tmp[k+3]>>2);
		raw[index+3]=(tmp[k+3]&0b00000011)<<8;
		raw[index+3]=raw[index+3]|tmp[k+4];
		index=index+4;
	}
	return 0;


}

