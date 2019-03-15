# include "pullet16interpreter.h"

/***************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456
 * Class 'Interpreter' for interpreting the Pullet16 code.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Austin Staton
 * Date: 21 October 2018
 *
 * This class does the interpretation of ASCII versions of 'executable'
 * files for the Pullet16.
 *
 * The 'Read' function reads an executable into memory, and then the
 * 'Interpret' function simulates execution of the Pullet16 using the
 * code loaded into the memory.
 *
 * Documentation for details of the functions is done as headers for
 * the functions.
 *
 * Notes:
 *   Notice throughout that 'GetTargetLocation' does the error checking
 *   for whether a target address is in fact out of bounds and also does
 *   the indirect lookup, so the code to interpret an instruction never
 *   needs to know whether the address is indirect or not.
**/

/***************************************************************************
 * Constructor
**/
Interpreter::Interpreter() {
}

/***************************************************************************
 * Destructor
**/
Interpreter::~Interpreter() {
}

/***************************************************************************
 * Accessors and Mutators
**/

/***************************************************************************
 * General functions.
**/

/***************************************************************************
 * Function 'DoADD'.
 * This top level function interprets the 'ADD' opcode.
 *
 * Load the contents from the 'target'.
 * Convert the 16-bit value to its 2s complement version as a 32-bit value.
 * Add, storing the result in the accumulator.
 * Arithmetic overflow causes the top bits to be lost but is not flagged
 *   as an error.  It's just the way hardware works.
**/
void Interpreter::DoADD(string addr, string target) {
#ifdef EBUG
  Utils::log_stream << "enter DoADD" << endl;
#endif

  Utils::log_stream << "EXECUTE:    OPCODE ADDR TARGET " << "ADD        "
                    << addr << " " << target << endl;
  /* Go to needed location. Get its contents. Convert to a 32 bit
   * Two's Complement value. Add it to the existing accumulator.
  **/
  int location = GetTargetLocation(addr, target);
  int val = DABnamespace::BitStringToDec(memory_.at(location).GetBitPattern());
  int converted_value = TwosComplementInteger(val);
  accum_ = TwosComplementInteger(accum_) + converted_value;

#ifdef EBUG
  Utils::log_stream << "leave DoADD" << endl;
#endif
}

/***************************************************************************
 * Function 'DoAND'.
 * This top level function interprets the 'AND' opcode.
 *
 * Load the contents from the 'target', taking indirection into account.
 * AND, storing the result in the accumulator.
**/
void Interpreter::DoAND(string addr, string target) {
#ifdef EBUG
  Utils::log_stream << "enter DoAND" << endl;
#endif
  Utils::log_stream << "EXECUTE:    OPCODE ADDR TARGET " << "AND "
                    << addr << " " << target << endl;
  /* Get target location. Get the contents to and to the accumulator. 
   * AND the contents together with the accumulator bit by bit.
  **/
  int location = GetTargetLocation(addr, target);
  int add = DABnamespace::BitStringToDec(memory_.at(location).GetBitPattern());
  accum_ &= add;
#ifdef EBUG
  Utils::log_stream << "leave DoAND" << endl;
#endif
}

/***************************************************************************
 * Function 'DoBAN'.
 * This top level function interprets the 'BAN' opcode.
 *
 * If the accumulator value is negative, branch to the target location.
 * Otherwise, just continue on continuing on.
**/
void Interpreter::DoBAN(string addr, string target) {
#ifdef EBUG
  Utils::log_stream << "enter DoBAN" << endl;
#endif
  Utils::log_stream << "OPCODE ADDR TARGET " << "BAN " << addr << " "
                    << target << endl;
  // Ensure that the accumulator is negative to branch. Hence,
  // "Branch Accumulator Negative". If negative, branch (jump)
  // to the target location.
  if (accum_ < 0) {
    pc_ = GetTargetLocation(addr, target);
  } else {
    Utils::log_stream << "the accumulator was not negative." << endl;
  }
#ifdef EBUG
  Utils::log_stream << "leave DoBAN" << endl;
#endif
}

