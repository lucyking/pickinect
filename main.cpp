#include <opencv2/opencv.hpp>

#include <vector>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#define SIZE 385024


int i,j;

using namespace cv;
using namespace std;

struct sysinfo sys_info;
char uptimeInfo[15];
unsigned long uptime;

void createAlphaMat(Mat &mat ,unsigned int* val);
unsigned int get_raw(unsigned int raw[],unsigned char tmp[]);


int main( int argc, char** argv )
{

	unsigned char data[SIZE];
    unsigned int raw[640*480],min_depth=2047,min_x,min_y;

	FILE *camera,*fo;
	camera=fopen("/dev/video0", "rb");


	//Mat mat(480, 640, CV_8UC4);
while(1){

	sysinfo(&sys_info);
	unsigned long uptime = sys_info.uptime;
	sprintf(uptimeInfo, "%ld", uptime);

//	fo=fopen(uptimeInfo,"wb");


    fread(data, sizeof(data[0]), SIZE, camera);
 //   fwrite(data, sizeof(data[0]), SIZE, fo);

 //   fclose(fo);
    get_raw(raw,data);


    min_depth=2047;
    for(i=0+640*4+5;i<640*480-640;i++){
    	if(raw[i]<min_depth){
    		min_depth=raw[i];
    	    min_x=i/640;
    	    min_y=i%640;
    	}
    }

    printf("%s>[%d,%d]:%d\n",uptimeInfo,min_x,min_y,min_depth);


/*	createAlphaMat(mat,raw);

	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(10);
	sprintf(uptimeInfo, "%ld.png", uptime);
	imwrite(uptimeInfo, mat, compression_params);

*/


}


	return 0;
}





void createAlphaMat(Mat &mat ,unsigned int* val)
{   int k=0+68+5;
	unsigned int pixel;
	for (int i = 0; i < mat.rows; i++) {
		for (int j = 0; j < mat.cols; j++) {
			pixel=val[k]/4;
			Vec4b& rgba = mat.at<Vec4b>(i, j);
			rgba[0] = pixel;
			rgba[1] = pixel;
			rgba[2] = pixel;
			rgba[3] = 255;
			k++;
		}
	}

}





unsigned int get_raw(unsigned int raw[],unsigned char tmp[]) {



	int k,index;


	index=0;
	k=0;

	for (k=0x240+1;k<384000;k=k+5){


		raw[index+0]=tmp[k+0]<<2;
		raw[index+0]=raw[index+0]|(tmp[k+1]>>6);
/*
if(raw[index+0]<min && raw[index+0]>0)
			min=raw[index+0];
*/
		raw[index+1]=(tmp[k+1]&0b00111111)<<4;
		raw[index+1]=raw[index+1]|(tmp[k+2]>>4);
//		if(raw[index+1]<min && raw[index+1]>0)
//					min=raw[index+1];

		raw[index+2]=(tmp[k+2]&0b00001111)<<6;
		raw[index+2]=raw[index+2]|(tmp[k+3]>>2);
//		if(raw[index+2]<min && raw[index+2]>0)
//					min=raw[index+2];

		raw[index+3]=(tmp[k+3]&0b00000011)<<8;
		raw[index+3]=raw[index+3]|tmp[k+4];
//		if(raw[index+3]<min && raw[index+3]>0)
//					min=raw[index+3];

		index=index+4;

	}




	return 0;
}


