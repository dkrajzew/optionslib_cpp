/** ************************************************************************
   @project      options library
   @file         OptionsXercesHandler.cpp
   @since        03.03.2004
   @author       Daniel Krajzewicz (daniel@krajzewicz.de)
   @copyright    Eclipse Public License v2.0 (EPL v2.0), (c) Daniel Krajzewicz 2004-2021
   *********************************************************************** */

/* =========================================================================
* compile only if XML is supported
* ======================================================================= */
#ifdef USE_XERCES_XML



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
#include <sax2/Attributes.hpp>
#include <sax2/DefaultHandler.hpp>
#include <util/PlatformUtils.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <sax2/XMLReaderFactory.hpp>
#include <iostream>
#include <string>
#include "OptionsXercesHandler.h"
#include "OptionsCont.h"

/* -------------------------------------------------------------------------
 * (optional) memory checking
 * ----------------------------------------------------------------------- */
#ifdef DEBUG_ALLOC_NVWA
#include <debug_new.h>
#endif



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;
using namespace XERCES_CPP_NAMESPACE;



/* =========================================================================
 * method definitions
 * ======================================================================= */
OptionsXercesHandler::OptionsXercesHandler(OptionsCont &options, const std::string &file) 
    : myOptions(options), myHadError(false), myFileName(file) {
}


OptionsXercesHandler::~OptionsXercesHandler() {
}


void
OptionsXercesHandler::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs) {
    myCurrentOptionName = convert(localname);
}


void
OptionsXercesHandler::characters(const XMLCh* const chars, const XMLSize_t length) {
    if(myOptions.contains(myCurrentOptionName) && myOptions.canBeSet(myCurrentOptionName)) {
        myOptions.set(myCurrentOptionName, convert(chars));
    }
    myCurrentOptionName = "";
}


void
OptionsXercesHandler::warning(const SAXParseException& exception) {
    std::cerr << "Warning:" << std::endl << convert(exception.getMessage()) << std::endl << " in " << myFileName << '.' << std::endl;
}


void
OptionsXercesHandler::error(const SAXParseException& exception) {
    std::cerr << "Error:" << std::endl << convert(exception.getMessage()) << std::endl << " in " << myFileName << '.' << std::endl;
    myHadError = true;
}


void
OptionsXercesHandler::fatalError(const SAXParseException& exception) {
    std::cerr << "Error:" << std::endl << convert(exception.getMessage()) << std::endl << " in " << myFileName << '.' << std::endl;
    myHadError = true;
}


bool
OptionsXercesHandler::errorOccured() {
    return myHadError;
}


std::string 
OptionsXercesHandler::convert(const XMLCh * const str) {
    std::string ret;
    for (size_t i=0; str[i]!=0; ++i) {
        ret += (char) str[i];
    }
    return ret;
}



#endif // USE_XERCES_XML



// *************************************************************************
