#include "CityData.h"
#pragma warning(disable:4996)
CityData::CityData()
{
	//initialization three information
	LocationId = 0;
	name = new char[50]();
	country = new char[50]();
}


CityData::~CityData()
{
}

int CityData::GetLocationId()
{
	// return locationId
	return LocationId;
}

char * CityData::Getname()
{
	// return name
	return name;
}

char * CityData::Getcountry()
{
	// retunr country
	return country;
}

void CityData::SetLocationId(int LocationId)
{
	// set locationid
	this->LocationId = LocationId;
}

void CityData::Setname(char * name)
{
	// set name
	strcpy(this->name, name);
}

void CityData::Setcountry(char * country)
{
	// set country
	strcpy(this->country, country);
}
