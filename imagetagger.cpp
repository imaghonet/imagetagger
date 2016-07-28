#include "opencv2/opencv.hpp"
#include <iostream>
#include <algorithm>
#include <array>

const int MAX_PIXELS_TO_COUNT = 1000000; // If the area of a picture is bigger than MAX_PIXELS_TO?_COUNT it will omit some of pixels to fit
const int MAX_AMOUNT_OF_IDS = 1000; //

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

int colorIdCount[MAX_AMOUNT_OF_IDS] = {};

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
	const std::pair<int, const std::basic_string<char> > *it = std::find_if(COLORS.begin(), COLORS.end(), [targetHue](std::pair<int, const std::string> color) -> bool{
		auto maxHueValue = color.first;
		auto colorName = color.second;
		if (maxHueValue >= targetHue){
			return true;
		}
		return false;
	});
	
	if (it - COLORS.begin() >= 8){
		return 1;
	}
	return it-COLORS.begin();
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

int main (int argc, char** argv){
	
	if (argc < 2){ // No argument with image directory
		std::cout << "ERROR: No image was given!\n";
		return -1;
	}
	
	
	cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);
	
	if (image.empty()){
		std::cout << "ERROR: There is no such image!\n";
		return -1;
	}	
	
	int pixelsToSkip=1;
	int area = image.size().height * image.size().width;
	
	while (area/(pixelsToSkip * pixelsToSkip) > MAX_PIXELS_TO_COUNT)
		pixelsToSkip++;
		
	
	int mostPlentifulColor=0;
	int colorId;
	for (int y=0; y < image.size().height ; y+=pixelsToSkip){
		for (int x=0; x < image.size().width; x+= pixelsToSkip){
			//OpenCV reads images in BGR format
			colorId = hslToColorId(
					rgbToHsl(
						static_cast<int>(image.at<cv::Vec3b>(cv::Point(x,y))[2]),  //red
						static_cast<int>(image.at<cv::Vec3b>(cv::Point(x,y))[1]),  //green
						static_cast<int>(image.at<cv::Vec3b>(cv::Point(x,y))[0])));//blue
			
			if (colorId >= MAX_AMOUNT_OF_IDS){
				std::cout << "ERROR: ID too high to be counted";
				return -1;
			}
			
			colorIdCount[colorId]++;
			mostPlentifulColor = std::max(mostPlentifulColor, colorIdCount[colorId]);
		}
	}
	
	
	bool first = 1;
	int outputted=0;
	
	for (int& i : colorIdCount){
		
		if (i > (mostPlentifulColor*3)/4){
			if (outputted >= 5){
				return 0;
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
