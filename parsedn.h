/*
 * ParseDN interprets ActiveDirectory DN entries
 * Uses:
 *  -Split DN into individual components
 *  -Return individual properties
 *  -Return formatted URLS for DCs
 *
 * (c)2022 Jacob Bogner
 */

#ifndef PARSEDISTINGUISHEDNAME_PARSEDN_H
#define PARSEDISTINGUISHEDNAME_PARSEDN_H

#include <iostream>
#include <utility>
#include <vector>

#define CN 0
#define DN 1
#define DC 2
#define OU 3
#define L 4
#define ST 5
#define O 6
#define C 7
#define STREET 8
#define UID 9
#define URL 1

std::string adCodes[] = {"CN", "DN", "DC", "OU", "L", "ST", "O", "C", "STREET", "UID"};
std::string adCodeEquiv[] = {"CN=", "DN=", "DC=", "OU=", "L=", "ST=", "O=", "C=", "STREET=", "UID="};
struct token{
    std::string typeCode;
    std::string value;
};

struct DNentry{
    std::vector<struct token> distName;
    std::string commonName;
    std::string DCcode;
    std::string formattedURL;
};

struct token buildToken(std::string component){ // Take a component of a DN and break it into a token
    struct token newToken;
    for(int i = 0; i < component.size(); i++){
        if(component[i] == '='){
            newToken.typeCode = component.substr(0, i);
            newToken.value = component.substr(i+1);
        }
    }
    return newToken;
}

std::string getCommonName(const std::vector<struct token>& distName){
    //Take in a DN as a vector of tokens, extract the CN, and return as a string
    std::string commonName;
    for(auto & i : distName){
        if(i.typeCode == adCodes[CN]){
            commonName = i.value;
            //std::cout << "CN of this entry is " << commonName << "\n";
        }
    }
    return commonName;
}

std::string getDC_code(const std::vector<struct token>& distName, int req){
    /*
     * Take in DN as vector of tokens and a request to return either DC format or URL format, extract the complete list
     * of DC codes, and return request as string
    */
    std::string DCcode;
    bool firstPass = true;
    for(auto & i : distName){
        if(i.typeCode == adCodes[DC]){
            if(firstPass){
                firstPass = false;
            } else{
                if(req == DC){
                    DCcode.append(",");
                } else{
                    DCcode.append(".");
                }
            }
            if(req == DC){
                DCcode.append(adCodeEquiv[DC]);

            }
            DCcode.append(i.value);
        }
    }
    /*
    if(req == DC){
        std::cout << "The DC code is " << DCcode << "\n";
    } else{
        std::cout << "The URL is " << DCcode << "\n";
    }
    */
    return DCcode;
}

std::vector<struct token> tokenizeRow(std::string newRow){ //Turn a complete DN string into an array of tokens
    std::vector<struct token> tokenRow;
    std::vector<std::string> data;
    for(int i = 0; i < newRow.size(); i++) {
        if(i > 0){
            if((newRow[i] == ',' && newRow[i-1] != '\\') || newRow[i+1] == '\0'){
                //Split row on ',' but don't split if ',' is in context of '\'. If an '\0' is encountered, this must be the last word.
                std::string newComponent;
                if(newRow[i+1] == '\0'){ //Take whole string if we are at the end of the line
                    newComponent = newRow.substr(0, i+1); //Get the substring to tokenize
                }
                else{ //Only take up to the ',' if there is more left in the string
                    newComponent = newRow.substr(0, i); //Get the substring to tokenize
                }
                //std::cout << "Component string is: " << newComponent << "\n";
                struct token newToken = buildToken(newComponent); //Turn the substring into a token
                tokenRow.push_back(newToken); //Push the token into the array of tokens
                newRow = newRow.substr(i+1); //Trim processed component of row and skip over ','
                i = 0; //Reset i to re-start the loop for our new string
            }
        }
    }
    return tokenRow;
}

struct DNentry createEntry(std::string newRow){
    struct DNentry newEntry;
    newEntry.distName = tokenizeRow(std::move(newRow));
    newEntry.commonName = getCommonName(newEntry.distName);
    newEntry.DCcode = getDC_code(newEntry.distName, DC);
    newEntry.formattedURL = getDC_code(newEntry.distName, URL);
    return newEntry;
}


#endif //PARSEDISTINGUISHEDNAME_PARSEDN_H
