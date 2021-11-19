// Abraham Meza //
// CS 457 - Project 1 //
// September 29, 2021 //

// This program creates a Database Management System (DMS), allowing a user to create, use, modify, and delete databases/tables for the purpose of mirroring sqlite functionalities and metadata organization. The program has been coded with a variety of fail-safes to prevent runtime errors from user-input. //

// Header File(s) //

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

// Linux Header File(s) //

#include <algorithm>
#include <string>
#include <climits>

// Namespace Declaration(s) //

using namespace std;

// Function Prototype(s) //

// Database Functions (Creation, Deletion, Access) //

void createDatabase(string initialPath, string dataBase); // createDatabase(): creates a database by initializing a directory using specified database name (3rd arg). By passing the root/parent path, the function ensures that users can only create a database while in the root/parent. If they are not, then the function will place them at the root before creating a new database. The function prevents a user from creating an existing database (no overwrite).
void deleteDatabase(string initialPath, string dataBase); // deleteDatabase(): deletes a database by recursively deleteing a database using system calls, removing the specified database name (3rd arg). By passing the root/parent path, the function ensures that users can only delete a database while in the root/parent. If they are not, then the function places them at the root before delete an existing database. The function prevents the user from deleting a database that doesn't exist.
void useDatabase(string initialPath, string dataBase); // useDatabase(): allows a user to access a specific database (2nd arg) to create, use, modify, and delete tables containing metadata. By passing the root/parent path, the function ensures that users can only access databases while in the root/parent. If they are not, then the function will place them at the root before accessing a new database. The function prevents a user from accessing a database that doesn't exist.

// Table Functions (Creation, Deletion, Update, Query) //

void createTable(string tableName, vector<string> attributesName, vector<string> attributesType); // createTable(): creates a table by exporting a .txt file with the table name, that contains specified attribute names/types in a tabular format. The function ensures that users are unable to create tables outside a database. The function prevents users from creating an existing table (no overwrite).
void deleteTable(string tableName); // deleteTable(): deletes a table by removing the file with the specified table name (deallocated properly). The function prevents a user from deleting a table outside the database. The function prevents users from removing a table/file that doesn't exist.
void queryTable(string query, string tableName); // queryTable(): fetches table information by reading the specified file/table name, using the query to filter out unnecessary information. The function prevents users from fetching information outside a database. The function prevents users from fetching information from a table/file that doesn't exist.
void updateTable(string alter, string tableName, vector<string> attributesName, vector<string> attributesType); // updateTable(): modifies an existing table by appending attribute name/type information to the target file. The function prevents users from modifying an existing table outside a database. The function prevents users from modifying a table/file that doesn't exist.

// Main Program //

