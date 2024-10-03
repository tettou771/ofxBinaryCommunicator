#pragma once
#include "ofxBinaryCommunicatorTopicStructMaker.h"

TOPIC_STRUCT_MAKER(OscLikeMessage, 250,
    static const int MAX_ARGS = 32;      // Maximum number of arguments
    static const int ADDRESS_SIZE = 32;  // Size of the address string
    static const int TYPESIZE = 32;      // Size of the type string

    char address[ADDRESS_SIZE];          // Message address
    char typestr[TYPESIZE];              // Type string indicating argument types

    // Color type
    struct Color{
        uint8_t r, g, b, a;
    };

    union {
        float f[MAX_ARGS];                // float arguments
        int32_t i[MAX_ARGS];              // int32_t arguments
        uint32_t ui[MAX_ARGS];            // uint32_t arguments
        char C[MAX_ARGS][4];              // 4-character string arguments
        Color color[MAX_ARGS];
    };

    // Enum representing the type of each argument
    enum ArgType {
        OSCLIKE_TYPE_NONE,
        OSCLIKE_TYPE_BOOL,
        OSCLIKE_TYPE_INT32,
        OSCLIKE_TYPE_UINT32,
        OSCLIKE_TYPE_FLOAT,
        OSCLIKE_TYPE_CHAR,
        OSCLIKE_TYPE_CHAR4,
        OSCLIKE_TYPE_COLOR
    };
    
    // Constructor: Initializes the message
    OscLikeMessage(){
        clear();
    }

    // Clears the message
    void clear(){
        memset(address, 0, sizeof(address));
        memset(typestr, 0, sizeof(typestr));
        memset(f, 0, sizeof(f));
    }

    // Retrieves the number of arguments
    int getNumArgs() const {
        int count = 0;
        for(int idx = 0; idx < MAX_ARGS; idx++) {
            if(typestr[idx] != '\0') count++;
        }
        return count;
    }

    // Adds arguments
    bool addInt32Arg(int32_t value){
        int idx = getNumArgs();
        if(idx < MAX_ARGS){
            i[idx] = value;
            typestr[idx] = 'i';
            return true;
        }
        return false; // No available slot
    }

    bool addUint32Arg(uint32_t value){
        int idx = getNumArgs();
        if(idx < MAX_ARGS){
            ui[idx] = value;
            typestr[idx] = 'I';
            return true;
        }
        return false;
    }

    bool addFloatArg(float value){
        int idx = getNumArgs();
        if(idx < MAX_ARGS){
            f[idx] = value;
            typestr[idx] = 'f';
            return true;
        }
        return false;
    }

    bool addCharArg(char value){
        int idx = getNumArgs();
        if(idx < MAX_ARGS){
            C[idx][0] = value;
            C[idx][1] = C[idx][2] = C[idx][3] = 0;
            typestr[idx] = 'c';
            return true;
        }
        return false;
    }

    bool addChar4Arg(const char value[4]){
        int idx = getNumArgs();
        if(idx < MAX_ARGS){
            memcpy(C[idx], value, 4);
            typestr[idx] = 'C';
            return true;
        }
        return false;
    }

    bool addColorArg(const Color& value){
        int idx = getNumArgs();
        if(idx < MAX_ARGS){
            color[idx] = value;
            typestr[idx] = 'r'; // 'r' denotes Color type
            return true;
        }
        return false;
    }

    bool addBoolArg(bool value){
        int idx = getNumArgs();
        if(idx < MAX_ARGS){
            i[idx] = value ? 1 : 0;
            typestr[idx] = value ? 'T' : 'F';
            return true;
        }
        return false;
    }

    /**
     * Getter Methods
     */
    // Retrieves an argument as int32_t
    int32_t getArgAsInt32(int index) const {
        if (index < 0 || index >= MAX_ARGS) return 0;
        if (typestr[index] == 'i') return i[index];
        return 0;
    }

    // Retrieves an argument as uint32_t
    uint32_t getArgAsUint32(int index) const {
        if (index < 0 || index >= MAX_ARGS) return 0;
        if (typestr[index] == 'I') return ui[index];
        return 0;
    }

    // Retrieves an argument as float
    float getArgAsFloat(int index) const {
        if (index < 0 || index >= MAX_ARGS) return 0.0f;
        if (typestr[index] == 'f') return f[index];
        return 0.0f;
    }

    // Retrieves an argument as a single char
    char getArgAsChar(int index) const {
        if (index < 0 || index >= MAX_ARGS) return '\0';
        if (typestr[index] == 'c') return C[index][0];
        return '\0';
    }

    // Retrieves an argument as a 4-character string
    void getArgAsChar4(int index, char out[4]) const {
        if (index < 0 || index >= MAX_ARGS) return;
        if (typestr[index] == 'C') {
            memcpy(out, C[index], 4);
        } else {
            // If not a 4-char string, set output to zeros
            memset(out, 0, 4);
        }
    }

    // Retrieves an argument as Color
    Color getArgAsColor(int index) const {
        if (index < 0 || index >= MAX_ARGS) {
            Color empty = {0, 0, 0, 0};
            return empty;
        }
        if (typestr[index] == 'r') return color[index];
        Color empty = {0, 0, 0, 0};
        return empty;
    }

    // Retrieves an argument as bool
    bool getArgAsBool(int index) const {
        if (index < 0 || index >= MAX_ARGS) return false;
        if (typestr[index] == 'T') return true;
        if (typestr[index] == 'F') return false;
        return false;
    }

    // Retrieves the type of an argument
    ArgType getArgType(int index) const {
        if (index < 0 || index >= MAX_ARGS) return OSCLIKE_TYPE_NONE;
        switch(typestr[index]){
            case 'T':
            case 'F':
                return OSCLIKE_TYPE_BOOL;
            case 'i':
                return OSCLIKE_TYPE_INT32;
            case 'I':
                return OSCLIKE_TYPE_UINT32;
            case 'f':
                return OSCLIKE_TYPE_FLOAT;
            case 'c':
                return OSCLIKE_TYPE_CHAR;
            case 'C':
                return OSCLIKE_TYPE_CHAR4;
            case 'r':
                return OSCLIKE_TYPE_COLOR;
            default:
                return OSCLIKE_TYPE_NONE;
        }
    }

    /**
     * Setter Methods
     */
    // Sets an argument as int32_t
    void setInt32Arg(int32_t value, int index){
        if(index >= 0 && index < MAX_ARGS){
            i[index] = value;
            typestr[index] = 'i';
        }
    }

    // Sets an argument as uint32_t
    void setUint32Arg(uint32_t value, int index){
        if(index >= 0 && index < MAX_ARGS){
            ui[index] = value;
            typestr[index] = 'I';
        }
    }

    // Sets an argument as float
    void setFloatArg(float value, int index){
        if(index >= 0 && index < MAX_ARGS){
            f[index] = value;
            typestr[index] = 'f';
        }
    }

    // Sets an argument as a single char
    void setCharArg(char value, int index){
        if(index >= 0 && index < MAX_ARGS){
            C[index][0] = value;
            // Optionally, clear the remaining bytes
            C[index][1] = C[index][2] = C[index][3] = 0;
            typestr[index] = 'c';
        }
    }

    // Sets an argument as a 4-character string
    void setChar4Arg(const char value[4], int index){
        if(index >= 0 && index < MAX_ARGS){
            memcpy(C[index], value, 4);
            typestr[index] = 'C';
        }
    }

    // Sets an argument as Color
    void setColorArg(const Color& value, int index){
        if(index >= 0 && index < MAX_ARGS){
            color[index] = value;
            typestr[index] = 'r'; // 'r' denotes Color type
        }
    }

    // Sets an argument as bool
    void setBoolArg(bool value, int index){
        if(index >= 0 && index < MAX_ARGS){
            i[index] = value ? 1 : 0;
            typestr[index] = value ? 'T' : 'F';
        }
    }

    // Sets the address using a C-string
    void setAddress(const char* addr){
        strncpy(address, addr, ADDRESS_SIZE);
        address[ADDRESS_SIZE - 1] = '\0'; // Ensure null-termination
    }

#ifdef OF_VERSION_MAJOR
    // Sets the address using a string
    void setAddress(const string& addr){
        strncpy(address, addr.c_str(), ADDRESS_SIZE);
        address[ADDRESS_SIZE - 1] = '\0'; // Ensure null-termination
    }
#endif

    // Retrieves the address
    const char* getAddress() const {
        return address;
    }
                   
    const char* getTypestr() const {
        return typestr;
    }

#ifdef OF_VERSION_MAJOR
    string getAddressString() const {
        return string(address);
    }

    string getTypestrString() const {
        return string(typestr);
    }
#endif

)
