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

struct line {
  double x1;
  double x2;
  double y1;
  double y2;
};


struct Error {
  CImg<double> homograp;
  double error;
};

bool compHomo(Error &e1,Error &e2){
  return (e1.error<e2.error);
}

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

CImg<double> transform(CImg<double> input_image,CImg<double> homog){
  CImg<double> output_image(input_image.width(),input_image.height(),input_image.depth(),input_image.spectrum());
  CImg<double> inv = homog.invert(true);
  for(int i=0;i<input_image.width();i++){
    for(int j=0;j<input_image.height();j++){
      int x = (inv(0,0)*i) + (inv(0,1)*j) + inv(0,2);
      int y = (inv(1,0)*i)+ (inv(1,1)*j) + inv(1,2) ;
      int w = (inv(2,0)*i)+ (inv(2,1)*j)+inv(2,2);
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
  return output_image;
}

vector<line> match2Images(CImg<double> &input_image,CImg<double> &input_image2){
  CImg<double> final_image;
  CImg<double> gImg1 = input_image.get_RGBtoHSI().get_channel(2);
  CImg<double> gImg2 = input_image2.get_RGBtoHSI().get_channel(2);
  final_image.append(input_image);
  final_image.append(input_image2);
  vector<SiftDescriptor> img1 = Sift::compute_sift(gImg1);
  vector<SiftDescriptor> img2 = Sift::compute_sift(gImg2);
  vector<line> lines;
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
	line temp = {img1[i].col,img1[i].row,p1.x,p1.y};
	lines.push_back(temp);
      }
    }
  final_image.save("result.png");
  return lines;
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
	  vector<line> count = match2Images(input_image,input_image2);
	  struct matStr temp= {string(argv[i]),count.size()};
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
	CImg<double> input_image(inputFile.c_str());
	CImg<double> input_image2(argv[2]);
	vector<Error> errorlist;
	vector<line> count = match2Images(input_image,input_image2);
	for(int i=0;i<200;i++){
	  vector<int> rndLst;
	  for(;rndLst.size()<4;){
	    int randnum = rand()%count.size();
	    if(std::find(rndLst.begin(),rndLst.end(),randnum)==rndLst.end())
	      rndLst.push_back(randnum);
	  }
	  double x1 = count[rndLst[0]].x1;double x_1 = count[rndLst[0]].x2;double y1 = count[rndLst[0]].y1;double y_1 = count[rndLst[0]].y2;
	  double x2 = count[rndLst[1]].x1;double x_2 = count[rndLst[1]].x2;double y2 = count[rndLst[1]].y1;double y_2 = count[rndLst[1]].y2;
	  double x3 = count[rndLst[2]].x1;double x_3 = count[rndLst[2]].x2;double y3 = count[rndLst[2]].y1;double y_3 = count[rndLst[2]].y2;
	  double x4 = count[rndLst[3]].x1;double x_4 = count[rndLst[3]].x2;double y4 = count[rndLst[3]].y1;double y_4 = count[rndLst[3]].y2;
	  CImg<double> mat(8,8,1,1);
	  mat(0,0) = x1;mat(0,1) = y1;mat(0,2) = 1;mat(0,3) = 0; mat(0,4) = 0; mat(0,5) = 0;mat(0,6) = (-x1*x_1);mat(0,7) = (x_1*(-y1));
	  mat(1,0) = 0; mat(1,1) = 0; mat(1,2) = 0;mat(1,3) = x1;mat(1,4) = y1;mat(1,5) = 1;mat(1,6) = (-x1*y_1);mat(1,7) = (y_1*(-y1));
	  mat(2,0) = x2;mat(2,1) = y2;mat(2,2) = 1;mat(2,3) = 0; mat(2,4) = 0; mat(2,5) = 0;mat(2,6) = (-x2*x_2);mat(2,7) = (x_2*(-y2));
	  mat(3,0) = 0; mat(3,1) = 0; mat(3,2) = 0;mat(3,3) = x2;mat(3,4) = y2;mat(3,5) = 1;mat(3,6) = (-x2*y_2);mat(3,7) = (y_2*(-y2));
	  mat(4,0) = x3;mat(4,1) = y3;mat(4,2) = 1;mat(4,3) = 0; mat(4,4) = 0; mat(4,5) = 0;mat(4,6) = (-x3*x_3);mat(4,7) = (x_3*(-y3));
	  mat(5,0) = 0; mat(5,1) = 0; mat(5,2) = 0;mat(5,3) = x3;mat(5,4) = y3;mat(5,5) = 1;mat(5,6) = (-x3*y_3);mat(5,7) = (y_3*(-y3));
	  mat(6,0) = x4;mat(6,1) = y4;mat(6,2) = 1;mat(6,3) = 0; mat(6,4) = 0; mat(6,5) = 0;mat(6,6) = (-x4*x_4);mat(6,7) = (x_4*(-y4));
	  mat(7,0) = 0; mat(7,1) = 0; mat(7,2) = 0;mat(7,3) = x4;mat(7,4) = y4;mat(7,5) = 1;mat(7,6) = (-x4*y_4);mat(7,7) = (y_4*(-y4));	
	  CImg<double> inv = mat.invert(true);
	  CImg<double> points(1,8,1,1,count[rndLst[0]].x2,count[rndLst[0]].y2,count[rndLst[1]].x2,count[rndLst[2]].y2,count[rndLst[2]].x2,count[rndLst[2]].y2,count[rndLst[3]].x2,count[rndLst[3]].y2);
	  mat*=points;
	  cout<<mat(0,0)<<" "<<mat(0,1)<<" "<<mat(0,2)<<" "<<mat(0,3)<<" "<<mat(0,4)<<" "<<mat(0,5)<<" "<<mat(0,6)<<" "<<mat(0,7)<<endl;
	  CImg<double> input_image(inputFile.c_str());
	  CImg<double> homog(3,3,1,1,mat(0,0),mat(0,1),mat(0,2),mat(0,3),mat(0,4),mat(0,5),mat(0,6),mat(0,7),1);
	  CImg<double> homeInv = homog.invert(true);
	  double err=0;
	   for(int index =0;index<count.size();index++){
	    int x = (homeInv(0,0)*count[index].x1) + (homeInv(0,1)*count[index].y1) + homeInv(0,2);
	    int y = (homeInv(1,0)*count[index].x1)+ (homeInv(1,1)*count[index].y1) + homeInv(1,2);
	    int w = (homeInv(2,0)*count[index].x1)+ (homeInv(2,1)*count[index].y1) + homeInv(2,2);
	    if(w!=0){
	      x=x/w;
	      y=y/w;
	      double d = sqrt(((x-count[index].x2)*(x-count[index].x2))+((y-count[index].y2)*(y-count[index].y2)));
	      err+=d;
	    } 
	   }
	  Error e = {mat,err};
	  errorlist.push_back(e);
	}
	vector<Error>::iterator minError = std::min_element(errorlist.begin(),errorlist.end(),compHomo);
	CImg<double> homog(3,3,1,1,minError->homograp(0,0),minError->homograp(0,1),minError->homograp(0,2),minError->homograp(0,3),minError->homograp(0,4),minError->homograp(0,5),minError->homograp(0,6),minError->homograp(0,7),1);
	cout<<minError->homograp(0,0)<<" "<<minError->homograp(0,1)<<" "<<minError->homograp(0,2)<<" "<<minError->homograp(0,3)<<" "<<minError->homograp(0,4)<<" "<<minError->homograp(0,5)<<" "<<minError->homograp(0,6)<<" "<<minError->homograp(0,7)<<endl;
	transform(input_image2,homog);
      }
    else if(part == "part3")
      {
	// do something here!
	CImg<double> input_image(inputFile.c_str());
	CImg<double> homog(3,3,1,1);
	homog(0,0) = 0.907;homog(0,1) = 0.258;homog(0,2) = -182;
	homog(1,0) = -0.153;homog(1,1) = 1.44;homog(1,2) = 58;
	homog(2,0) = -0.000306;homog(2,1) = 0.000731;homog(2,2) = 1;
	transform(input_image,homog);
	
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








