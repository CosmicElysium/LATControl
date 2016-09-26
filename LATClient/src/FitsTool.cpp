#include <FitsTool.h>
#include <iostream>
#include <ctime>
#include <cstring>

using namespace std;

namespace lbtlat {





int FitsTool::write16(string filename, USHORT * data, int width, int height)
{
	fitsfile *fptr;
	int status = 0;
	long fpixel = 1, naxis = 2, nelements;
	long naxes[2] = {height,width};

	string fullname = __dir + "/" + __prefix + filename + ".fits";

	fits_create_file(&fptr, fullname.c_str() , &status);

	fits_create_img(fptr, SHORT_IMG, naxis, naxes, &status);

	nelements = naxes[0] * naxes[1];

	fits_write_img(fptr, TSHORT, fpixel, nelements, data, &status);

	//add keys to the header.
	for (unsigned int i=0; i<__keys.size(); i++)
		putKey(fptr,status, __keys[i]);

	//add cards to the header.
	for (unsigned int i=0; i<__cards.size(); i++)
		fits_insert_record(fptr, 6, (char *) __cards[i].c_str(), &status);

	fits_close_file(fptr, &status);

	fits_report_error(stderr, status);
	return status;
}


int FitsTool::write8(string filename, unsigned char * data, int width, int height)
{
	fitsfile *fptr;
	int status = 0;
	long fpixel = 1, naxis = 2, nelements;
	long naxes[2] = {height,width};

	string fullname = __dir + "/" + __prefix + filename + ".fits";

	fits_create_file(&fptr, fullname.c_str() , &status);

	fits_create_img(fptr, BYTE_IMG, naxis, naxes, &status);

	nelements = naxes[0] * naxes[1];
    //std::cout << nelements << std::endl;

	fits_write_img(fptr, TBYTE, fpixel, nelements, data, &status);
	//add keys to the header.
	for (unsigned int i=0; i<__keys.size(); i++)
		putKey(fptr,status, __keys[i]);

	//add cards to the header.
	for (unsigned int i=0; i<__cards.size(); i++)
		fits_insert_record(fptr, 6, (char *) __cards[i].c_str(), &status);

	fits_close_file(fptr, &status);

	fits_report_error(stderr, status);
	return status;
}

int FitsTool::readImage(string filename, std::vector<ushort> & img, int & width, int & height)
{
	fitsfile *fptr;       //pointer to the FITS file; defined in fitsio.h
	int status = 0;
	int anynull;
	char comment[80];
	unsigned short nullval;
	long fpixel = 1, size;

	fits_open_file ( &fptr, filename.c_str(), READONLY, &status );

	fits_read_key ( fptr, TINT, "NAXIS1", &width, comment, &status );
	fits_read_key ( fptr, TINT, "NAXIS2", &height, comment, &status );

	size = width * height;
	img.resize ( size );

	fits_read_img ( fptr, TSHORT, fpixel, size, &nullval, &img[0], &anynull, &status );

	fits_close_file ( fptr, &status );         // close the file
	fits_report_error ( stderr, status );      // print out any error messages
	return status;
}


void FitsTool::putKey(fitsfile *fptr, int &status, fitskey fk)
{
	int i = StrToType(fk.datatype);
	int tint;
	double tdouble;
	short int tshort;
	unsigned short int tushort;
	unsigned int tuint;
	long int tlong;
	float tfloat;
	char tstr[30];

	switch(i)
	{
	  case TINT: 	tint = atoi(fk.value.c_str());
			fits_write_key(fptr, TINT, strdup(fk.keyname.c_str()), &tint , strdup(fk.comment.c_str()), &status);
			break;
	  case TDOUBLE:	tdouble = atof(fk.value.c_str());
			fits_write_key(fptr, TDOUBLE, strdup(fk.keyname.c_str()), &tdouble , strdup(fk.comment.c_str()), &status);
			break;
	  case TLOGICAL:tint = atoi(fk.value.c_str());
			fits_write_key(fptr, TLOGICAL, strdup(fk.keyname.c_str()), &tint , strdup(fk.comment.c_str()), &status);
			break;
	  case TUSHORT:	tushort = atoi(fk.value.c_str());
			fits_write_key(fptr, TUSHORT, strdup(fk.keyname.c_str()), &tushort , strdup(fk.comment.c_str()), &status);
			break;
	  case TSHORT:	tshort = atoi(fk.value.c_str());
			fits_write_key(fptr, TSHORT, strdup(fk.keyname.c_str()), &tshort , strdup(fk.comment.c_str()), &status);
			break;
	  case TUINT:	tuint = atoi(fk.value.c_str());
			fits_write_key(fptr, TUINT, strdup(fk.keyname.c_str()), &tuint , strdup(fk.comment.c_str()), &status);
			break;
	  case TLONG:	tlong = atol(fk.value.c_str());
			fits_write_key(fptr, TLONG, strdup(fk.keyname.c_str()), &tlong , strdup(fk.comment.c_str()), &status);
			break;
	  case TFLOAT:	tfloat = atof(fk.value.c_str());
			fits_write_key(fptr, TFLOAT, strdup(fk.keyname.c_str()), &tfloat , strdup(fk.comment.c_str()), &status);
			break;
	  case TSTRING:	strcpy(tstr,fk.value.c_str());
	  		fits_write_key(fptr, TSTRING, strdup(fk.keyname.c_str()), &tstr, strdup(fk.comment.c_str()), &status);
	}
}




int FitsTool::StrToType(string type)
{
	if (type == "TINT") return TINT;
	if (type == "TDOUBLE") return TDOUBLE;
	if (type == "TLOGICAL") return TLOGICAL;
	if (type == "TUSHORT") return TUSHORT;
	if (type == "TSHORT") return TSHORT;
	if (type == "TUINT") return TUINT;
	if (type == "TLONG") return TLONG;
	if (type == "TFLOAT") return TFLOAT;
	if (type == "TCOMPLEX") return TCOMPLEX;
	if (type == "TSTRING") return TSTRING;
	return -1;
}

}
