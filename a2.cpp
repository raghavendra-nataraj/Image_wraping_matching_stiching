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

bool sortPoint(point p1,point p2){
  return p1.score<p2.score;
}

void match2Images(CImg<double> &input_image,CImg<double> &input_image2){
  CImg<double> final_image;
  CImg<double> gImg1 = input_image.get_RGBtoHSI().get_channel(2);
  CImg<double> gImg2 = input_image2.get_RGBtoHSI().get_channel(2);
  final_image.append(input_image);
  final_image.append(input_image2);
  vector<SiftDescriptor> img1 = Sift::compute_sift(gImg1);
  vector<SiftDescriptor> img2 = Sift::compute_sift(gImg2);
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
      if(p1.score/p2.score<0.5){
	const unsigned char color[] = { 255,128,64 };
	final_image.draw_line(img1[i].col,img1[i].row,input_image.width()+p1.x,p1.y,color);
      }
    }
  final_image.save("result.png");
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
    string inputFile2 = argv[3];
    if(part == "part1")
      {
	// This is just a bit of sample code to get you started, to
	// show how to use the SIFT library.

	CImg<double> input_image(inputFile.c_str());
	CImg<double> input_image2(inputFile2.c_str());
	// convert image to grayscale
	match2Images(input_image,input_image2);
	/*vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);
		cout<<"Size="<<descriptors.size()<<endl;
		for(int i=0; i<descriptors.size(); i++)
	  {
	    cout << "Descriptor #" << i << ": x=" << descriptors[i].col << " y=" << descriptors[i].row << " descriptor=(";
	    for(int l=0; l<128; l++)
	      cout << descriptors[i].descriptor[l] << "," ;
	    cout << ")" << endl;

	    for(int j=0; j<5; j++)
	      for(int k=0; k<5; k++)
		if(j==2 || k==2)
		  for(int p=0; p<3; p++)
		    input_image((int)descriptors[i].col+k-1, (int)descriptors[i].row+j-1, 0, p)=0;

	  }

	  input_image.get_normalize(0,255).save("sift.png");*/
		
      }
    else if(part == "part2")
      {
	// do something here!
      }
    else if(part == "part3")
      {
	// do something here!
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