/***************************************************************************
 * Function 'DoBR'.
 * This top level function interprets the 'BR' opcode.
 *
 * Branch unconditionally to the target location.
**/
void Interpreter::DoBR(string addr, string target) {
#ifdef EBUG
  Utils::log_stream << "enter DoBR" << endl;
#endif
  Utils::log_stream << "OPCODE ADDR TARGET " << "BR  " << addr << " "
                    << target << endl;
  // Branch (jump in memory) to the target location.
  pc_ = GetTargetLocation(addr, target);
#ifdef EBUG
  Utils::log_stream << "leave DoBR" << endl;
#endif
}

/***************************************************************************
 * Function 'DoLD'.
 * This top level function interprets the 'LD' opcode.
 *
 * Load the accumulator with the contents of the target location.
 * The contents are the last twelve bits. Meaning, the first four that have 
 * the opcode and addressing will be ignored.
**/
void Interpreter::DoLD(string addr, string target) {
#ifdef EBUG
  Utils::log_stream << "enter DoLD" << endl;
#endif
  Utils::log_stream << "EXECUTE:    OPCODE ADDR TARGET " << "LD         "
                    << addr << " " << target << endl;
  // Get the target location to load. Load (make the accumulator)
  // the value found by the target location.
  int location = GetTargetLocation(addr, target);
  int add = DABnamespace::BitStringToDec(memory_.at(location).GetAddressBits());
  accum_ = add;
#ifdef EBUG
  Utils::log_stream << "leave DoLD" << endl;
#endif
}

/***************************************************************************
 * Function 'DoRD'.
 * This top level function interprets the 'RD' opcode.
 *
 * If there is more data:
 *   scan the next line for input
 *   convert from the hex character format input into an int value
 *   store the int value in the accumulator
 * Else:
 *   crash on read past end of file
**/
void Interpreter::DoRD(Scanner& data_scanner) {
#ifdef EBUG
  Utils::log_stream << "enter DoRD" << endl;
#endif
  Utils::log_stream << "OPCODE " << "RD  " << endl;
  Utils::log_stream << std::boolalpha << data_scanner.HasNext() << endl;
  // ".empty()" returns true if there is nothing else in the vector.
  // Referenced from c++.com

  // Read the needed file as long for as long as there is more data to read.
  // Convert the hexadecimal data to a decimal, then make that the accumulator.

  if (data_scanner.HasNext()) {
    string line = data_scanner.NextLine();
    Hex convert = Hex(line);
    accum_ = TwosComplementInteger(convert.GetValue());
  } else {
    exit(1);
  }
#ifdef EBUG
  Utils::log_stream << "leave DoRD" << endl;
#endif
}

/***************************************************************************
 * Function 'DoSTC'.
 * This top level function interprets the 'STC' opcode.
 *
 * Get the target location.
 * Store the accumulator at that location.
 * Zero the accumulator.
 *
 * This assumes that 'GetTargetLocation' does the error checking for invalid
 * addresses.
**/
void Interpreter::DoSTC(string addr, string target) {
#ifdef EBUG
  Utils::log_stream << "enter DoSTC" << endl;
#endif
  Utils::log_stream << "EXECUTE:    OPCODE ADDR TARGET " << "STC        "
                    << addr << " " << target << endl;
  // Get the target location. Make the address in memory at that location
  // the value of the accumulator. Reset the accumulator.
  int location = GetTargetLocation(addr, target);
  string to_store = DABnamespace::DecToBitString(accum_, 16);
  memory_.at(location) = (to_store);
  accum_ = 0;

#ifdef EBUG
  Utils::log_stream << "leave DoSTC" << endl;
#endif
}

