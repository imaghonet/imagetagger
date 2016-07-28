# imagetagger
Extract main color names from image

---

### imagetagger requires installed:
* OpenCV    [OpenCV web page](http://opencv.org/)
* g++       

---

### Usage:  

    imagetagger "IMAGE DIRECTORY"  
program will output most dominant colors in IMAGE in csv format

---

### Installation:  
After entering imagetagger directory use  

    make  
to compile imagetagger.cpp to imagetagger the standalone program  

    sudo make install
to compile imagetagger.cpp and put it to /usr/local/bin/imagetagger

---

### Code Explanation
1. imagetagger loads image  
2. scans it pixel after pixel and if the size of the image is too big it skips some to fit in reasonable time limit
3. in function __imageToPixels(cv::Mat*)__ it

> 1. changes the deafult BGR format of each pixel to HSL format
2. imagetagger gives (with some approximation) each color an Id (by __hslToColorId(hsl)__)
3. keeps count of every Id (in __colorIdCount__) 

4. most dominant colorIds are converted to color names (by __idToColorName(int)__) and outputed

### Default format of colorId and color names assigned to them

> h - hue value  
  s - saturation value  
  l - luminance value  

[] - no prefix

1.. - red			     346 h  15  
2.. - orange			  16 h  45  
3.. - yellow			  46 h  70  
4.. - green		  	  71 h 145  
5.. - cyan		  	 146 h 190  
6.. - blue		  	 190 h 260  
7.. - violet			 261 h 290  
8.. - pink	  	 	 291 h 345  

0.0 - 0   s 10  shades of gray  
>	010 - black		     0 l  16  
	020 - dark gray		17 l  33  
	030 - light gray	50 l  66  
	040 - white			  83 l 100  

..1 - 	with tint of 10 s  39  
>	.21 - dark gray with tint of	0  l  49  
	.31 - light gray with tint of	50 l 101  

 ..2 - [normal] 	40  s 100  
>	.12 - dark    		 0 l  33  
	.22 - [normal]   	34 l  65  
	.32 - light		    66 l 101  
