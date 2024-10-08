/** ************************************************************************
   @project      options library
   @file         tester.cpp
   @since        26.06.2019
   @author       Daniel Krajzewicz (daniel@krajzewicz.de)
   @copyright    Eclipse Public License v2.0 (EPL v2.0), (c) Daniel Krajzewicz 2019-2021
   @brief        The tester application is just for internal testing purposes. 

   It reads a definitions file called "options.txt" which includes 
   definitions of options and other things to set up the options container. 
   Then the application performs the things defined by the 
   http://texttest.sourceforge.net/ test system.
   *********************************************************************** */

/* *************************************************************************
   project:      PaletteConverter
   module:       paletteconverter_main.cpp
   purpose:      The main module
   begin:        10.08.2009
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
#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>

#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsFileIO_CSV.h>
#include <utils/options/OptionsFileIO_XML.h>

/* -------------------------------------------------------------------------
 * (optional) memory checking
 * ----------------------------------------------------------------------- */
#ifdef WANT_MEMCHECK
#include <foreign/nvwa/debug_new.h>
#endif // WANT_MEMCHECK



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;



/* =========================================================================
 * definitions
 * ======================================================================= */
#define APPNAME "options_tester"
#define APPVERSION "1.0.0"



/* =========================================================================
 * static member definitions
 * ======================================================================= */
OptionsCont myOptions;
std::string configOptionName;
OptionsTypedFileIO *fileIO = 0;



/* =========================================================================
 * enum definitions
 * ======================================================================= */
enum ReturnCodes {
    STAT_OK = 0,
    STAT_MISSING_DEFINITION = 1,
    STAT_MISSING_OPTION = 2,
    STAT_FALSE_OPTION = 3,
    STAT_READ_COMMENT = 4
};



ReturnCodes
loadDefinition() {
    ifstream defs("options.txt");
    if(!defs.good()) {
        std::cerr << "Could not open the definitions file ('options.txt')" << std::endl;
        return STAT_MISSING_DEFINITION;
    }
    while(defs.good()) {
        string line;
        std::getline(defs, line);
        if(line.find(';')==string::npos) {
            // ok, it's a subsection
        } else {
            // ok, it's an option
            // ... parse the line
            std::vector<std::string> synonyms;
            char abbr = '!';
            std::string type, description, defaultValue;
            istringstream f(line);
            string s;    
            while (getline(f, s, ';')) {
                if(type.length()==0) {
                    type = s;
                    continue;
                }
                if (s.length()==1) {
                    abbr = s[0];
                } else if(s[0]=='!') {
                    description = s.substr(1);
                }  else if(s[0]=='+') {
                    defaultValue = s.substr(1);
                } else {
                    synonyms.push_back(s);
                }
            }
            // ... is it the tail/head help stuff?
            if(type=="HELPHEADTAIL") {
                myOptions.setHelpHeadAndTail(synonyms[0], synonyms[1]);
                continue;
            }
            if(type=="CONFIG") {
                configOptionName = synonyms[0];
                if(configOptionName.find("xml")!=std::string::npos) {
                    fileIO = new OptionsFileIO_XML();
                } else if(configOptionName.find("csv")!=std::string::npos) {
                    fileIO = new OptionsFileIO_CSV();
                } else {
                    throw std::runtime_error("Unknown configuration format");
                }

                continue;
            }
            // ... is it a named section begin?
            if(type=="SECTION") {
                myOptions.beginSection(synonyms[0]);
                continue;
            }
            // ... build the option, first
            Option *option = 0;
            if(type=="INT") {
                if(defaultValue.length()==0) {
                    option = new Option_Integer();
                } else {
                    int v = atoi(defaultValue.c_str());
                    option = new Option_Integer(v);
                }
            } else if(type=="DOUBLE") {
                if(defaultValue.length()==0) {
                    option = new Option_Double();
                } else {
                    double v = atof(defaultValue.c_str());
                    option = new Option_Double(v);
                }
            } else if(type=="BOOL") {
                if(defaultValue.length()==0) {
                    option = new Option_Bool();
                } else {
                    option = new Option_Bool();
                }
            } else if(type=="STRING") {
                if(defaultValue.length()==0) {
                    option = new Option_String();
                } else {
                    option = new Option_String(defaultValue);
                }
            } else if(type=="FILE") {
                if(defaultValue.length()==0) {
                    option = new Option_FileName();
                } else {
                    option = new Option_FileName(defaultValue);
                }
            }
            // ... add it to the container
            std::string firstName = synonyms.front();
            synonyms.erase(synonyms.begin());
            if(abbr!='!') {
                myOptions.add(firstName, abbr, option);
            } else {
                myOptions.add(firstName, option);
            }
            while(!synonyms.empty()) {
                myOptions.addSynonym(firstName, synonyms.front());
                synonyms.erase(synonyms.begin());
            }
            // ... add description if given
            if(description.length()!=0) {
                myOptions.setDescription(firstName, description);
            }
        }
    }
    return STAT_OK;
}

int
main(int argc, char *argv[]) {
    ReturnCodes ret = STAT_OK;
    // load the definition
    ret = loadDefinition();
    // parse options
    if(ret==STAT_OK) {
        try {
            if(!OptionsIO::parseAndLoad(myOptions, argc, argv, *fileIO, configOptionName)) {
                ret = STAT_READ_COMMENT;
            }
            OptionsIO::printHelp(std::cout, myOptions);
            std::cout << "-------------------------------------------------------------------------------" << std::endl;
            std::cout << myOptions;
            std::cout << "-------------------------------------------------------------------------------" << std::endl;
        } catch(std::exception &e) {
            if(dynamic_cast<std::runtime_error*>(&e)!=0) {
                std::cerr << "Got std::runtime_error: " << dynamic_cast<std::runtime_error*>(&e)->what() << std::endl;
            } else {
                std::cerr << "Got std::exception" << std::endl;
            }
            ret = STAT_READ_COMMENT;
        }
    }
    delete fileIO;
    if(ret!=STAT_OK) {
        cerr << "Quitting (on error)." << endl;
    }
    return ret;
}


// *************************************************************************