/***************************************************************************
 * Function 'DoSTP'.
 * This top level function interprets the 'STP' opcode.
 *
 * Since the function one higher up interprets the 'kPCForStop' value as
 * the indicator to stop execution, all we need to do here is assign that
 * constant to the program counter.
**/
void Interpreter::DoSTP() {
#ifdef EBUG
  Utils::log_stream << "enter DoSTP" << endl;
#endif
  Utils::log_stream << "OPCODE " << "STP " << endl;
  // Give a value to know when to stop.
  pc_ = kPCForStop;
#ifdef EBUG
  Utils::log_stream << "leave DoSTP" << endl;
#endif
}

/***************************************************************************
 * Function 'DoSUB'.
 * This top level function interprets the 'SUB' opcode.
 * 
 * Subtract contents of memory from accumulator.
**/
void Interpreter::DoSUB(string addr, string target) {
#ifdef EBUG
  Utils::log_stream << "enter DoSUB" << endl;
#endif
  Utils::log_stream << "EXECUTE:    OPCODE ADDR TARGET " << "SUB        "
                    << addr << " " << target << endl;
  // Get the target location. Using Two's Complement Arithmetic, subtract
  // the data at that location from the accumulator.
  int location = GetTargetLocation(addr, target);
  string address_bits = memory_.at(location).GetAddressBits();
  int to_sub = DABnamespace::BitStringToDec(address_bits);
  accum_ = accum_ - to_sub;

#ifdef EBUG
  Utils::log_stream << "leave DoSUB" << endl;
#endif
}

/***************************************************************************
 * Function 'DoWRT'.
 * This top level function interprets the 'WRT' opcode.
 *
 * Convert the 16-bit accumulator to a 32-bit 2s complement value.
 * Write that value to standard output.
 *
 * Note that we actually write more than just the value itself so we can do
 * better tracing. This could/should be fixed in a final version of this code.
**/
void Interpreter::DoWRT(ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter DoWRT" << endl;
#endif
  Utils::log_stream << "EXECUTE:    OPCODE             " << "WRT" << endl;
  // This is what controls the output file. Write the accumulator as a 32 bit
  // 2s complement value.
  int accum_to_write = TwosComplementInteger(accum_);
  out_stream << "WRITE OUTPUT" << "      " << accum_to_write << " "
             << DABnamespace::DecToBitString(accum_, 16) << endl;

#ifdef EBUG
  Utils::log_stream << "leave DoWRT" << endl;
#endif
}

/***************************************************************************
 * Function 'DumpProgram'.
 * This top level function dumps the ASCII of the machine code from memory.
 *
 * Parameters:
 *   out_stream - the scanner to read for source code
**/
void Interpreter::DumpProgram(ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter DumpProgram" << endl;
#endif
  // This loop prints all of the onememoryword objects to the log stream.
  for (unsigned int i = 0; i < memory_.size(); ++i) {
    Utils:: log_stream << "WRITE OUTPUT" << Utils::Format(accum_, 8)
       << " " << Utils::Format(TwosComplementInteger(accum_)) << endl;
  }

#ifdef EBUG
  Utils::log_stream << "leave DumpProgram" << endl;
#endif
}

