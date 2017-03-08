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
#include <map>
#include <iomanip>
#include <sstream>
//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;

#define THRESHOLD 0.8

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


struct bestSift {
  int sift1;
  int sift2;
  int matches;
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
  CImg<double> output_image(input_image.width(),input_image.height(),input_image.depth(),input_image.spectrum(),0);
  CImg<double> inv = homog.invert(true);
  for(int i=0;i<input_image.width();i++){
    for(int j=0;j<input_image.height();j++){
      double x = (inv(0,0)*i) + (inv(1,0)*j) + inv(2,0);
      double y = (inv(0,1)*i)+ (inv(1,1)*j) + inv(2,1) ;
      double w = (inv(0,2)*i)+ (inv(1,2)*j)+inv(2,2);
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
  //output_image.save("result1.png");
  return output_image;
}

CImg<double> matMult(CImg<double> mat1,CImg<double> mat2){
  CImg<double> result(mat2.width(),mat1.height(),1,1);
  for(int i=0; i<mat1.height(); i++){
    for(int j=0; j<mat2.width(); j++){
      double sum=0;
      for(int k=0; k<mat1.width(); k++){
	sum = sum + mat1(k,i) * mat2(k,j);
      }
      result(i,j) = sum;
    }
  }
  //cout<<result(0,0)<<" "<<result(1,0)<<" "<<result(2,0)<<" "<<result(3,0)<<" "<<result(4,0)<<" "<<result(5,0)<<" "<<result(6,0)<<" "<<result(7,0)<<endl;
  return result;
}

vector<line> match2Images(CImg<double> &input_image,CImg<double> &input_image2,int w,int k,int iterNum ){
  CImg<double> final_image;
  CImg<double> gImg1 = input_image.get_RGBtoHSI().get_channel(2);
  CImg<double> gImg2 = input_image2.get_RGBtoHSI().get_channel(2);
  final_image.append(input_image);
  final_image.append(input_image2);
  vector<SiftDescriptor> img1 = Sift::compute_sift(gImg1);
  vector<SiftDescriptor> img2 = Sift::compute_sift(gImg2);
  std::map<string, bestSift> siftMatch;
  for(int randIter = 0;randIter<iterNum;randIter++){
    vector<int> rndLst;
    for(;rndLst.size()<k;){
      int randnum = rand()%128;
      if(std::find(rndLst.begin(),rndLst.end(),randnum)==rndLst.end())
	rndLst.push_back(randnum);
    }
    for(int i=0;i<img1.size();i++){
      for(int j=0;j<img2.size();j++){
	bool mat = true;
	for(int k=0;k<rndLst.size();k++){
	  int index = rndLst[k];
	  //cout<<(int)floor(img1[i].descriptor[index])/w<<" "<<(int)floor(img2[j].descriptor[index])/w;
	  if((int)floor(img1[i].descriptor[index])/w != (int)floor(img2[j].descriptor[index])/w){
	    mat = false;
	    break;
	    
	  }
	}
	if(mat){
	  string key;
	  std::ostringstream ostr;
	  ostr <<i<<"_"<<j;
	  string name = ostr.str();
	  map<string, bestSift>::iterator it = siftMatch.find(name);
	  if(it!=siftMatch.end()){
	    struct bestSift bSift = siftMatch[name]; bSift.matches+=1; siftMatch[name] = bSift;
	  }else{
	    struct bestSift bSift = {i,j,1}; siftMatch[name] = bSift;
	  }
	}
      }
    }
  }
  int thresholdMatch=0;
  map<string, bestSift>::iterator it;
  vector<SiftDescriptor> imgDesc1;
  vector<int> addDes1;
  vector<SiftDescriptor> imgDesc2;
  vector<int> addDes2;
  for(it=siftMatch.begin();it!=siftMatch.end();it++){
    struct bestSift bs = it->second;
    if(bs.matches>=thresholdMatch){
      if(find(addDes1.begin(),addDes1.end(),bs.sift1)==addDes1.end()){
	addDes1.push_back(bs.sift1);
	imgDesc1.push_back(img1[bs.sift1]);
      }
      if(find(addDes2.begin(),addDes2.end(),bs.sift2)==addDes2.end()){
	addDes2.push_back(bs.sift2);
	imgDesc2.push_back(img2[bs.sift2]);
      }
    }
  }
  cout<<imgDesc1.size()<<" "<<imgDesc2.size()<<endl;
  vector<line> lines;
  for(int i=0; i<imgDesc1.size(); i++)
    {
      vector<point> scores;
      for(int j = 0; j<imgDesc2.size();j++){
	double score = 0.0;
	for(int k=0;k<128;k++){
	  score+= ((imgDesc1[i].descriptor[k] - imgDesc2[j].descriptor[k]) * (imgDesc1[i].descriptor[k] - imgDesc2[j].descriptor[k]));
	}
	score = sqrt(score);
	point p = {imgDesc2[j].col,imgDesc2[j].row,score};
	scores.push_back(p);
      }
      sort(scores.begin(),scores.end(),sortPoint);
      point p1 = scores[0];
      point p2 = scores[1];
      if(p2.score!=0 && p1.score/p2.score<THRESHOLD){
	const unsigned char color[] = { 255,128,64 };
	final_image.draw_line(imgDesc1[i].col,imgDesc1[i].row,input_image.width()+p1.x,p1.y,color);
	struct line temp = {imgDesc1[i].col,p1.x,imgDesc1[i].row,p1.y};
	lines.push_back(temp);
      }
    }
  final_image.save("result.png");
  return lines;
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
	//cout<<img1[i].col<<" "<<img1[i].row<<" "<<img2[j].col<<" "<<img2[j].row<<endl;
	point p = {img2[j].col,img2[j].row,score};
	scores.push_back(p);
      }
      sort(scores.begin(),scores.end(),sortPoint);
      point p1 = scores[0];
      point p2 = scores[1];
      if(p1.score/p2.score<THRESHOLD){
	const unsigned char color[] = { 255,128,64 };
	final_image.draw_line(img1[i].col,img1[i].row,input_image.width()+p1.x,p1.y,color);
	struct line temp = {img1[i].col,p1.x,img1[i].row,p1.y};
	lines.push_back(temp);
      }
    }
  //final_image.save("result.png");
  return lines;
}





