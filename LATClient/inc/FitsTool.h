#ifndef FITSTOOL
#define FITSTOOL

#include <string>
#include <vector>
#include <fitsio.h>
#include <stdint.h>

#define USHORT uint16_t
#define SHORT int16_t



namespace lbtlat {

//! Structure to allocate the fits header keywords, later on added to the fits file.
struct fitskey
{
	std::string datatype;
	std::string keyname;
	std::string value;
	std::string comment;
};

/**
* @class FitsTool
* @author Jose Borelli <borelli@mpia.de>
* @brief  Wrapper class for the NASA cfitsio libray. Provides some tools to deal with Fits files.
* @todo Needs a logger, exceptions.
*/
class FitsTool
{
 public:

	/** @brief Class constructor */
	FitsTool (){};

	/** @brief Class destructor */
	virtual ~FitsTool (){};




	/** @brief Creates a SHORT_IMG fits file
	@param filename Fits file name.
	@param data Raw image.
	@param width Image width
	@param height Image height
	@return status of the command. */
	int write16(std::string filename, USHORT * data, int width, int height);

	/** @brief Creates an integer fits file.
	@param filename Fits file name.
	@param data Raw image.
	@param width Image width
	@param height Image height
	@return status of the command. */
	int write8(std::string filename, unsigned char * data, int width, int height);

	/** @brief Open the specified fits file and allocates the raw data into the ushort vector.
	@param filename Fits file name.
	@param img allocated raw image.
	@param width Image width.
	@param height Image height.
	@return status of the command. */
	int readImage(std::string filename, std::vector<ushort> & img, int & width, int & height);

	/** @brief Set the fits file name prefix.
	@param prefix The name prefix. */
	void setPrefix(std::string prefix) { __prefix = prefix; };

	 /** @brief Get the fits file prefix.
	@return The fits file name prefix. */
	std::string getPrefix() { return __prefix; };

	/** @brief Set the working directory.
	@param dir The working path.*/
	void setDirectory(std::string dir) { __dir = dir; };

	/** @brief Get the working directory.
	@return The working path. */
	std::string getDirectory() { return __dir; };

	/** @brief Defines the list of the fits header keywords and its values.
	@param keys The list of keyword to be added.*/
	void setKeys(std::vector<fitskey> keys) { __keys = keys; };

	/** @brief Adds a new fits header keyword to the file.
	@param k The keyword to be added. */
	void addKey(fitskey k) { __keys.push_back(k); };

	/** @brief clearKeys clears the list of keywords. */
	void clearKeys(){ __keys.clear(); };

	/** @brief Defines the list of the fits cards.
	@param cards the list of cards to be added.*/
	void setCards(std::vector<std::string> cards) { __cards = cards; };

	/** @brief Adds a new card to the file.
	@param card The card to be added. */
	void addCard(std::string card) { __cards.push_back(card); };

	/** @brief clearCards clears the list of cards. */
	void clearCards() { __cards.clear(); };

 private:
	std::string	__dir;
	std::string 	__prefix;
	std::vector<fitskey>	__keys;
	std::vector<std::string>	__cards;

	/** @brief Private function to put the keywords into the Fits header.
	@param fptr Fits file pointer.
	@param status Status of the command..
	@param fk Fits keyword. */
	void putKey(fitsfile *fptr, int &status, fitskey fk);

	/** @brief Private function to get the keyword data type from a string.
	@param type The string representation of the data type.
	@return the enumerated index representation of the data type. */
	int StrToType(std::string type);

};

}

#endif


