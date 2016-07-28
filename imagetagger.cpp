#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>

const int NUMBER_OF_COLORS = 9; //in hsl red is bot at the begging and at the end
const std::array<std::pair<int, const std::string> , NUMBER_OF_COLORS > COLORS = {
   std::make_pair( 15,  "red"),
   std::make_pair( 45,  "orange"),
   std::make_pair( 70,  "yellow"),
   std::make_pair( 145, "green"),
   std::make_pair( 189, "cyan"),
   std::make_pair( 260, "blue"),
   std::make_pair( 290, "violet"),
   std::make_pair( 344, "pink"),
   std::make_pair( 360, "red")
};

const int SHADES_OF_GRAY = 4;
const std::string GRAY_SCALE[] = {
	"black","dark gray", "light gray", "white"
};

const std::string TINT = "with a tint of";

const int MAX_AMOUNT_OF_IDS = 1000; //
int colorIdCount[MAX_AMOUNT_OF_IDS] = {};

const size_t MAX_BITMAP_SIZE = static_cast<size_t>(200 * 1 << 20);

struct HSL{
	int h; //   0 - 360
	int s; // % 0 - 100
	int l; // % 0 - 100
	
	HSL (int h=0, int s=100, int l=50){
		this->h = h;
		this->s = s;
		this->l = l;
	}
};


int hueToColor(int targetHue){
	const std::pair<int, const std::string > *it = std::find_if(COLORS.begin(), COLORS.end(), [targetHue](std::pair<int, const std::string> color) -> bool{
		auto maxHueValue = color.first;
		auto colorName = color.second;
		if (maxHueValue >= targetHue){
			return true;
		}
		return false;
	});
	
	if (it - COLORS.begin() + 1 >= 9){
		return 1;
	}
	return it-COLORS.begin()+1;
}

HSL rgbToHsl(int r, int g, int b){
	HSL hsl = HSL();
	double R = static_cast<double>(r) / 255.0;
	double G = static_cast<double>(g) / 255.0;
	double B = static_cast<double>(b) / 255.0;
	double Cmax = static_cast<int>(std::max(r,std::max(g,b))) / 255.0;
	double Cmin = static_cast<int>(std::min(r,std::min(g,b))) / 255.0;
	double del= Cmax - Cmin;
	
	
	//L
	hsl.l = (Cmax + Cmin)*50.0;
	
	//S
	if (Cmax == 0.0){
		hsl.s = 0;
	}else{
		if (2*hsl.l > 100)
			hsl.s = static_cast<int>((del / (Cmax + Cmin))*100.0);
		else
			hsl.s = static_cast<int>((del / (2.0 - Cmax - Cmin))*100.0);
	}
	
	//H
	double H;
	if (del == 0.0) {hsl.h = 0;
	}else{
		if (Cmax == R)
			H = (  ( (G-B) /del));
		else if (Cmax == G)
			H = (  ( (B-R) /del)  +2);
		else
			H = (  ( (R-G) /del)  +4);
			
		H *= 60;
		if (H < 0){
			H += 360;
		}
		hsl.h = static_cast<int>(H);
	}
	return hsl;
}

int hslToColorId(HSL hsl){
	 if (hsl.s <= 10){
		if (hsl.l <= 16){
			return 10;
		}else if (hsl.l <= 33){
			return 20;
		}else if (hsl.l <= 66){
			return 30;
		}else{
			return 40;
		}
	 }else{
		 int id = hueToColor(hsl.h)*100;
		 if (hsl.s <= 39){
			 if (hsl.l <= 49){
				 return id + 21;
			 }else{
				 return id + 31;
			 }
		 }else{
			 if (hsl.l <= 33){
				 return id + 12;
			 }else if(hsl.l <= 65){
				 return id + 22;
			 }else{
				 return id + 32;
			 }
		 }
	 }
}

std::string idToColorName(int id){
	if (id % 10 == 0){
		return GRAY_SCALE[((id/10)%10)-1];
	}else{
		if (id% 10 == 1){
			return GRAY_SCALE[((id/10)%10)-1] + " " + TINT + " " + COLORS[(id/100)-1].second;
		}else{
			if ((id/10)%10 == 1){
				return "dark " + COLORS[(id/100)-1].second;
			}else if((id/10)%10 == 3) {
				return "light " + COLORS[(id/100)-1].second;
			}else{
				return COLORS[(id/100)-1].second;
			}
		}
	}
}

int imageToPixels(const cv::Mat *image){
	int mostPlentifulColor=0;
	int colorId;
	for (int y=0; y < image->size().height ; y++){
		for (int x=0; x < image->size().width; x++){
			//OpenCV reads images in BGR format
			colorId = hslToColorId(
					rgbToHsl(
						static_cast<int>(image->at<cv::Vec3b>(cv::Point(x,y))[2]),  //red
						static_cast<int>(image->at<cv::Vec3b>(cv::Point(x,y))[1]),  //green
						static_cast<int>(image->at<cv::Vec3b>(cv::Point(x,y))[0])));//blue
			
			if (colorId >= MAX_AMOUNT_OF_IDS){
				std::cout << "ERROR: ID too high to be counted";
				return -1;
			}
			
			colorIdCount[colorId]++;
			mostPlentifulColor = std::max(mostPlentifulColor, colorIdCount[colorId]);
		}
	}
	
	return mostPlentifulColor;
}

