#include "../utils.cpp"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <filesystem>
namespace fs =  std::filesystem;
#define IS_TRUE(x) { if (!(x)) {std::cout << __FUNCTION__ << " failed on line " << __LINE__ << std::endl; assert(false);} }
#define IS_FALSE(x) {IS_TRUE(!x);}
#define IS_EQ(x,y, msg) { if((x)!=(y)) {std::cout<<__FUNCTION__<<"failed on line " << __LINE__ << std::endl << msg << std::endl; assert(false);}} 
void doesPathExistTests(){
    IS_TRUE(doesPathExist("../testFiles/file.txt"));
    IS_FALSE(doesPathExist("../testFiles/file1.txt"));

}
void isDirectoryTests(){
    IS_TRUE(isDirectory("../testFiles"));
    IS_FALSE(isDirectory("../testFiles/file.txt"));
    IS_FALSE(isDirectory("../testFiles/file1.txt"));
}

void trimWhitespaceTests(){
    std::vector<std::string> strs;
    strs.push_back("    test123    ");
    strs.push_back("");
    strs.push_back("\n\t\t\t\t");
    strs.push_back("ans");
    std::vector<std::string> ans;
    ans.push_back("test123");
    ans.push_back("");  
    ans.push_back("");
    ans.push_back("ans");
    for (unsigned int i=0; i<strs.size();i++){
        IS_EQ(trimWhitespace(strs.at(i)), ans.at(i), trimWhitespace(strs.at(i)) +  " does not equal " + ans.at(i));
    }
}

void doesEpochBackupExistTests(){
    std::string startPath = "../testFiles/epochBackupFolder/";
    IS_TRUE(doesEpochBackupExists(startPath, 123456));
    IS_TRUE(doesEpochBackupExists(startPath,7890));
    IS_FALSE(doesEpochBackupExists(startPath,1));
}

void createParentFolderIfDoesntExistTests(){
    fs::path startPath = "../testFiles/parentFolderTests/";
    createParentFolderIfDoesntExist(startPath / "nonExistant/file.txt");
    createParentFolderIfDoesntExist(startPath/ "a/b/c/d/e/f/g/h/i/j/k");
    createParentFolderIfDoesntExist(startPath/ "1/2/3/4/5");
    IS_TRUE(doesPathExist(startPath/"a/b/c/d/e/f/g/h/i/j"));
    IS_FALSE(doesPathExist(startPath/"a/b/c/d/e/f/g/h/i/j/k"));
    IS_FALSE(doesPathExist(startPath/"nonExistant/file.txt"));
    IS_TRUE(doesPathExist(startPath/"nonExistant"));

    system((std::string("rm -r ") + (startPath/"nonExistant").string()).c_str());
    system((std::string("rm -r ") + (startPath/"a").string()).c_str());
}

void splitTests(){
    std::vector<std::string> inputs;
    std::vector<std::vector<std::string>> ans;
    std::vector<char> delimiters;
    inputs.push_back("the quick brown fox");
    ans.push_back({"the","quick","brown","fox"});
    delimiters.push_back(' ');
    inputs.push_back("thequickbrownfox");
    ans.push_back({"thequickbrownfox"}); 
    delimiters.push_back(' ');
    inputs.push_back("the quick brown fox");
    ans.push_back({"the quick br", "wn f", "x"});
    delimiters.push_back('o');
    for(unsigned int i=0; i<inputs.size();i++){
        IS_EQ(split(inputs.at(i), delimiters.at(i)),ans.at(i), std::to_string(i));
    }

}

int main(void) {
    std::cout << "Current path is " << std::filesystem::current_path()
         << std::endl;
    std::cout<<"Testing does path exist method"<<std::endl;
    doesPathExistTests();
    std::cout<<"Testing is directory method"<<std::endl;
    isDirectoryTests();
    std::cout<<"Testing trim whitespace method"<<std::endl;
    trimWhitespaceTests();
    std::cout<<"Testing does epoch backup exist" <<std::endl;
    doesEpochBackupExistTests();
    std::cout<<"Testing create parent foler if doesnt exist" <<std::endl;
    createParentFolderIfDoesntExistTests();
}
