#include <string>

#include <json11/json11.hpp>

// TODO: export for windows dll
#define CSG_EXPORT

namespace csg {

class Parser {
  
public:

  //! Reads CSG file.
  CSG_EXPORT static json11::Json parse (const std::string theFilePath);

  //! Reads CSGJS file.
  CSG_EXPORT static json11::Json parseJSON (const std::string theFilePath);

  //! Validates CSG data.
  CSG_EXPORT static void validate (const json11::Json theData);

  //! Writes CSG file.
  CSG_EXPORT static void write (const json11::Json theData, const std::string theFilePath);

  //! Writes CSGJS file.
  CSG_EXPORT static void writeJSON (const json11::Json theData, const std::string theFilePath);

private:

  // Serializes JSON objects into CSG format.
  static void writeData (std::ostream& theStream, const json11::Json theData, const std::string& theIndent);

  // Serializes properties of objects and instructions.
  static void writeProperties (std::ostream& theStream, const json11::Json theData);

  // Serializes CSG object.
  static void writeObject (std::ostream& theStream, const json11::Json theData, const std::string& theIndent);

  // Serializes CSG instruction.
  static void writeInstruction (std::ostream& theStream, const json11::Json theData, const std::string& theIndent);

  //! Checks if object has at least specified children count
  static void assertChildrenNum (const json11::Json theData, int theChildrenNum);
  
};

} // csg