CImg<double> calculateHomography(CImg<double> input_image,CImg<double> input_image2){
  vector<Error> errorlist;
  vector<line> count = match2Images(input_image,input_image2);
  //cout<<count.size()<<endl;
  if(count.size()<4)
    throw std::overflow_error("Zero matching point\nIncrease Threshold\n");
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
    mat(0,0) = x1;mat(1,0) = y1;mat(2,0) = 1;mat(3,0) = 0; mat(4.0) = 0; mat(5.0) = 0;mat(6.0) = (-x1*x_1);mat(7,0) = (x_1*(-y1));
    mat(0,1) = 0; mat(1,1) = 0; mat(2,1) = 0;mat(3,1) = x1;mat(4,1) = y1;mat(5,1) = 1;mat(6,1) = (-x1*y_1);mat(7,1) = (y_1*(-y1));
    mat(0,2) = x2;mat(1,2) = y2;mat(2,2) = 1;mat(3,2) = 0; mat(4,2) = 0; mat(5,2) = 0;mat(6,2) = (-x2*x_2);mat(7,2) = (x_2*(-y2));
    mat(0,3) = 0; mat(1,3) = 0; mat(2,3) = 0;mat(3,3) = x2;mat(4,3) = y2;mat(5,3) = 1;mat(6,3) = (-x2*y_2);mat(7,3) = (y_2*(-y2));
    mat(0,4) = x3;mat(1,4) = y3;mat(2,4) = 1;mat(3,4) = 0; mat(4,4) = 0; mat(5,4) = 0;mat(6,4) = (-x3*x_3);mat(7,4) = (x_3*(-y3));
    mat(0,5) = 0; mat(1,5) = 0; mat(2,5) = 0;mat(3,5) = x3;mat(4,5) = y3;mat(5,5) = 1;mat(6,5) = (-x3*y_3);mat(7,5) = (y_3*(-y3));
    mat(0,6) = x4;mat(1,6) = y4;mat(2,6) = 1;mat(3,6) = 0; mat(4,6) = 0; mat(5,6) = 0;mat(6,6) = (-x4*x_4);mat(7,6) = (x_4*(-y4));
    mat(0,7) = 0; mat(1,7) = 0; mat(2,7) = 0;mat(3,7) = x4;mat(4,7) = y4;mat(5,7) = 1;mat(6,7) = (-x4*y_4);mat(7,7) = (y_4*(-y4));
    CImg<double> inv = mat.invert(true);
    CImg<double> points(1,8,1,1,x_1,y_1,x_2,y_2,x_3,y_3,x_4,y_4);
    //inv =matMult(inv,points);
    inv*=points;
    CImg<double> homog(3,3,1,1,inv(0,0),inv(1,0),inv(2,0),inv(3,0),inv(4,0),inv(5,0),inv(6,0),inv(7,0),1.0);
    //cout<<inv(0,0)<<" "<<inv(0,1)<<" "<<inv(0,2)<<" "<<inv(0,3)<<" "<<inv(0,4)<<" "<<inv(0,5)<<" "<<inv(0,6)<<" "<<inv(0,7)<<endl;
    CImg<double> homeInv = homog;
    homeInv.invert(true);
    double err=0;
	   for(int index =0;index<count.size();index++){
	     double x = (homeInv(0,0)*count[index].x1) + (homeInv(0,1)*count[index].y1) + homeInv(0,2);
	     double y = (homeInv(1,0)*count[index].x1)+ (homeInv(1,1)*count[index].y1) + homeInv(1,2);
	     double w = (homeInv(2,0)*count[index].x1)+ (homeInv(2,1)*count[index].y1) + homeInv(2,2);
	     if(w!=0){
	       x=x/w;
	       y=y/w;
	       double d = sqrt(((x-count[index].x2)*(x-count[index].x2))+((y-count[index].y2)*(y-count[index].y2)));
	       err+=d;
	     } 
	   }
	   if(err!=0){
	     struct Error e = {homog,err};
	     errorlist.push_back(e);
	   }
  }
  vector<Error>::iterator minError = std::min_element(errorlist.begin(),errorlist.end(),compHomo);
	CImg<double> homog = minError->homograp;
	//homog.print();
	CImg<double> homogres(minError->homograp);
	return  homogres;
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
	/*CImg<double> input_image(inputFile.c_str());
	vector<matStr> matC;
	// do something here!
	for(int in=3;in<argc;in++){
	  string imagename(argv[in]);
	  CImg<double> input_image2(imagename.c_str());
	  CImg<double> final_image;
	  CImg<double> gImg1 = input_image.get_RGBtoHSI().get_channel(2);
	  CImg<double> gImg2 = input_image2.get_RGBtoHSI().get_channel(2);
	  vector<SiftDescriptor> img1 = Sift::compute_sift(gImg1);
	  final_image.append(input_image);
	  final_image.append(input_image2);
	  CImg<double> homog = calculateHomography(input_image,input_image2);
	  vector<line> count = match2Images(input_image,input_image2);
	  const unsigned char color[] = { 255,128,64 };
	  double score = 0.0;
	  for(int i=0; i<count.size(); i++)
	    {
	      CImg<double> inv = homog.invert(true);
	      double x = (inv(0,0)*count[i].x1) + (inv(1,0)*count[i].y1) + inv(2,0);
	      double y = (inv(0,1)*count[i].x1)+ (inv(1,1)*count[i].y1) + inv(2,1) ;
	      double w = (inv(0,2)*count[i].x1)+ (inv(1,2)*count[i].y1)+inv(2,2);
	      if(w!=0){
		x=x/w;
		y=y/w;
		score+= sqrt(((x-count[i].x2) * (x-count[i].x2))+((y-count[i].y2) * (y-count[i].y2)));
		if(x>=0 && y>=0 && x<input_image2.width() && y<input_image2.height()){
		  final_image.draw_line(count[i].x1,count[i].y1,input_image.width()+x,y,color);
		}	
	      }
       	    }
	  final_image.save("test.png");
	  struct matStr temp= {string(argv[in]),(int)score};
	  matC.push_back(temp);
	  homog(1,1,1,3,0);
	}
	sort(matC.begin(),matC.end(),sortImages);
	vector<matStr>::iterator it;
	//cout<<matC.size()<<endl;
	for(it = matC.begin();it != matC.end();it++){
	  cout<<it->fileName<<endl;
	}
	cout<<endl;*/
	int k=50,w =50,randNum=200;
	vector<matStr> matC;
	CImg<double> input_image(inputFile.c_str());
	for (int i=3;i<argc;i++){
	  CImg<double> input_image2(argv[i]);
	  vector<line> count = match2Images(input_image,input_image2,w,k,randNum);
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
    else if(part == "part3")
      {
	// do something here!
	/*CImg<double> input_image(inputFile.c_str());
	CImg<double> homog(3,3,1,1);
	homog(0,0) = 0.907;homog(1,0) = 0.258;homog(2,0) = -182;
	homog(0,1) = -0.153;homog(1,1) = 1.44;homog(2,1) = 58;
	homog(0,2) = -0.000306;homog(1,2) = 0.000731;homog(2,2) = 1;
	CImg<double> output_image = transform(input_image,homog);
	output_image.save("result.png1");*/
	CImg<double> input_image(inputFile.c_str());
	// do something here!
	for(int i=3;i<argc;i++){
	  string imagename(argv[i]);
	  string finalImage = imagename.substr(0,imagename.length()-4);
	  finalImage+="_wrapped.png";
	  cout<<finalImage<<endl;
	  CImg<double> input_image2(imagename.c_str());
	  CImg<double> homog = calculateHomography(input_image2,input_image);
	  CImg<double> output_image = transform(input_image2,homog);
	  output_image.save(finalImage.c_str());
	  homog(1,1,1,3,0);
	  output_image(1,1,1,3,0);
	  const double val = 0.0;
	  }
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