int main() {

    // Variable Declaration / Initialization //

    int found; // helper-variable: used to record the position of a substring from str.find(); solution to prevent different signedness comparison issues (better than type-casting)

    bool error = false; // boolean variable: fail-safe helper (record error status)

    char delimiter(';'); // delimiter variable: used for getline() delimiter (stop reading when reach ";")
    char cwdHome[PATH_MAX]; // buffer variable: used for getcwd() to reflect home/root directory

    string command; // variable: used to take user-input database command input
    string nameStructure; // variable: used to record database | table label (provided by command var.)
    string keyword; // variable: used to store parsed keyword of user-input command (vector[0]): CASE SENSITIVE ISSUES
    string structure; // variable: used to store parsed stucture of user-input command (vector[1]): CASE SENSITIVE ISSUES
    string indication; // variable: used to store parsed target of user-input command (vector[2]): CASE SENSITIVE ISSUES
    string action; // variable used to store parsed alter option of user-input comman (vector[3]): CASE SENSITIVE ISSUES
    string query; // variable used to store parsed query option of user-input command (vector[1])
    string rootDirectory = getcwd(cwdHome, sizeof(cwdHome)); // comparison variable: record current home directory for future comparisons (i.e., changing between directories, etc.)

    const char * databaseDirectory = rootDirectory.c_str(); // variable: used to check if current directory is root/parent directory; strcmp() only takes in char * inputs

    vector<string> words; // vector instance: used to store parsed string to identify keyword, stucture, and label
    vector<string> attributeName; // vector instance: used to store parsed string to identify attributes names for "TABLE"
    vector<string> attributeType; // vector instance: used to store parsed string to identify attribute types for "TABLE"

    cout << "\n" <<
            "/////////////////////////////\n" <<
            "/////  Database Program /////\n" <<
            "/////////////////////////////" << endl << endl; // test label design */


    while(keyword != ".EXIT") { // while loop: user-interface menu (database/table options) - continue to display menu until user exits the program

        // Cin User Input //
        
        getline(cin, command, delimiter); // read user input and store value into option

        cin.ignore(); // ignore new-line character input: clear cin buffer (prevents accidental new line from being seen as the keyword (first word))
        command.erase(remove(command.begin(), command.end(), '\n'), command.end()); // str.erase(): remove all new lines from user-input string (prevents '/n' in commands)
        command.erase(remove(command.begin(), command.end(), '\r'), command.end()); // str.erase(): remove all return characters from user-input string (prevents '/r' in commands): solves issue in where empty lines inside files were treated as command characters

        found = command.find(".EXIT"); // ".EXIT" does not use ';' as a delimiter; therefore, program needs to parse ".EXIT" to determine if the program should end (discard anything that follows ".EXIT")

        if(found != -1) { // if statement: if ".EXIT" is a substring of the user-input, parse ".EXIT"
            command = (command.substr(found , 5)).c_str(); // re-assign original command string to reflect ".EXIT"
        }

        if(command.front() != '-' && !command.empty()) { // if statement: if command begins with '-' (comments) or isEmpty(), ignore and getline() once again (do not proceed with program)
            stringstream ss(command); // stringstream classs ss: string command treated as stream (parsing -> extraction)
            string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (words)

            while(getline(ss, temp, ' ')){ // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                if(temp.front() == '(') { // if statememt: if string begins with a '(' (signifies start of parameter), remove '(' for clean parsing
                    temp.erase(temp.find_first_of('('), 1); // str.erase(): remove '(' from string
                }

                if(temp.back() == ')') { // if statement: if string ends with a ')' (signifies end of parameter), remove ')' for clean parsing
                    temp.erase(temp.find_last_of(')'), 1); // str.erase(): remove ')' from string
                }

                words.push_back(temp); // push_back(): push current string value into vector (list of words)
            }

            keyword = words.at(0); // variable: record command keyword (first element of the vector -> word[0])
            transform(keyword.begin(), keyword.end(), keyword.begin(), ::toupper); // transform(..., ::toupper): var. keyword is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues                

            // DECISION TREE BASED ON WORD PARSING //

            if(keyword == "CREATE") { // if statement: if the keyword is create, navigate decision tree to determine structure and label, then apply corresponding algorithm (i.e., create -> database > database name, create -> table -> table features)
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    structure = words.at(1); // variable: record command structure (second element of the vector -> vector[1])
                    transform(structure.begin(), structure.end(), structure.begin(), ::toupper); // transform(..., ::toupper): var. structure is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues

                    nameStructure = words.at(2); // variable: record command structure (database | table) name (third element of the vector -> vector[2])// variable: record command structure (database | table) name (third element of the vector -> vector[2])
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }
        
                if(error != true) {  // if statement: if no error occured (parameter read was successful), attempt to create a new file/table
                    if(structure == "DATABASE") { // if statement: if the structure is a database, conduct database creation algorithm
                        createDatabase(rootDirectory, nameStructure); // createDatabase(): create new database / directory
                    }
                    else if(structure == "TABLE") { // else if statement: if the structure is a table, conduct table creation algorithm
                        try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                            for(size_t i = 3; i < words.size(); i += 2) { // for loop: if structure was a "TABLE", record elements > 3 as parameter variables for "TABLE" (e.g., attributeName and attributeType)
                                attributeName.push_back(words.at(i)); // push_back(): push current string value into vector (list of attribute names)

                                found = words.at(i).find(','); // variable: stores position of substring ',' if found

                                if(found != -1) { // if statement: if attributeName (first string of first parameter entry) contains a ',', then next string is not an attributeType (is another attributeName) -> ERROR = TRUE -> failsafe
                                    error = true; // bool: ERROR = TRUE -> failsafe
                                }
                                
                                attributeType.push_back(words.at(i + 1)); // push_back(): push current string value into vector (list of attribute types)
                            }
                        }
                        catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                            error = true; // bool: ERROR = TRUE -> failsafe
                        }

                        for(size_t i = 0; i < attributeType.size(); i++) { // for loop: used to traverse vector attributeType elements: remove ',' characters
                            found = attributeType.at(i).find(','); // variable: stores position of substring ',' if found

                            if(found != -1) { // if statement: if string contains ',' (signifies var. seperation in parameter), remove ',' for clean parsing
                                attributeType.at(i).erase(attributeType.at(i).find(','), 1); // str.erase(): remove ',' from string
                            }
                        }

                        if(error != true) { // if statement: if no error occured (parameter read was successful), create a file/table
                            if(!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot create a table (prompt error message)
                                cerr << "ERROR: Table " << nameStructure << " cannot be created outside a database." << endl; // error message: cannot make a table without a database
                            }
                            else { // else statement: else, if inside a non-root directory (a.k.a. inside a database directory), allow user to create file/table
                                createTable(nameStructure, attributeName, attributeType); // createTable(): create new file/table
                            }
                        }
                        else { // else statement: if error occured (parameter read not successful), prompt error message (do not create table)
                            cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name (attr.name attr.type, ...)]" << endl; // error message (unstable parameters)
                        }
                    }
                    else { // else statement: if structure is not recognized, then output an error code to the user
                        if(!structure.empty()) { // if structure is not recognized && structure is not empty ("" | " "), then prompt error code (structure not recognized)
                            cerr << "ERROR: Structure \"" << structure << "\" not recognized! Database cannot be created. Please try again!" << endl; // error message: user structure parameter is not recognized (!DATABASE | !TABLE) 
                        }
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do not attempt to alter an existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name]" << endl; // error message (unstable parameters)
                }
            }
            else if(keyword == "DROP") { // else if statement: if the keyword is delete, navigate decision tree to determine structure and label, then apply corresponding algorithm (i.e., delete -> database > database name, delete -> table -> table features)
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    structure = words.at(1); // variable: record command structure (second element of the vector -> vector[1])
                    transform(structure.begin(), structure.end(), structure.begin(), ::toupper); // transform(..., ::toupper): var. structure is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues

                    nameStructure = words.at(2); // variable: record command structure (database | table) name (third element of the vector -> vector[2])
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }

                if(error != true) { // if statement: if no error occured (parameter read was successful), delete an existing file/table

                    if(structure == "DATABASE") { // if statement: if the structure is a database, conduct database deletion algorithm
                        deleteDatabase(rootDirectory, nameStructure); // deleteDatabase(): delete existing database / directory
                    }
                    else if(structure == "TABLE") { // else if statement: if the structure is a table, conduct table deletion algorithm
                        if(!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot drop a table (prompt error message)
                            cerr << "ERROR: Table " << nameStructure << " cannot be deleted outside a database." << endl; // error message: cannot search a table without a database
                        }
                        else { // else statement: if not inside of root directory (inside database), delete target file/table
                            deleteTable(nameStructure); // deleteTable(): delete exisiting database/directory
                        }
                    }
                    else{  // else statement: if structure is not recognized, then output an error code to the user
                        cerr << "ERROR: Structure \"" << structure << "\" not recognized! Database cannot be deleted. Please try again!" << endl; // error message: user structure parameter is not recognized (!DATABASE | !TABLE) 
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do delete an existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name]" << endl; // error message (unstable parameters)
                }
            }
            else if (keyword == "USE") { // else if statement: if the keyword is use, enter corresponding database instance for data usage
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    nameStructure = words.at(1); // record database name from 2nd element in the vector (FORMAT: keyword name)
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }
                
                if(error != true) { // if statement: if no error occured (parameter read was successful), access an existing file/table
                    useDatabase(rootDirectory, nameStructure); // useDatabase(): open existing database / directory
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do not access existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword name]" << endl; // error message (unstable parameters)
                }
            }
            else if (keyword == "SELECT") { // else if statement: if the keyword is use, enter corresponding database instance for data usage
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    query = words.at(1); // record query option from 2nd element in the vector (FORMAT: keyword query target name)

                    indication = words.at(2); // record target from 3rd element in the vector (FORMAT: keyword query target name)
                    transform(indication.begin(), indication.end(), indication.begin(), ::toupper); // transform(..., ::toupper): var. indication is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }

                if(error != true){ // if statement: if no error occured (parameter read was successful), query an existing file/table
                    if(indication == "FROM") { // if statement: if indication == "FROM", attempt to query existing table using user parameters
                        nameStructure = words.at(3); // record table name from 4th element in the vector ((FORMAT: keyword query target name)

                        if(!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot create a table (prompt error message)
                            cerr << "ERROR: Table " << nameStructure << " cannot be searched outside a database." << endl; // error message: cannot search a table without a database
                        }
                        else { // else statement: if not inside of root directory (inside database), query target file/table
                            queryTable(query, nameStructure); // queryTable(): query an existing table
                        }
                    }
                    else { // else statement: if indication string not recognized, prompt error message (cannot query file/table source)
                        cerr << "ERROR: Indication \"" << indication << "\" not recognized. Please try again!" << endl; // error message: user indication parameter is not recognized (!FROM)
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (query existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword query indication name]" << endl; // error message (unstable parameters)
                }
            }
            else if (keyword == "ALTER") { // else if statement: if the keyword is use, enter corresponding database instance for data usage
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    structure = words.at(1); // variable: record command structure (second element of the vector -> vector[1])
                    transform(structure.begin(), structure.end(), structure.begin(), ::toupper); // transform(..., ::toupper): var. structure is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }

                if(error != true) { // if statement: if no error occured (parameter read was successful), attempt to alter an existing file/table
                    if(structure == "TABLE") { // if statement: if structure == "TABLE", attempt to alter/update table using user parameters
                        try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                            nameStructure = words.at(2); // record table name from 3rd element in the vector (FORMAT: keyword structure name action attributeName attributeType)

                            action = words.at(3); // record alter instruction from 4th element in the vector (FORMAT: keyword structure name action attributeName attributeType)
                            transform(action.begin(), action.end(), action.begin(), ::toupper); // transform(..., ::toupper): var. action is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues

                            for(size_t i = 4; i < words.size(); i += 2) { // for loop: if structure was a "TABLE", record elements > 4 as parameter variables for "TABLE" (e.g., attributeName and attributeType)
                                attributeName.push_back(words.at(i)); // push_back(): push current string value into vector (list of attribute names)

                                found = words.at(i).find(','); // variable: stores position of substring ',' if found

                                if(found != -1) { // if statement: if attributeName (first string of first parameter entry) contains a ',', then next string is not an attributeType (is another attributeName) -> ERROR = TRUE -> failsafe
                                    error = true; // bool: ERROR = TRUE -> failsafe
                                }
                                
                                attributeType.push_back(words.at(i + 1)); // push_back(): push current string value into vector (list of attribute types)
                            }
                        }
                        catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                            error = true; // bool: ERROR = TRUE -> failsafe
                        }

                        for(size_t i = 0; i < attributeType.size(); i++) { // for loop: used to traverse vector attributeType elements: remove ',' characters
                            found = attributeType.at(i).find(','); // variable: stores position of substring ',' if found

                            if(found != -1) { // if statement: if string contains ',' (signifies var. seperation in parameter), remove ',' for clean parsing
                                attributeType.at(i).erase(attributeType.at(i).find(','), 1); // str.erase(): remove ',' from string
                            }
                        }

                        if(error != true) { // if statement: if no error occured (parameter read was successful), create a file/table
                            if(!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot create a table (prompt error message)
                                cerr << "ERROR: Table " << nameStructure << " cannot be altered outside a database." << endl; // error message: alter a table without a database
                            }
                            else { // else statement: else, if inside a non-root directory (a.k.a. inside a database directory), allow user to create file/table
                                updateTable(action, nameStructure, attributeName, attributeType); // updateTable(): update existing file/table
                            }
                        }
                        else { // else statement: if error occured (parameter read not successful), prompt error message (do not create table)
                            cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name action (attr.name attr.type, ...)]" << endl; // error message (unstable parameters)
                        }
                    }
                    else { // else statement: if structure is not recognized, then output an error code to the user
                        if(!structure.empty()) { // if structure is not recognized && structure is not empty ("" | " "), then prompt error code (structure not recognized)
                            cerr << "ERROR: Structure \"" << structure << "\" not recognized! Database cannot be deleted. Please try again!" << endl; // error message: user structure parameter is not recognized (!TABLE) 
                        }
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do not attempt to alter an existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name action (attr.name attr.type, ...)]" << endl; // error message (unstable parameters)
                }
            }
            else { // else statement: if keyword is not recognized, then output an error code to the user
                if(keyword != ".EXIT") { // if statement: if keyword is not recognized && if keyword is not ".EXIT (end program)", then output error code to user.
                    cerr << "ERROR: Command not recognized! Please try again!" << endl; // error message: user command does not match available database commands
                }
            }
            
            // RESET VARIABLES BEFORE NEW READ //

            error = false; // reset boolean value for new error retrieval (fail-safe)

            command.clear(); // reset variable value for new command retrieval 
            temp.clear(); // reset variable value for new parsing retrieval
            words.clear(); // reset vector instance for new parsing retrieval
            attributeName.clear(); // reset vector instance for new parsing retrieval
            attributeType.clear(); // reset vector instance for new parsing retrival
            ss.str(string()); // reset stream for new parsing retrieval
        }
    }

    cout << "All done." << endl;

    return 0; // end program
}

