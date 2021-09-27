#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <map>
#include <vector>

// Gets the total amount of items from train.txt
double getTotal(std::map<std::string,int> & occur){
    double sum = 0;
    for(std::map<std::string,int>::iterator it = occur.begin(); it != occur.end(); ++it){
        sum += double(it->second);
    }
    return sum;
}

// Calculates numerator for bayes theorem
double getNum(std::string item, std::set<std::string> & classifyInfo, std::map<std::string,int> & occur, std::map<std::string,std::map<std::string,int> > & info){
    //P(MUSHY|BANANA) * P(BROWN|BANANA) .... * P(BANANA)
    double items = occur.find(item)->second;
    double total = getTotal(occur);
    double conProb = 1;
    double nonConProb = items / total;

    std::set<std::string>::iterator it;
    std::map<std::string,std::map<std::string,int> >::iterator it2;
    std::map<std::string,int>::iterator it3;

    for(it = classifyInfo.begin(); it != classifyInfo.end(); ++it){
        it2 = info.find(item);
        if(it2 != info.end()){
            it3 = it2->second.find(*it);
            if(it3 != it2->second.end()){
                double tempNum = it3->second + 1;
                conProb *= (tempNum/(items+1));
            }
            else{
                double tempNum = 1;
                conProb *= (tempNum/(items+1));
            }
        }
    }
    return conProb * nonConProb;
}

// Calculates denominator for bayes theorem
double getDen(std::string item, std::set<std::string> & classifyInfo, std::map<std::string,int> & occur, std::map<std::string,std::map<std::string,int> > & info){
    //P(mushy|not banana) * p(brown|not banana) * p(not banana)
    double total = getTotal(occur);
    double notitems = (total - occur.find(item)->second);
    double nonConProb = notitems / total;
    double conProb = 1;
    double adjs = 0;
    for(std::set<std::string>::iterator it = classifyInfo.begin(); it != classifyInfo.end(); ++it){
        for(std::map<std::string,std::map<std::string,int> >::iterator it2 = info.begin(); it2 != info.end(); ++it2){
            if(it2->first != item){
                std::map<std::string,int>::iterator it3 = it2->second.find(*it);
                if(it3 != it2->second.end()){
                    adjs = adjs + it3->second;
                }
            }
        }
        conProb *= ((adjs+1)/(notitems+1));
    }
    return conProb * nonConProb;
}

// Calculates the probability with bayes theorem
double getBayes(std::string item, std::set<std::string> & classifyInfoString, std::map<std::string,int> & occur, std::map<std::string,std::map<std::string,int> > & info){
    double num = getNum(item,classifyInfoString,occur,info);
    double den = num + getDen(item,classifyInfoString,occur,info);
    return num / den;
}

// Creates a map with the probability of each item from classify.txt 
std::map<std::string,std::vector<double> > classify(std::map<std::string,int> & occur, std::map<std::string,std::map<std::string,int> > & info, std::map<int,std::set<std::string> > & classifyInfo){
    std::map<std::string,std::vector<double> > classified;
    std::vector<double> prob;
    std::map<std::string,int>::iterator it;
    std::map<int,std::set<std::string> >::iterator it2;

    // Creates the map<string,vector<double>> that will find the probability from classify.txt
    for(it = occur.begin(); it != occur.end(); ++it){
        for(it2 = classifyInfo.begin(); it2 != classifyInfo.end(); ++it2){  // Create vector<double>
            double probability = getBayes(it->first,it2->second,occur,info);
            prob.push_back(probability);
        }
        classified.insert(std::pair<std::string,std::vector<double> >(it->first,prob));
        prob.clear();
    }
    return classified;
} 


int main(int argc, char * argv[]){
    if(argc < 4){
        std::cerr << "Error, wrong amount of inputs. Try again!" << std::endl;
        return 1;
    }

    std::map<std::string,int> occur;  // Map with items and its amount of occurances from train.txt
    std::map<std::string,std::map<std::string,int> > info;  // Map with items and its adjectives and its amount of occurances from train.txt

    //Creates a in file stream for train.txt and opens the file
    std::ifstream train_file;
    train_file.open(argv[1]);
    std::string train_line;
    std::string delimter = " \t\r\n\v\f,";
    bool begin_number = true;
    int i;

    //Parses the train.txt and appends it to their respective dictionaries
    while (getline(train_file, train_line) ) {
        //This is used to skip the first number
        if (begin_number == true){
            begin_number = false;
            continue;
        }
        std::string temp;
        std::string item;
        std::string adj;
        //Parses each line and breaks it up
        std::stringstream parsed_line(train_line);
        i = 0;
        while (parsed_line >> temp){
            bool found = false;
            //This int i n is to grab the item from each line
            if (i == 0){
                item = temp;
                //If the item is not currently within the occur dictionary, add it in
                if (occur.find(item) == occur.end()){
                    occur[item] = 1;
                    found = true;
                }
                else{
                    occur[item] += 1;
                }
                i += 1;
            //This then grabs all of the adjectives and puts them in their respective dictionary     
            }
            else{
                adj = temp;
                
                if ( info[item].find(adj) == info[item].end() ){
                    info[item][adj] = 1;
                }
                else{
                    info[item][adj] += 1;
                }
            }
        }
    }
    train_file.close();

    std::map<int,std::set<std::string> > classifyInfo;  // Store classifed cases from classify.txt
    int n = 1;

    std::fstream classify_file;
    classify_file.open(argv[2]);
    int total;
    classify_file >> total;
    // Parses classify.txt 
    std::string buffer, line;
    getline(classify_file, buffer);
    while(getline(classify_file,line)){
        std::set<std::string> classSet;
        std::stringstream temp(line);
        std::string adj;
        while(temp >> adj) {
            classSet.insert(adj);
        }
        classifyInfo.insert(std::pair<int,std::set<std::string> >(n,classSet));
        n++;
    }
    classify_file.close();
    
    std::map<std::string,std::vector<double> > solved = classify(occur,info,classifyInfo);  // Map with probability of classifications
    std::vector<double> tempProb; // Largest probability value
    std::vector<std::string> tempSolved; // Classification with the largest probability value
    tempProb = solved.begin()->second; // Copies the probability value from the first classification in the map
    
    // Inserts first classification
    for(int i = 0; i < total; i++){
        tempSolved.push_back(solved.begin()->first);
    }
    // Goes through classifications & updates the value to the item with the highest probability
    std::map<std::string,std::vector<double> >::iterator it;
    for(unsigned int i = 0; i < tempProb.size(); i++){
        for(it = solved.begin(); it != solved.end(); ++it){
            if(it != solved.begin()){
                if(it->second[i] > tempProb[i]){
                    tempProb[i] = it->second[i];
                    tempSolved[i] = it->first;
                }
                else{
                    continue;
                }
            }
        }
    }

    std::ofstream ofile;
    ofile.open(argv[3]);
    // Print answers
    for(unsigned int j = 0; j < tempSolved.size(); j++){
        if(j == tempSolved.size()-1){
            ofile << tempSolved[j];
        }
        else{
            ofile << tempSolved[j] << '\n';
        }
    }
    ofile.close();

    return 0;
}
