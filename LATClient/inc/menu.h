#ifndef TCAMMENU_H
#define TCAMMENU_H

#include <iostream>
#include <map>
#include <string.h>

namespace lbtlat {

enum commandNames {
	CAMPOWERON, CAMPOWEROFF, CAMPOWERSTAT, GETCAMPARAM, SETCAMPARAM, GETIMAGE, SHUTTEROPEN, SHUTTERCLOSE, SHUTTERSTAT, FOCHOME, FOCMOVEREL, FOCMOVEABS, FOCPOS, FOCSTOP, HELP, LIST
};

enum camParams {
    HEIGHT, WIDTH, EXPTIME, BINNINGX, BINNINGY
};

/**
* @class menu
* @author Jose Borelli <borelli@mpia.de>
* @brief is responisble of generate the irc menu, control the options, and print out the main menu.
*/
class menu{
public:
	/** @brief class constructor */
	menu();

	/** @brief class destructor */
	~menu();

	/** @brief Prints out the command list, in alphabetical order.
	 * @param IRVmajor Major version
	 * @param IRVminor Minor Version
	 * @param IRVrelease release number */
	void printMain(int IRVmajor, int IRVminor);

	/** @brief Prints out the command list, in alphabetical order.*/
	void list();

	/** @brief Check if the given command exist. In positive case returns the command number associated to.
	 * @param cmd the command to execute
	 * @return the command number, or -1 in case of error. */
	int CheckCmd(std::string cmd);


	/** @brief Check if the given parameter exist. In positive case returns the parameter number associated to.
	 * @param param the parameter to execute
	 * @return the parameter number, or -1 in case of error. */
	int CheckParam(std::string param);


	/** @brief Get the help message associated to the specified command number.
	 * @param cmd The command number
	 * @return the corresponding help message */
	std::string HelpCmd(int cmd);

private:
	std::map<std::string,int> commands;
	std::map<std::string,int> params;
};

}
#endif