// Function Prototype Definitions //

void createDatabase(string initialPath, string dataBase) { // createDatabase(): create new database/directory
    int check; // placeholder variable: used to determine if execution was sucessful (make-shift boolean)

    char cwdCurr[PATH_MAX]; // buffer variable: used for getcwd() to reflect current directory

    // convert parameter string data to const char * for comparisons (i.e., strcmp(), chdir(), getcwd(), etc.)

    const char * homeDirectory = initialPath.c_str();
    const char * directoryName = dataBase.c_str();

    if(strcmp(homeDirectory, getcwd(cwdCurr, sizeof(cwdCurr)))) { // if statement: if inside a database (a non-root directory), change directory back to the root to create a new database (cannot have database within a database)
        chdir(homeDirectory); // change back to root/home directory
    }

    check = mkdir(directoryName, 0777); // create specified database (directory), and return value (if -1, then directory could not be created)

    if (check == -1) { // if statement: if directory cannot be created
       cerr << "!Failed to create database " << dataBase << " because it already exists." << endl; // error message: directory already exists
    }
    else { // else statement: if directory created successfully
        cout << "Database " << dataBase << " created." << endl; // cout confirmation (success)
    }
}

void deleteDatabase(string initialPath, string dataBase) { // deleteDatabase(): delete existing database/directory
    int check; // placeholder variable: used to determine if execution was sucessful (make-shift boolean)

    char cwdCurr[PATH_MAX]; // buffer variable: used for getcwd() to reflect current directory

    string systemCall = "rm -r " + dataBase + " 2> /dev/null"; // variable: used to store systemCall command (remove directory/database recursively) - added error message redirect to NULL space for proper output (no system() error message)

    // convert parameter string data to const char * for comparisons (i.e., strcmp(), chdir(), getcwd(), etc.)

    const char * homeDirectory = initialPath.c_str();
    const char * execute = systemCall.c_str();

    if(strcmp(homeDirectory, getcwd(cwdCurr, sizeof(cwdCurr)))) { // if statement: if inside a database (a non-root directory), change directory back to the root to remove an existing database (cannot delete a database within a database)
        chdir(homeDirectory); // change back to root/home directory
    }

    check = system(execute); // remove specified database (directory), and return value (if check != 0, then directory could not be removed)

    if(check != 0) { // if statement: if check != 0 -> database (directory) cannot be removed (e.g., does not exist) 
        cerr << "!Failed to delete " << dataBase << " because it does not exist." << endl; // error message: directory does not exist
    }
    else { // else statement: if check == 0 -> database (directory) removed successfully
        cout << "Database " << dataBase << " deleted." << endl; // cout confirmation (success)
    }
}

