// B657 assignment 2 skeleton code
//
// Compile with: "make"
//
// See assignment handout for command line and project specifications.


//Link to the header file
#include "CImg.h"
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>

//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;


struct point{
  double x;
  double y;
  double score;
};

struct lines {
  double x1;
  double x2;
  double y1;
  double y2;
};

struct matStr {
  string fileName;
  int matches;
};

bool sortPoint(point p1,point p2){
  return p1.score<p2.score;
}


bool sortImages(matStr i1,matStr i2){
  return i1.matches>i2.matches;
}

int match2Images(CImg<double> &input_image,CImg<double> &input_image2){
  CImg<double> final_image;
  CImg<double> gImg1 = input_image.get_RGBtoHSI().get_channel(2);
  CImg<double> gImg2 = input_image2.get_RGBtoHSI().get_channel(2);
  final_image.append(input_image);
  final_image.append(input_image2);
  vector<SiftDescriptor> img1 = Sift::compute_sift(gImg1);
  vector<SiftDescriptor> img2 = Sift::compute_sift(gImg2);
  int matches=0;
  for(int i=0; i<img1.size(); i++)
    {
      vector<point> scores;
      for(int j = 0; j<img2.size();j++){
	double score = 0;
	for(int k=0;k<128;k++){
	  score+= ((img1[i].descriptor[k] - img2[j].descriptor[k]) * (img1[i].descriptor[k] - img2[j].descriptor[k]));
	}
	score = sqrt(score);
	point p1 = {img2[j].col,img2[j].row,score};
	scores.push_back(p1);
      }
      sort(scores.begin(),scores.end(),sortPoint);
      point p1 = scores[0];
      point p2 = scores[1];
      if(p1.score/p2.score<0.7){
	const unsigned char color[] = { 255,128,64 };
	final_image.draw_line(img1[i].col,img1[i].row,input_image.width()+p1.x,p1.y,color);
	matches+=1;
      }
    }
  final_image.save("result.png");
  return matches;
}

int main(int argc, char **argv)
{
  try {

    if(argc < 2)
      {
	cout << "Insufficent number of arguments; correct usage:" << endl;
	cout << "    a2 part_id ..." << endl;
	return -1;
      }

    string part = argv[1];
    string inputFile = argv[2];
    if(part == "part1")
      {
	// This is just a bit of sample code to get you started, to
	// show how to use the SIFT library.
	vector<matStr> matC;
	CImg<double> input_image(inputFile.c_str());
	for (int i=3;i<argc;i++){
	  CImg<double> input_image2(argv[i]);
	  // convert image to grayscale
	  int count = match2Images(input_image,input_image2);
	  struct matStr temp= {string(argv[i]),count};
	  matC.push_back(temp);
	}
	sort(matC.begin(),matC.end(),sortImages);
	vector<matStr>::iterator it;
	cout<<matC.size()<<endl;
	for(it = matC.begin();it != matC.end();it++){
	  cout<<it->fileName<<endl;
	}
	cout<<endl;
		
      }
    else if(part == "part2")
      {
	// do something here!
      }
    else if(part == "part3")
      {
	// do something here!
	CImg<double> input_image(inputFile.c_str());
	CImg<double> output_image(input_image.width(),input_image.height(),input_image.depth(),input_image.spectrum());
	for(int i=0;i<input_image.width();i++){
	  for(int j=0;j<input_image.height();j++){
	    //int x = (0.907*i) + (0.258*j) - 182;
	    //int y = (-0.153*i)+ (1.44*j) + 58 ;
	    //int w = (-0.000306*i)+ (0.000731*j)+1;
	    int x = (1.1247*i) + (-0.3147*j) + 222.9409;
	    int y = (0.1088*i)+ (0.6851*j) - 19.9247 ;
	    int w = (0.0003*i)+ (-0.0006*j)+1.0828;
	    if(w!=0){
	      x=x/w;
	      y=y/w;
	    }
	    if(x>=0 && y>=0 && x<input_image.width() && y<input_image.height()){
	      output_image(i,j,0) = input_image.atXY(x,y,0);
	      output_image(i,j,1) = input_image.atXY(x,y,1);
	      output_image(i,j,2) = input_image.atXY(x,y,2);
	      }
	  }
	}
	output_image.save("result.png");
	
      }
    else
      throw std::string("unknown part!");

    // feel free to add more conditions for other parts (e.g. more specific)
    //  parts, for debugging, etc.
  }
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}