/***************************************************************************
 * Function 'Execute'.
 * This top level function executes the code.
 *
 * Execution is basically a switch statement based on the opcode value.
 *
 * Parameters:
 *   this_word - the instance of 'OneMemoryWord' to be executed.
 *   data_scanner - the 'Scanner', needed for the 'RD' instruction
 *   out_stream - the output stream , needed for the 'WRT' instruction
**/
void Interpreter::Execute(OneMemoryWord this_word, Scanner& data_scanner,
                          ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter Execute" << endl;
#endif
  Utils::log_stream << ToString() << endl;
  // Get opcode, direct/indirect flag (0/1), and address bits from the
  // OneMemoryWord object (a bit string). Using the opcode, look up the
  // respective command, then call the respective command's function.
  string opcode = this_word.GetMnemonicBits();
  string address = this_word.GetAddressBits();
  string is_direct = this_word.GetIndirectFlag();
  assert(opcode.length() == 3);
  // Find the command to execute using the first three bits of this_word.
  // Combinations registered in the code_to_mnemonic_ class map.
  string cmd = code_to_mnemonic_.find(opcode) -> second;
  if (cmd == "EEE") {  // "EEE" indictates a RD/WRT/STP.
    // The last three address bits will delinate the commmand.
    string last_three = this_word.GetAddressBits().substr(9);
    if (last_three == "010") DoSTP();
    else if (last_three == "001") DoRD(data_scanner);
    else if (last_three == "011") DoWRT(out_stream);
  } else if (cmd == "BAN") {
    DoBAN(is_direct, address);
  } else if (cmd == "SUB") {
    DoSUB(is_direct, address);
  } else if (cmd == "STC") {
    DoSTC(is_direct, address);
  } else if (cmd == "AND") {
    DoAND(is_direct, address);
  } else if (cmd == "ADD") {
    DoADD(is_direct, address);
  } else if (cmd == "LD ") {
    DoLD(is_direct, address);
  } else if (cmd == "BR ") {
    DoBR(is_direct, address);
  }

#ifdef EBUG
  Utils::log_stream << "leave Execute" << endl << endl;
#endif
}

/***************************************************************************
 * Function 'FlagAddressOutOfBounds'.
 * Check to see if an address is between 0 and 'kMaxMemory' inclusive and
 * die with an error if this isn't the case.
 *
 * Parameter:
 *   address - the address to check for out of bounds
**/
void Interpreter::FlagAddressOutOfBounds(int address) {
#ifdef EBUG
  Utils::log_stream << "enter FlagAddressOutOfBounds" << endl;
#endif
  // Mark the address as outside of memory if the requested address is too
  // large.
  if (!(address > 0 && address <= DABnamespace::kMaxMemory)) {
    Utils::log_stream << "The address was out of bounds" << endl;
    exit(1);
  }
#ifdef EBUG
  Utils::log_stream << "leave FlagAddressOutOfBounds" << endl;
#endif
}

/***************************************************************************
 * Function 'GetTargetLocation'.
 * Get the target location, perhaps through indirect addressing.
 *
 * Note that this function crashes the program if the target location is out
 * of bounds for this simulated computer.
 *
 * Parameter:
 *   label - the label for our tracing output (debugging purposes)
 *   addr - is this indirect or not?
 *   target - the target to look up
**/
int Interpreter::GetTargetLocation(string addr, string target) {
#ifdef EBUG
  Utils::log_stream << "enter GetTargetLocation" << endl;
#endif
  /* If fourth bit (addr) is 0, then there is direct addressing. Therefore,
   * convert target to decimal, and that is the target location. If addr is 
   * 1, then there is indirect addressing. Therefore, convert the target to 
   * decimal, then go to that location, and get that location's target data 
   * to convert.
  */
  int location = 0;
  int converted_value = DABnamespace::BitStringToDec(target);
  assert(addr == "0" || addr == "1");
  if (addr == "0" && converted_value <= pc_) {
    location = converted_value;
  } else if (addr == "1" && converted_value <= pc_) {
    string memory_bits = memory_.at(converted_value).GetAddressBits();
    int memory_decimal = DABnamespace::BitStringToDec(memory_bits);
    location = memory_decimal;
    }

#ifdef EBUG
  Utils::log_stream << "leave GetTargetLocation" << endl;
#endif

  return location;
}

/***************************************************************************
 * Function 'Interpret'.
 * This top level function interprets the code.
 *
 * Note that errors in execution will result in the program being terminated
 * from functions other than this one.
 *
 * We run a loop until we either hit the bogus PC value for the STP or we
 * encounter an error, which can include having the PC go past 4095.
 * So the interpreter runs a logically infinite loop:
 *
 * while true (execution should stop with a 'STP' instruction)
 *   decode the instruction pointed to by the PC
 *   execute the instruction
 *   check for invalid PC or infinite loop
**/
void Interpreter::Interpret(Scanner& data_scanner, ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter Interpret" << endl;
#endif

  // Run a loop to control the hardware. This loop will call Execute() to
  // decode the needed bits and run further instruction.
  bool is_true = true;
  pc_ = 0;
  while (is_true) {
    if (pc_ < memory_.size()) {
      if (pc_ > DABnamespace::kMaxMemory) {
        Utils::log_stream << "crashing. pc too big" << endl;
        exit(1);
      }
      Execute(memory_.at(pc_), data_scanner, out_stream);
      ++pc_;
    } else {
    is_true = false;
    }
  }

#ifdef EBUG
  Utils::log_stream << "leave Interpret" << endl;
#endif
}