void useDatabase(string initialPath, string dataBase) { // useDatabase(): access existing database/directory
    int check; // placeholder variable: used to determine if execution was sucessful (make-shift boolean)

    char cwdCurr[PATH_MAX]; // buffer variable: used for getcwd() to reflect current directory

    // convert parameter string data to const char * for comparisons (i.e., strcmp(), chdir(), getcwd(), etc.)

    const char * homeDirectory = initialPath.c_str();
    const char * directoryName = dataBase.c_str();

    if(strcmp(homeDirectory, getcwd(cwdCurr, sizeof(cwdCurr)))) { // if statement: if inside a database (a non-root directory), change directory back to the root to access a new database (switch between databases)
        chdir(homeDirectory); // change back to root/home directory
    }

    check = chdir(directoryName); // change to specified database (directory), and return value (if -1, then directory could not be created)

    if (check == -1) { // if statement: if directory cannot be created
        cerr << "!Failed to access database " << dataBase << " because it does not exist." << endl; // error message: directory already exists
    }
    else { // else statement: if directory created successfully
        cout << "Using database " << dataBase << "." << endl; // cout confirmation (success)

        // IGNORE FOLLOWING CODE: Used for testing to see if database was accessed properly //
        // cout << "Current Path: " << getcwd(cwdCurr, sizeof(cwdCurr)) << endl; // cout confirmation: current path/database
    }
}

