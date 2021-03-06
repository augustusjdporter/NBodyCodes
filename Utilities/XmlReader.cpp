#ifdef _WIN32
#define _USE_MATH_DEFINES
#include <cmath>
#endif

#include "XmlReader.h"


using namespace std;
using namespace rapidxml;

XmlReader::XmlReader()
{
}

XmlReader::~XmlReader()
{
}

const string XmlReader::simulationType() const
{
	return m_simulation_type;
}

int XmlReader::parseConfig(char* configFile, int* timestep, int* numberOfSteps, int* samplingRate, double* normalisation, Universe* simulation_universe)
{
	int result;

	FILE *file;
	fopen_s(&file, configFile, "r");
	if (file == NULL) 
	{
        result = -1;
    }
    else
    {
	    fclose(file);

		m_timestep = timestep;
		m_numberOfSteps = numberOfSteps;
		m_samplingRate = samplingRate;
		m_simulation_universe = simulation_universe;

		ifstream theFile(configFile);

		cout << "Parsing " << string(configFile) << "..." << endl;
		vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
		buffer.push_back('\0');
		// Parse the buffer using the xml file parsing library into doc 
		m_doc.parse<0>(&buffer[0]);
		// Find our root node
		m_root_node = m_doc.first_node("SimulationProfile");

		m_simulation_type = m_root_node->first_node("simulationType")->value();

		if (m_simulation_type == "planetary")
		{
			result = parsePlanetaryConfig();
			*normalisation = AU;
		}
		else if (m_simulation_type == "galaxy")
		{
			result = parseGalaxyConfig();
			*normalisation = kPc;
		}
		else
		{
			result = -2;
		}
	}

	m_timestep = NULL;
	m_numberOfSteps = NULL;
	m_samplingRate = NULL;
	m_simulation_universe = NULL;
	m_root_node = NULL;

	return result;
}

int XmlReader::parseGalaxyConfig()
{
	*m_timestep = atoi(m_root_node->first_node("timestep")->value())*secondsInYear;
	*m_numberOfSteps = atoi(m_root_node->first_node("numberOfSteps")->value());
	*m_samplingRate = atoi(m_root_node->first_node("samplingRate")->value());

	for (xml_node<> * galaxy_node = m_root_node->first_node("Galaxy"); galaxy_node; galaxy_node = galaxy_node->next_sibling("Star"))
	{
		int numberOfStars = atoi(galaxy_node->first_attribute("numberOfStars")->value());
		double massOfStars = atof(galaxy_node->first_attribute("massOfStars")->value())*solar_mass;
		int numberOfGas = atoi(galaxy_node->first_attribute("numberOfGas")->value());
		double massOfGas = atof(galaxy_node->first_attribute("massOfGas")->value())*solar_mass;
		double massOfBH = atof(galaxy_node->first_attribute("massBlackHole")->value())*solar_mass;
		double xCenter = atof(galaxy_node->first_attribute("xCenter")->value())*kPc;
		double yCenter = atof(galaxy_node->first_attribute("yCenter")->value())*kPc;
		double zCenter = atof(galaxy_node->first_attribute("zCenter")->value())*kPc;
		double xScale = atof(galaxy_node->first_attribute("xScale")->value())*kPc;
		double yScale = atof(galaxy_node->first_attribute("yScale")->value())*kPc;
		double zScale = atof(galaxy_node->first_attribute("zScale")->value())*kPc;
		double velocity = atof(galaxy_node->first_attribute("velocity")->value());
		double dispersion = atof(galaxy_node->first_attribute("dispersion")->value());

		m_simulation_universe->addSystem(new Galaxy(numberOfStars,
								  massOfStars, //stars
					 			  numberOfGas, 
					 			  numberOfGas, //gas
					 			  massOfBH, //mass of BH
					 			  xCenter, 
					 			  yCenter, 
					 			  zCenter, //x,y,z
					 			  xScale, 
					 			  yScale, 
					 			  zScale, //x,y,z
					 			  velocity, 
					 			  dispersion));// velocity and dispersion
	}

	for (xml_node<> * darkMatter_node = m_root_node->first_node("DarkMatterHalo"); darkMatter_node; darkMatter_node = darkMatter_node->next_sibling("Star"))
	{
		int numberOfDM = atoi(darkMatter_node->first_attribute("numberOfDM")->value());
		double massOfDM = atof(darkMatter_node->first_attribute("massOfDM")->value())*solar_mass;
		double xScale = atof(darkMatter_node->first_attribute("xScale")->value())*kPc;
		double yScale = atof(darkMatter_node->first_attribute("yScale")->value())*kPc;
		double zScale = atof(darkMatter_node->first_attribute("zScale")->value())*kPc;
		m_simulation_universe->addSystem(new DarkMatterHalo(numberOfDM, massOfDM, xScale, yScale, zScale));
	}

	return 0;
}

