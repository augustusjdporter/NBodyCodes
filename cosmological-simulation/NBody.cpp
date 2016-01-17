//g++ -std=c++11 -O3 -o NBody.out -I. NBody.cpp body.cpp
//STATIC UNIFORM

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <map>
#include <utility>
#include <fstream>

#include "body.h"

using namespace std;

int main()
{
	cout << "Enter number of bodies: ";
	int bodies_number;
	cin >> bodies_number;
	cout << endl;
	cout << "Enter number of iterations to take: ";
	int iteration_number;
	cin >> iteration_number;

	const time_t ctt = time(0);
	cout << asctime(localtime(&ctt)) << endl;//output time 
	time_t beginninguni, enduni;

	const double kPc(3.0857*pow(10, 19));
	const double Solar_Mass(1.989*pow(10, 30));
	const double year(31536000);
	const double timestep(pow(10,5)*year);//year in seconds

	std::default_random_engine generator;

  	std::uniform_real_distribution<double> distribution(0.,3.0*kPc);
  	//std::uniform_real_distribution<double> distributionz(0.,1.0*kPc);
  	//std::normal_distribution<double> distribution_velocity(230000,1*pow(10,5));

	vector <Body> Body_Vector;
	//Body_Vector.push_back(Planet("Sun", 1.9891*pow(10, 32), 0, 0, 0, 0, 0, 0));
	for (int i =0; i<bodies_number; i++)
	{
		double numberx = distribution(generator);
		double numbery = distribution(generator);
		double numberz = distribution(generator);

		//Direction of velocity (perpindicular to the displacement)
		//double velocity_Direction_x = pow(pow(numberx,2) + pow(numbery,2),-0.5)*(-numbery);
		//double velocity_Direction_y = pow(pow(numberx,2) + pow(numbery,2),-0.5)*(numberx);

		//Magnitude of velocity (constant) ms^-1
		//double velocity_magnitude = distribution_velocity(generator);
		//double numberxvel = velocity_Direction_x * velocity_magnitude;
		//double numberyvel = velocity_Direction_y * velocity_magnitude;

		stringstream combiner;
		combiner << "Star" << i;
		string name;
		combiner >> name;
		Body_Vector.push_back(Body(name, pow(10, 12)/bodies_number*Solar_Mass, numberx, numbery, numberz, 0, 0, 0));
		//cout << numberx << " " << numbery << " " << numberz << " " << numberxvel << " " << numberyvel << endl;
	}

	ofstream file;
	string filename;
	filename = "StartCoords.txt";//(shapename).txt
	file.open (filename);

	for (int i=0; i<Body_Vector.size(); i++)
	{
		file << Body_Vector.at(i).name << "\t" << Body_Vector.at(i).xPosition << "\t" << Body_Vector.at(i).yPosition << "\t" << Body_Vector.at(i).zPosition << endl;//prints shape data with overloaded <<
	}
	file.close();
	//Body_Vector.push_back(Planet("Sun", 1.9891*pow(10, 30), 0, 0, 0, 0, 0, 0));


for (int j = 0; j < iteration_number; j++)
{
	beginninguni = time(0);
	vector <Body>::iterator it;
	for (it = Body_Vector.begin(); it != Body_Vector.end(); ++it)
  	{
  		//cout << "Enter loop" << endl;
	vector<double> acceleration = it->accelerationCalc(Body_Vector);

	it->xPosition = it->xPosition + it->xVelocity*timestep;
	it->yPosition = it->yPosition + it->yVelocity*timestep;
	it->zPosition = it->zPosition + it->zVelocity*timestep;
	
	it->xVelocity = it->xVelocity + acceleration.at(0)*timestep;
	it->yVelocity = it->yVelocity + acceleration.at(1)*timestep;
	it->zVelocity = it->zVelocity + acceleration.at(2)*timestep;
	//cout << acceleration.at(0)*timestep << endl;
	//cout << acceleration.at(1)*timestep << endl;
	//cout << acceleration.at(2)*timestep << endl;
	}
	cout << j << endl;
	enduni = time(0);
	cout << "Time taken for this universe (seconds): " << enduni - beginninguni << endl;
}

	ofstream file2;

	file2.open ("EndCoords.txt");

	for (int i=0; i<Body_Vector.size(); i++)
	{
		file2 << Body_Vector.at(i).name << "\t" << Body_Vector.at(i).xPosition << "\t" << Body_Vector.at(i).yPosition << "\t" << Body_Vector.at(i).zPosition << endl;//prints shape data with overloaded <<
	}
	file2.close();

	return 0;
}