void createTable(string tableName, vector<string> attributesName, vector<string> attributesType) { // createTable(): create a new table/file
    string fileName = tableName + (".txt"); // variable: used to record full name of file/table for creation algorithm

    fstream file(fileName); // fstream initialization

    if(!file) { // if statement: if file/table does not exist, create a new table
        file.open(fileName, ios::out); // file.open(): attempt to open target file in write-only

        for(size_t i = 0; i < attributesName.size(); i ++) { // for loop: create a table using attributes (seperate section for each attribute)
            if(i == attributesName.size() - 1) { // if statement: if there is only 1 table attribute, export only 1 attribute section
                file << attributesName[i] << " " << attributesType[i]; // export table attribute (single)
            }
            else { // else statement: else if there are multiple table attributes, export all attributes seperated by '|'
                file << attributesName[i] << " " << attributesType[i] << setw(5) << "|" << setw(6); // export table attributes (multiple)
            }
        }

        cout << "Table " << tableName << " created." << endl; // cout confirmation (success)
    }
    else { // else statement: else if file/table does exist, do not create a new table (prompt error message)
        cerr << "!Failed to create table " << tableName << " because it already exists." << endl; // error message (file already exists)
    }

    file.close(); // file.close(): close file from write-only
}

void deleteTable(string tableName) { // deleteTable(): delete existing table/file
    string fileName = tableName + (".txt"); // variable: used to record full name of file/table for deletion algorithm

    const char * targetFile = fileName.c_str(); // convert parameter string data to const char*; needed var. type for remove()

    fstream file(fileName); // fstream initialization

    if(!file){ // if statement: if file/table does not exist, do not delete an existing table (prompt error message)
        cerr << "!Failed to delete "<< tableName << " because it does not exist." << endl; // error message (file already exists)
    }
    else { // else statement: else if file/table does exist, delete existing table
        remove(targetFile); // remove(): deletes file 

        cout << "Table " << tableName << " deleted." << endl; // cout confirmation (success)
    }
}

