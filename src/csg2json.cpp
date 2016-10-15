#include <iostream>
#include <string>
#include <algorithm>

#include <csgparser.hpp>

using namespace json11;

void printHelp() {

  std::cout << "Usage: csg2json <input_file> <output_file>\n"
               "  csg2json converts CSG files to CSGJS and vice versa.\n"
               "  Example:\n"
               "    csg2json input.csg output.csgjs\n";
}

//! Extracts file extension
std::string getFileExtension (const std::string& theFileName) {

  std::string anExt;
  std::string::size_type anIdx = theFileName.rfind (".");
  if (anIdx != std::string::npos) {
    anExt = theFileName.substr (anIdx + 1);
  }
  return anExt;
}

//! Converts string to lower case
std::string toLower (const std::string& theString) {

  std::string aRes = theString; 
  // no Unicode please
  std::transform (theString.begin(), theString.end(), aRes.begin(), ::tolower);
  return aRes;
}

int main (int argc, char ** argv) {

  if (argc != 3) {
    printHelp();
    return 0;
  }

  Json aData;

  std::string anInputExt = toLower (getFileExtension (argv[1]));

  if (anInputExt == "csg") {
    aData = csg::Parser::parse (argv[1]);
  }
  else if (anInputExt == "csgjs") {
    aData = csg::Parser::parseJSON (argv[1]);
  }
  else {
    std::cout << "Unrecognized extension: " << anInputExt << std::endl;
    return 1;
  }

  std::string anOutputExt = toLower (getFileExtension (argv[2]));

  if (anOutputExt == "csg") {
    csg::Parser::write (aData, argv[2]);
  }
  else if (anOutputExt == "csgjs") {
    csg::Parser::writeJSON (aData, argv[2]);
  }
  else {
    std::cout << "Unrecognized extension: " << anOutputExt << std::endl;
    return 1;
  }
 
  return 0;
}
