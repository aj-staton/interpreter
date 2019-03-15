#include "hex.h"
/***************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456
 * Class 'Hex' as a container for one hex operand.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Austin Staton
 * Date: 21 October 2018
 *
 * We use an instance of a class for a hex operand since there are
 * a number of things we need to do with the operand and a number of tests
 * to be performed on the operands to make sure they are legal, etc.
 *
 * Variables used in this class:
 *
 *  bool is_invalid_       : is this operand invalid?
 *  bool is_negative_      : is this operand negative?
 *  bool is_null_          : is this operand null (i.e., blank)?
 *  int value_             : the actual 'int' translation of the hex
 *  string error_messages_ : error messages about invalidity
 *  string text_           : the actual source text of the operand
 *
 * Note that "null" is used as the flag for an opcode's target if that
 * opcode is actually all blanks.
**/

/***************************************************************************
 * Constructor
**/
Hex::Hex() {
}

/***************************************************************************
 * Constructor
**/
Hex::Hex(string text) {
  this->Init(text);
}

/***************************************************************************
 * Destructor
**/
Hex::~Hex() {
}

/***************************************************************************
 * Accessors and Mutators
**/

/***************************************************************************
 * Accessor for 'error_messages_'.
 * This simply returns an error message string if the 'is_invalid_' flag
 * has been set anywhere during execution of the program.
**/
string Hex::GetErrorMessages() const {
  string error_messages = "";
  return error_messages;
}

/***************************************************************************
 * Accessor for 'text_'.
**/
string Hex::GetText() const {
  return text_;
}

/***************************************************************************
 * Accessor for 'value_'.
**/
int Hex::GetValue() const {
  return value_;
}

/***************************************************************************
 * Accessor for error flag 'is_invalid_'.
 * The answer to 'HasAnError' is 'true' iff 'is_invalid_' is true.
**/
bool Hex::HasAnError() const {
  return is_invalid_;
}

/***************************************************************************
 * Accessor for 'is_negative_'.
**/
bool Hex::IsNegative() const {
  return is_negative_;
}

/***************************************************************************
 * Accessor for 'is_null_' in the negative.
**/
bool Hex::IsNotNull() const {
  return !is_null_;
}

/***************************************************************************
 * Accessor for 'is_null_' in the positive.
**/
bool Hex::IsNull() const {
  return is_null_;
}

/***************************************************************************
 * General functions.
**/

/***************************************************************************
 * Initialization function.
 * Set the iternal 'text_' variable with the text as read, and then parse
 * the text to create the 'value' and set validity flags.
**/
void Hex::Init(string text) {
  text_ = text;
  ParseHexOperand();
}

/***************************************************************************
 * Function 'ParseHexOperand'.
 * Parses the text into a decimal value and sets the error flag.
 *
 * The text to be parsed must be exactly five characters long
 * with a plus or minus sign present followed by exactly four
 * legitimate hex digit ASCII characters, with the letters uppercase.
**/
void Hex::ParseHexOperand() {
#ifdef EBUG
  Utils::log_stream << "enter ParseHexOperand" << endl;
#endif
  // This should only run if the data is of length 5, since this is a 16
  // bit machine.
  assert(text_.length() == 5);
  // This is used to determine if its needed to multiply the final value by -1.
  if (text_.substr(0, 1) == "+") {
    is_negative_ = false;
  } else if (text_.substr(0, 1) == "-") {
    is_negative_ = true;
  } else {
    is_invalid_ = true;
  }
  // The below conversion was referenced from c++'s website. Specifically,
  // from http://www.cplusplus.com/reference/string/stoi/. Credit belongs
  // to the author of that page.
  string to_convert = text_.substr(1);
  // Convert the hexadecimal to an integer. Use the leading +/- to know if
  // the value is positive or negative.
  value_ = std::stoi(to_convert, nullptr, 16);
  if (is_negative_) value_ *= -1;

#ifdef EBUG
  Utils::log_stream << "leave ParseHexOperand" << endl;
#endif
}

/***************************************************************************
 * Function 'ToString'.
 * This function formats a 'Hex' for prettyprinting.
 * If the operand is null (e.g., for an instruction that does not take an
 * operand, we print five dots.
 *
 * Returns:
 *   the prettyprint string for printing
**/
string Hex::ToString() const {
#ifdef EBUG
  Utils::log_stream << "enter ToString" << endl;
#endif
  string s = "";

  if (text_ == "nullhexoperand") {
    s += Utils::Format(".....", 5);
  } else {
    s += Utils::Format(text_, 5);
  }

#ifdef EBUG
  Utils::log_stream << "leave ToString" << endl;
#endif
  return s;
}
