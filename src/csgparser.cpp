#include <iostream>
#include <fstream>
#include <cmath>
#include <list>

#include <json11/json11.hpp>
#include <parser/parser.h>

#include <csgparser.hpp>

using namespace lars;

namespace csg {

//! Helper class for CSG format parsing.
class CsgVisitor{  
  
public:

  CsgVisitor()
    : m_versionName ("undefined"),
      m_majorVersion (0),
      m_minorVersion (0) {}

  json11::Json getResult() {

    json11::Json::array aContents = json11::Json::array();

    for (auto anObject: m_value.array_items()) {
      if (!anObject.is_null()) {
        aContents.push_back (anObject);
      }
    }

    json11::Json::object aResult = json11::Json::object({
      { "type", "CSG file" },
      { "version-name", m_versionName },
      { "version-major", m_majorVersion },
      { "version-minor", m_minorVersion },
      { "contents", aContents },
    });

    return aResult;
  }

  json11::Json getValue (expression<CsgVisitor> e) {

    e.accept (this);
    return m_value;
  }

  std::pair<std::string, json11::Json> getProperty (expression<CsgVisitor> e) {

    return std::make_pair (e[0].string(), getValue (e[1]));
  }

  void visitNumber (expression<CsgVisitor> e) {

    m_value = std::stod (e.string());
  }

  void visitBoolean (expression<CsgVisitor> e) {

    m_value = e.string() == "true";
  }

  void visitString (expression<CsgVisitor> e) {

    m_value = e.string();
  }

  void visitVersion (expression<CsgVisitor> e) {

    if (e.size() != 3) {
      std::cout << "Warning: incorrect version string: " << e.string() << std::endl;
    }

    m_versionName = e[0].string();
    m_majorVersion = std::stoi (e[1].string());
    m_minorVersion = std::stoi (e[2].string());
  }

  void visitProperties (expression<CsgVisitor> e) {

    json11::Json::object aProps;

    for (int i = 0; i < e.size(); ++i) {
      auto aProperty = getProperty (e[i]);
      aProps[aProperty.first] = aProperty.second;
    }

    m_value = aProps;
  }

  void visitArray (expression<CsgVisitor> e) {

    json11::Json::array anArray;

    for (int i = 0; i < e.size(); ++i) {
      anArray.push_back (getValue (e[i]));
    }

    m_value = anArray;
  }
 
  void visitObject (expression<CsgVisitor> e) {

    json11::Json::object anObject = json11::Json::object({
      { "type", e[0].string() },
      { "properties", getValue (e[1]) },
    });

    m_value = anObject;
  }

  void visitInstruction (expression<CsgVisitor> e) {

    json11::Json::object anInstruction = json11::Json::object({
      { "type", e[0].string() },
      { "properties", getValue (e[1]) },
      { "objects", getValue (e[2]) },
    });

    m_value = anInstruction;
  }

  void printVersion() {
    std::cout << m_versionName << " " << m_majorVersion << "." << m_minorVersion << std::endl;
  }

private:

  json11::Json m_value;