/***************************************************************************
 * Function 'ReadProgram'.
 * This top level function reads the ASCII of the machine code, one line
 * at a time, uses the input to create an instance of 'OneMemoryWord', and
 * stores that instance into memory.
 *
 * Parameters:
 *   in_scanner - the scanner to read for source code
**/
void Interpreter::ReadProgram(Scanner& in_scanner) {
#ifdef EBUG
  Utils::log_stream << "enter ReadProgram" << endl;
#endif

  accum_ = 0;
  pc_ = 0;

  // Read the lines of the ASCII version of the executable and put the
  // ASCII into a 'vector' of 'OneMemoryWord' instances.
  int linesub = 0;
  while (in_scanner.HasNext()) {
    string line = in_scanner.NextLine();
    OneMemoryWord one_word = OneMemoryWord(line);
    memory_.push_back(one_word);
    ++linesub;
    ++pc_;
    Utils::log_stream << "READ " << linesub << " " << pc_ << " "
                      << line << endl;
  }

  Utils::log_stream << this->ToString() << endl;

#ifdef EBUG
  Utils::log_stream << "leave ReadProgram" << endl;
#endif
}

/***************************************************************************
 * Function 'ToString'.
 *
 * This outputs a prettyprinted string:
 *   PC, the program counter
 *   ACC, the accumulator, as an integer and as a bitstring
 *   a dump of memory from 0 through the max memory for this program
 *
 * Returns:
 *   the prettyprint string for printing
**/
string Interpreter::ToString() {
#ifdef EBUG
  Utils::log_stream << "enter ToString" << endl;
#endif

  string stars40 = "********* ********* ********* ********* ";
  string sss = "\n" + stars40 + stars40 + "\n";
  sss += "MACHINE IS NOW\n";

  sss += "PC    " + Utils::Format(pc_, 8) + "\n";

  int twoscomplement = this->TwosComplementInteger(accum_);
  sss += "ACCUM " + Utils::Format(twoscomplement, 8)
                  + " " + DABnamespace::DecToBitString(accum_, 16)
                  + "\n\n";

  int memorysize = memory_.size();
  for (int outersub = 0; outersub < memorysize; outersub += 4) {
    sss += "MEM " + Utils::Format(outersub, 4)
                + "-"
                + Utils::Format(outersub+3, 4);
    for (int innersub = outersub; innersub < outersub + 4; ++innersub) {
      if (innersub < memorysize) {
        sss += " " + memory_.at(innersub).GetBitPattern();
      }
    }
    sss += "\n";
  }
  sss += "\n" + stars40 + stars40;

#ifdef EBUG
  Utils::log_stream << "leave ToString" << endl;
#endif

  return sss;
}

/***************************************************************************
 * Function 'TwosComplementInteger'.
 *
 * This converts a 16 bit integer into the plus or minus 15 bit integer
 * that is the integer of the 2s complement bit pattern.
 *
 * Parameter:
 *   what - the integer value to convert to the integer that is the 2s compl
 *
 * Returns:
 *   the converted value
**/
int Interpreter::TwosComplementInteger(int what) {
#ifdef EBUG
  Utils::log_stream << "enter TwosComplementInteger" << endl;
#endif

  int twoscomplement = (what > 32768) ? what - 65536 : what;

#ifdef EBUG
  Utils::log_stream << "leave TwosComplementInteger" << endl;
#endif

  return twoscomplement;
}