void queryTable(string query, string tableName) { // queryTable(): fetch information from an existing table/file
    string line; // variable: used to fetch file data
    string fileName = tableName + (".txt"); // variable: used to record full name of file/table for creation algorithm

    ifstream file; // fstream initialization

    file.open(fileName, ios::in); // file.open(): attempt to open target file in read-only

    if(file) { // if statement: if file/table exists, fetch query

        if(query.compare("*") == 0) { // if statement: query == "*" (all info) -> print all table information to the user
            while(getline(file, line)){ // while loop: while extracting all file characters, print to user
                cout << line; // cout table query
            }

            cout << endl;
        }
        else { // else statement: if query not recognized, then output an error code to the user
            cerr << "ERROR: Unrecognized query! Please try again!" << endl; // error message (query does not exist)
        }
    }
    else { // else statement: else if file/table does not exist, do not fetch a query (prompt error message)
        cerr << "!Failed to query table " << tableName << " because it does not exist." << endl; // error message (file does not exist)
    }

    file.close(); // file.close(): close file from read-only
}

void updateTable(string alter, string tableName, vector<string> attributesName, vector<string> attributesType) { // updateTable(): modify an existing table/file
    string fileName = tableName + (".txt"); // variable: used to record full name of file/table for creation algorithm

    ofstream file; // fstream initialization

    file.open(fileName, ofstream::in); // file.open(): attempt to open target file in read-only (does it exist?)

    if(file) { // if statement: if file/table exists, fetch alter instruction

        file.close(); // file.close(): close file from read-only

        file.open(fileName, ofstream::app); // file.open(): attempt to open target file in append-only 

        if(alter.compare("ADD") == 0) { // if statement: if alter instruction == "ADD" -> append new attributeName/attributeType to file/table

            for(size_t i = 0; i < attributesName.size(); i ++) { // for loop: append requested attributes (seperate section for each additional attribute)
                file << setw(5) << " |" << setw(6) << attributesName[i] << " " << attributesType[i]; // export table attributes (multiple)
            }

            cout << "Table " << tableName << " modified." << endl; // cout confirmation (success)
        }
        else { // else statement: if alter instruction not recognized, then output an error code to the user
            cerr << "ERROR: Unrecognized alter instruction! Please try again!" << endl; // error message (alter does not exist)
        }
    }
    else { // else statement: else if file/table does not exist, do not alter an existing table (prompt error message)
        cerr << "!Failed to modify " << tableName << " because it does not exist." << endl; // error message (file does not exist)
    }

    file.close(); // file.close(): close file from append-only
}