int fileSize(std::string imagePath){
	int x, y;
	
	
	
	//by the usage of
	//http://www.wischik.com/lu/programmer/get-image-size.html
	//FILE *fl=std::fopen(imagePath.data(),"rb"); 
	std::ifstream f (imagePath , std::ios::in | std::ios::binary | std::ios::ate);
	
	if (f==0){
		std::cout << "ERROR: There is no such image!\n";
		return (-1);
	}
	 
	long len= f.tellg(); 
	f.seekg(0, std::ios::beg);
	
	if (len<24) {
		//fclose(fl); 
		f.close();
		std::cout << "ERROR: Image must be eather: JPEG, PNG or GIF!\n";
		return (-1);
	}
	
	// Strategy:
	// reading GIF dimensions requires the first 10 bytes of the file
	// reading PNG dimensions requires the first 24 bytes of the file
	// reading JPEG dimensions requires scanning through jpeg chunks
	// In all formats, the file is at least 24 bytes big, so we'll read that always
	char buf2[24]; 
	unsigned char buf[24];
	f.read(buf2,24);
	for (int i=0 ; i < 24; i++){
		buf[i] = static_cast<unsigned char>(buf2[i]);
	}
	// For JPEGs, we need to read the first 12 bytes of each chunk.
	// We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
	if ((buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF && buf[3]==0xE0 && buf[6]=='J' && buf[7]=='F' && buf[8]=='I' && buf[9]=='F') ||
        (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF && buf[3]==0xE1 && buf[6]=='E' && buf[7]=='x' && buf[8]=='i' && buf[9]=='f')){
		long pos=2;
		while (buf[2]==0xFF){
			if (buf[3]==0xC0 || buf[3]==0xC1 || buf[3]==0xC2 || buf[3]==0xC3 || buf[3]==0xC9 || buf[3]==0xCA || buf[3]==0xCB) 
				break;
			pos += 2+(buf[4]<<8)+buf[5];
			if (pos+12>len) break;
			f.seekg(pos, std::ios::beg);
			f.read(buf2+2, 12);
			for (int i=0 ; i < 24; i++){
				buf[i] = static_cast<unsigned char>(buf2[i]);
			}
		}
	}
	f.close();
	
	bool correctImageType = false;
	
	// JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
	if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF){
		y = (buf[7]<<8) + buf[8];
		x = (buf[9]<<8) + buf[10];
		correctImageType = true;
	}
	
	
	// GIF: first three bytes say "GIF", next three give version number. Then dimensions
	if (buf[0]=='G' && buf[1]=='I' && buf[2]=='F'){
		x = buf[6] + (buf[7]<<8);
		y = buf[8] + (buf[9]<<8);
		correctImageType = true;
	}
	// PNG: the first frame is by definition an IHDR frame, which gives dimensions
	if ( buf[0]==0x89 && buf[1]=='P' && buf[2]=='N' && buf[3]=='G' && buf[4]==0x0D && buf[5]==0x0A && buf[6]==0x1A && buf[7]==0x0A && buf[12]=='I' && buf[13]=='H' && buf[14]=='D' && buf[15]=='R'){
		x = (buf[16]<<24) + (buf[17]<<16) + (buf[18]<<8) + (buf[19]<<0);
		y = (buf[20]<<24) + (buf[21]<<16) + (buf[22]<<8) + (buf[23]<<0);
		correctImageType = true;
	}
	
	if (correctImageType == false){
		std::cout << "ERROR: Image must be eather: JPEG, PNG or GIF!\n";
		return -1;
	}
	
	if (static_cast<long long>( 54.0D + 4.0D*(65536.0D) + 4.0D*ceil(static_cast<double>(x) * 16.0D / 32.0D)*static_cast<double>(y)) > MAX_BITMAP_SIZE){
		std::cout << "ERROR: Image is too big!\n";
		return (-1);
	}
	
	return 1;
}

int main (int argc, char** argv){
	
	//Fetching input image
	if (argc < 2){ // No argument with image directory
		std::cout << "ERROR: No image was given!\n";
		return -1;
	}
	
	if (fileSize(argv[1]) == -1){
		return -1;
	}
	
	cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);
	
	if (image.empty()){
		std::cout << "ERROR: There is no such image!\n";
		return -1;
	}	
	
	//Calculations
	int mostPlentifulColor = imageToPixels(&image);
	
	
	// Output
	bool first = 1;
	int outputted=0;
	
	for (int& i : colorIdCount){
		
		if (i > (mostPlentifulColor*3)/4){
			if (outputted >= 5){
				break;
			}
			if (first){
				first = 0;
			}else{
				std::cout << ", ";
			}
			std::cout << idToColorName(&i-colorIdCount);
			outputted++;
		}
	}std::cout << "\n";
	
	return 0;
}
