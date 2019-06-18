#ifndef OptionsIO_h
#define OptionsIO_h
/* *************************************************************************
   project:      multipurpose library
   subproject:   options library
   module:       OptionsIO
   purpose:      Static helper methods for parsing and loading of options
   begin:        03.03.2004
   copyright:    (C) Daniel Krajzewicz
   email:        daniel@krajzewicz.de
   *************************************************************************
   Version:
   Remarks:
   *************************************************************************
   ToDo:
   *********************************************************************** */

/* =========================================================================
 * global configuration import
 * ======================================================================= */
#if defined _MSC_VER || defined __BORLANDC__
#include <windows_config.h>
#else
#include <config.h>
#endif



/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>

/* -------------------------------------------------------------------
 * (optional) memory checking
 * ----------------------------------------------------------------- */
#ifdef DEBUG_ALLOC_NVWA
#include <debug_new.h>
#endif



/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionCont;



/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class OptionsIO
 * @brief Static helper methods for parsing and loading of options.
 *
 * To enable loading of XML-configurations, USE_XML_OPTIONS must be defined.
 *  In this case, the configuration is loaded using the xerces-parser.
 */
class OptionsIO {
public:
    /** @brief Parses options from the command line and optionally loads options from a configuration file
	 * @param[in] into The options container to fill
	 * @param[in] argc The number of arguments given on the command line
	 * @param[in] argv The arguments given on the command line
  	 * @param[in] configurationName The path to the configuration to load (XML)
	 * @param[in] continueOnError Continues even if an error occures while parsing
  	 * @param[in] acceptUnknown Unknown options do not throw an exception
     */
    static bool parseAndLoad(OptionCont &into, int argc, char **argv,
		const std::string &configurationName, bool continueOnError=false,
		bool acceptUnknown=false);


private:
    /** @brief Loads options from a configuration file
	 * @param[in] into The options container to fill
	 * @param[in] configurationName The path to the configuration to load (XML)
	 */
    static bool load(OptionCont &into, const std::string &configurationName);


};


// *************************************************************************
#endif