  std::string m_versionName;
  int m_majorVersion;
  int m_minorVersion;
  
};

//! Creates the parser built on CSG format grammar.
static lars::parser<CsgVisitor> createParser() {

  parsing_expression_grammar_builder<CsgVisitor> aGrammar;
  using expression = expression<CsgVisitor>;

  auto aPropagateFn = [](expression e){ for (auto c : e) c.accept(); };

  aGrammar["File"        ] << "ObjectList &'\\0'"                                 << aPropagateFn;
  aGrammar["ObjectList"  ] << "( Comment | Object | Instruction )+"               << [](expression e){ e.visitor().visitArray (e); };
  aGrammar["Comment"     ] << "'#' ( VesrionStr | CommentStr )"                   << aPropagateFn;
  aGrammar["CommentStr"  ] << "(!('\n') .)*"                                      << [](expression e){ };
  aGrammar["VesrionStr"  ] << "VersionName VersionInt '.' VersionInt"             << [](expression e){ e.visitor().visitVersion (e); };
  aGrammar["VersionInt"  ] << "[0-9]"                                             ;
  aGrammar["VersionName" ] << "(!(['\n' ]) .)+"                                   ;
  aGrammar["Instruction" ] << "Name '(' ( Matrix | PropertyList ) ')' InstrBody"  << [](expression e){ e.visitor().visitInstruction (e); };
  aGrammar["InstrBody"   ] << "'{' ObjectList '}'"                                << aPropagateFn;
  aGrammar["Object"      ] << "Name '(' PropertyList ')' ';'"                     << [](expression e){ e.visitor().visitObject (e); };
  aGrammar["PropertyList"] << " Property ( ',' Property )* | Property? "          << [](expression e){ e.visitor().visitProperties (e); };
  aGrammar["Property"    ] << "( Name '=' ( Value ) )"                            ;
  aGrammar["Matrix"      ] << "Array"                                             << aPropagateFn;
  aGrammar["Array"       ] << "'[' ( Value ( ',' Value )* | (Value)? ) ']'"       << [](expression e){ e.visitor().visitArray (e); };
  aGrammar["Value"       ] << "Number | '\"' String '\"' | Boolean | Array"       << aPropagateFn;
  aGrammar["Number"      ] << "'-'? [0-9]+ ('.' [0-9]+)? ('e' '-'? [0-9]+)?"      << [](expression e){ e.visitor().visitNumber (e); };
  aGrammar["Boolean"     ] << "'true' | 'false'"                                  << [](expression e){ e.visitor().visitBoolean (e); };
  aGrammar["Name"        ] << "'$'? [a-zA-Z] [a-zA-Z]*"                           ;
  aGrammar["String"      ] << "(!('\"') .)*"                                      << [](expression e){ e.visitor().visitString (e); };

  aGrammar.set_starting_rule ("File");

  aGrammar["Whitespace"] << "[ \t\n\r]";

  aGrammar.set_separator_rule ("Whitespace");

  return aGrammar.get_parser();
}

json11::Json Parser::parse (const std::string theFilePath) {

  auto aParser = createParser();
  CsgVisitor aVisitor;

  std::ifstream aStream (theFilePath);
  std::stringstream aBuffer;
  aBuffer << aStream.rdbuf();

  try { 
    aParser.parse (aBuffer.str()).accept (&aVisitor); 
  }
  catch (parser<CsgVisitor>::error e) {
    for(auto i UNUSED :range (e.begin_position().character - 1)) {
      std::cout << " ";
    }
    for(auto i UNUSED :range (e.length())) {
      std::cout << "~";
    }
    std::cout << "^\n";
    std::cout << e.error_message() << " while parsing " << e.rule_name() << std::endl;
    std::cout << e.what() << std::endl;
  }

  return aVisitor.getResult();
}

json11::Json Parser::parseJSON (const std::string theFilePath) {
  
  std::ifstream aStream (theFilePath);
  std::stringstream aBuffer;
  aBuffer << aStream.rdbuf();

  std::string anErrors;
  json11::Json aCsg = json11::Json::parse (aBuffer.str(), anErrors);

  if (!anErrors.empty()) {
    std::cout << anErrors << std::endl;
  }

  validate (aCsg);

  return aCsg;
}

void Parser::writeProperties (std::ostream& theStream, const json11::Json theData) {
  if (!theData["properties"].is_object()) {
    throw std::runtime_error ("Object properties should be represented with a dictionary");
  }

  if (theData["properties"].object_items().empty()) {
    return; // not an error
  }

  auto aProperties = theData["properties"].object_items();

  // comma separated lists are messy
  auto anIter = aProperties.begin();
  theStream << anIter->first << " = " << anIter->second.dump();
  anIter++;
  for (; anIter != aProperties.end(); ++anIter) {

    theStream << ", " << anIter->first << " = " << anIter->second.dump();
  }
}

void Parser::writeObject (std::ostream& theStream, const json11::Json theData, const std::string& theIndent) {

  theStream << theIndent << theData["type"].string_value() << "(";
  writeProperties (theStream, theData);
  theStream << ");\n";
}

void Parser::writeInstruction (std::ostream& theStream, const json11::Json theData, const std::string& theIndent) {

  theStream << theIndent << theData["type"].string_value() << "(";
  writeProperties (theStream, theData);
  theStream << ") {\n";
  for (auto& anObject: theData["objects"].array_items()) {
    writeData (theStream, anObject, theIndent + "  ");
  }
  theStream << theIndent << "}\n";
}

void Parser::assertChildrenNum (const json11::Json theData, int theChildrenNum) {
  
  if (theData["objects"].array_items().size() < 2) {
    throw std::runtime_error ("Too few children objects for instruction: " + theData["type"].dump());
  }
}

void Parser::writeData (std::ostream& theStream, const json11::Json theData, const std::string& theIndent) {

  if (theData.is_array()) {
    for (auto& anObject: theData.array_items()) {
      writeData (theStream, anObject, theIndent);
    }

    return;
  }

  if (theData.is_null()) {
    throw std::runtime_error ("Unexpected NULL object");
  }

  if (!theData.is_object()) {
    throw std::runtime_error ("Dictionary expected: " + theData.dump());
  }

  std::string aType = theData["type"].string_value();

  if (aType == "CSG file") {

    theStream << theIndent << "# " << theData["version-name"].string_value() <<
                              " " << theData["version-major"].dump() <<
                              "." << theData["version-minor"].dump() << std::endl;

    writeData (theStream, theData["contents"], theIndent);
  }
  else if (aType == "group") {

    // OpenScad compatibility empty group
    if (theData["objects"].array_items().empty()) {
      theStream << theIndent << "group();" << std::endl;
    }
    else {
      assertChildrenNum (theData, 1);
      writeInstruction (theStream, theData, theIndent);
    }
  }
  else if (aType == "multmatrix") {

    // OpenScad compatibility matrix
    if (theData["properties"].is_array()) {

      // TODO: validate matrix
      theStream << theIndent << theData["type"].string_value() << "(";
      theStream << theData["properties"].dump();
      theStream << ") {\n";
      for (auto& anObject: theData["objects"].array_items()) {
        writeData (theStream, anObject, theIndent + "  ");
      }
      theStream << theIndent << "}\n";
    }
    else {
      writeInstruction (theStream, theData, theIndent);
    }
  }
  else if (aType == "union") {
    assertChildrenNum (theData, 2);
    writeInstruction (theStream, theData, theIndent);
  }
  else if (aType == "difference") {
    assertChildrenNum (theData, 2);
    writeInstruction (theStream, theData, theIndent);
  }
  else if (aType == "intersection") {
    assertChildrenNum (theData, 2);
    writeInstruction (theStream, theData, theIndent);
  }
  else if (aType == "smin") {
    assertChildrenNum (theData, 2);
    writeInstruction (theStream, theData, theIndent);
  }
  else if (aType == "cube") {
    writeObject (theStream, theData, theIndent);
  }
  else if (aType == "sphere") {
    writeObject (theStream, theData, theIndent);
  }
  else if (aType == "cylinder") {
    writeObject (theStream, theData, theIndent);
  }
  else if (aType == "cone") {
    writeObject (theStream, theData, theIndent);
  }
  else {
    throw std::runtime_error ("Unknown object type: " + theData["type"].dump());
  }
}

void Parser::write (const json11::Json theData, const std::string theFilePath) {

  std::ofstream aFile (theFilePath);
  writeData (aFile, theData, "");
  aFile.close();
}

void Parser::validate (const json11::Json theData) {

  std::stringstream aBuffer;
  try { 
    writeData (aBuffer, theData, "");
  }
  catch (std::exception e) {
    std::cout << "Serialization error: " << e.what() << std::endl;
    return;
  }

  auto aParser = createParser();
  CsgVisitor aVisitor;

  try { 
    aParser.parse (aBuffer.str()).accept (&aVisitor); 
  }
  catch (parser<CsgVisitor>::error e) {
    std::cout << "Validation error: " << e.what() << std::endl;
  }
}

void Parser::writeJSON (const json11::Json theData, const std::string theFilePath) {

  validate (theData);
  
  std::string aCsgJs = theData.dump();

  std::ofstream aFile;
  aFile.open (theFilePath);
  aFile << aCsgJs;
  aFile.close();
}

} // csg