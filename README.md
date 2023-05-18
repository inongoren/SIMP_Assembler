# SIMP_Assembler
An assembler for SIMP procesor (invented processor)

This Assembler is build in C language. 
it translate an Assembly code written for a SIMP processor to a maching language code.

**This is how the Assembler works**:
The Assmebler runs twice on the input file in order to translate it and writes the  translated machine language to an output file called "memin.txt".

**First Procedure**:
In this run the Assembler is looking for the labels in the code (the functions written in assembly,
those the machine needs to know their location to go back to whenever it needs).
The assembler keeps the names and the location of the labels in a labels array (array of a special struct called Label).
In addition, the assembler checks for the maximum (called MAX for the next procedure) between the lines of commands and the .word address (a special command which saves data in a specific address).

**Between Procedures**:
In this procedure the Assembler creates the output text files and writes MAX lines of 8 zeros in a line (The number of bits needed in order to tranlate a line from the input file)

**second Procedure**:
In this run the Assembler go through the input file line by line and checking if the line contain a command in it (and now know that this is a line that needs to be translated).
If so, The Assembler checks if it's a regular command or the special command .word.
  **Regular command (opcode) line** - the translation is made by separating the words in the line by the known structure of the line containing 5 feilds:
  1. Opcode - the command unique number, translated to the machine language by the commands table in the code.
  2. rd - a register, translated to the machine language by the registers table in the code.
  3. rs - a register, translated to the machine language by the registers table in the code.
  4. rt - a register, translated to the machine language by the registers table in the code.
  5. immediate (may contain different information) - 
    a. Label - the Assembler translats the Label to its location saved from the first procedure.
    b. Decimal number - The Assmbler converts it to a Hexadecimal number using dec_to_hex function.
    c. Hexadecimal number - The Assmebler doesnt to any translation.
  In the end of every tranlation, The Assembler writes the tranlated information to the output file in the desired location.
  **.word command** - The translation is made by recognizind the desired address and data and writes the data in the desired location in the output file.
  
  **Functions**:
  
  **writing_func** - 
  Description: gets a line of Assembly code, tranlate it and writes the translated content to the output file.
  input: Pointer to a text line which contain command in it, the command number, Pointer to the Labels array, Pointer to the output file.
  output: none.
  
  **dec_to_hex** - 
  Description: converts a Decimal number to a Hexadecimal numebr (three bits).
  input: decimal number (integer), Pointer to the array which the Hexadecimal number will be saved in.
  output: none
  
   **fill_the_line** - 
  Description: converts a Decimal number to a Hexadecimal numebr (eight bits, special for .word command).
  input: decimal number (integer), Pointer to the array which the Hexadecimal number will be saved in.
  output: none
  
   **hex2Dec** - 
  Description: converts a Hexadecimal numebr to a Decimal number.
  input: string contains the Hexadecimal number.
  output: Decimal number (integer)
  
   **uppercase** - 
  Description: converts a small letters in a string to Capitals.
  input: Pointer to a string.
  output: none.
  
  
  
  
  
  