int XmlReader::parsePlanetaryConfig()
{
	*m_timestep = atoi(m_root_node->first_node("timestep")->value());
	*m_numberOfSteps = atoi(m_root_node->first_node("numberOfSteps")->value());
	*m_samplingRate = atoi(m_root_node->first_node("samplingRate")->value());

	// Iterate over the stars
	for (xml_node<> * star_node = m_root_node->first_node("Star"); star_node; star_node = star_node->next_sibling("Star"))
	{
		string starName = star_node->first_attribute("name")->value();
		double starMass = atof(star_node->first_attribute("mass")->value())*solar_mass;
		double starXPos = atof(star_node->first_attribute("x")->value())*AU;
		double starYPos = atof(star_node->first_attribute("y")->value())*AU;
		double starZPos = atof(star_node->first_attribute("z")->value())*AU;
		double starXVel = atof(star_node->first_attribute("xVel")->value());
		double starYVel = atof(star_node->first_attribute("yVel")->value());
		double starZVel = atof(star_node->first_attribute("zVel")->value());
		double starRadius = atof(star_node->first_attribute("radius")->value())*solar_radius;
		bool logStarTrajectory = bool(atof(star_node->first_attribute("logTrajectory")->value()));

		System* solarSystem = new System(starName +" System");
		solarSystem->addBody(shared_ptr<Body>(new Body(starName,
									  starMass, 
									  starXPos, 
									  starYPos, 
									  starZPos, 
									  starXVel, 
									  starYVel, 
									  starZVel, 
									  starRadius, 
									  logStarTrajectory)));

	    for(xml_node<> * planet_node = star_node->first_node("Planet"); planet_node; planet_node = planet_node->next_sibling("Planet"))
	    {
	    	string planetName = planet_node->first_attribute("name")->value();
	    	double orbitalPeriod = atof(planet_node->first_attribute("orbitalPeriod")->value())*secondsInYear;
	    	double orbitalRadius = atof(planet_node->first_attribute("orbitalRadius")->value())*AU;
	    	double inclination = atof(planet_node->first_attribute("inclination")->value());
	    	double planetMass = atof(planet_node->first_attribute("mass")->value())*earth_mass;
			double planetXPos = atof(planet_node->first_attribute("orbitalRadius")->value())*AU + starXPos;
			double planetYPos = starYPos;
			double planetZPos = starZPos;
			double planetXVel = 0;
			double planetYVel = 1/(orbitalPeriod)*(2*M_PI)*orbitalRadius*cos(M_PI*inclination/180);
			double planetZVel = 1/(orbitalPeriod)*(2*M_PI)*orbitalRadius*sin(M_PI*inclination/180);
			double planetRadius = atof(planet_node->first_attribute("radius")->value())*earth_radius;
			bool logPlanetTrajectory = bool(atof(planet_node->first_attribute("logTrajectory")->value()));
	    	//v=wr
	    	//requires further development if the star is moving
	    	solarSystem->addBody(shared_ptr<Body>(new Body(planetName,
									 	  planetMass, 
									 	  planetXPos, 
									 	  planetYPos, //place it at same x and y as star. This can be looked into
									 	  planetZPos, 
									 	  0, 
									 	  planetYVel, 
									 	  planetZVel, 
									 	  planetRadius, 
									 	  logPlanetTrajectory)));
	    }

	    //systems->push_back(*solarSystem);
	    m_simulation_universe->addSystem(solarSystem);
	    solarSystem = NULL;
	    for(xml_node<> * cloud_node = star_node->first_node("ProtoplanetaryCloud"); cloud_node; cloud_node = cloud_node->next_sibling("ProtoplanetaryCloud"))
	    {
	    	int numberOfPlanetesimals = atoi(cloud_node->first_attribute("numberOfPlanetesimals")->value());
	    	double cloudMass = atof(cloud_node->first_attribute("mass")->value())*solar_Mass;
	    	double cloudXCenter = starXPos;
	    	double cloudYCenter = starYPos;
	    	double cloudZCenter = starZPos;
	    	double cloudXScale = atof(cloud_node->first_attribute("xScale")->value())*AU;
	    	double cloudYScale = atof(cloud_node->first_attribute("yScale")->value())*AU;
	    	double cloudZScale = atof(cloud_node->first_attribute("zScale")->value())*AU;
	    	double cloudVelocity = 0;
	    	double cloudDispersion = 0;

	    	m_simulation_universe->addSystem(new ProtoplanetaryCloud(starName + "cloud",
	    										   numberOfPlanetesimals, 
										    	   cloudMass, 
										    	   cloudXCenter, //place it at same x and y as star. This can be looked into
										    	   cloudYCenter, //place it at same x and y as star. This can be looked into
									 	 		   cloudZCenter,  
										    	   cloudXScale, 
										    	   cloudYScale, 
										    	   cloudZScale, 
										    	   cloudVelocity, 
										    	   cloudDispersion));
	    }    
	}

	return 0;
}