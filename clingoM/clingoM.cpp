#define __STDC_FORMAT_MACROS
#include "clingoM.h"
#include "clingo/libclingo/clingo.hh"

#include "core/vector.h"
#include "core/array.h"
#include "core/map.h"
#include <core/variant.h>
#include <core/dictionary.h>

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

int argCount(String args){ 
	int count = 0;

	for (int i = 0; i < args.size(); i++)
		if (args[i] == ' ') count++;

	return count+1;
}

char** argValue(String args, int argc){ 
	std::wstring ws = args.c_str();
	std::string s( ws.begin(), ws.end() );
	char* originalStr = strdup(s.c_str());
	char** argv = (char**)malloc(argc*sizeof(char*));
	char* token;
	char* delimiter = " "; 
	token = strtok(originalStr, delimiter);
	int i=0;
	while(token != NULL){ 
		argv[i] = strdup(token);
		i++;
		token = strtok(NULL, delimiter);
	}
	return argv;
}

void ClingoM::reset(String args){
	int argc = argCount(args);
	char** argv = argValue(args, argc);
	generateControl(argc, argv);
}

//Es necesario hacer trim de args

void ClingoM::cleanup(){ 
	this->ctl->cleanup();
}

void ClingoM::load(String filename){ 
	std::wstring ws = filename.c_str();
	std::string filename2( ws.begin(), ws.end() ); 
	this->ctl->load(filename2.c_str());
}

void ClingoM::ground(String programName, Array arguments){ 
	std::wstring ws = programName.c_str();
	std::string programName2( ws.begin(), ws.end() ); 
	Clingo::Symbol* params=(Clingo::Symbol*)malloc(arguments.size()*sizeof(Clingo::Symbol));
	for(int i=0; i<arguments.size(); i++){ 
		std::wstring ws2 = programName.c_str();
		std::string str2( ws.begin(), ws.end() ); 
		params[i] = Clingo::String(str2.c_str());
	}
	if(arguments.size()>0)
		this->ctl->ground({{programName2.c_str(), {params, arguments.size()}}});
	else
		this->ctl->ground({{programName2.c_str(), {}}});
} 

void ClingoM::_bind_methods(){
	ClassDB::bind_method(D_METHOD("reset", "args"), &ClingoM::reset);
	ClassDB::bind_method(D_METHOD("cleanup"), &ClingoM::cleanup);
	ClassDB::bind_method(D_METHOD("load", "filename"), &ClingoM::load);
	ClassDB::bind_method(D_METHOD("ground", "programName"), &ClingoM::ground);
	ClassDB::bind_method(D_METHOD("solve"), &ClingoM::solve);
	ClassDB::bind_method(D_METHOD("setArg", "option"), &ClingoM::setArg);
}

Dictionary ClingoM::solve(){ 
	Array pred;
	Map<String, Array> gMap;
	Dictionary dictionary;
	String aux;
    try {
        for (auto &m : ctl->solve()) {
            for (auto &atom : m.symbols()) {
                //std::cout << " " << atom.name();
				//std::cout << " " << atom.arguments().size();
				//std::cout << " " << atom.arguments()[0];
				aux = atom.name();
				if(!gMap.has(aux))
					gMap.insert(aux, {});
				pred = {};
				for(size_t i=0; i<atom.arguments().size(); i++)
					pred.append(atom.arguments()[i].to_string().c_str());
				gMap.find(aux)->get().append(pred);
            }
			for(Map<String, Array>::Element *E=gMap.front();E;E=E->next()) {
				dictionary[E->key()] = E->value();
			}
			break;
        }
    }
    catch (std::exception const &e) {
        std::cerr << "example failed with: " << e.what() << std::endl;
    }
	return dictionary; 
}

void ClingoM::setArg(String option, String value){
	std::wstring ws = option.c_str();
	std::string optionStr( ws.begin(), ws.end() ); 
	ws = value.c_str();
	std::string valueStr( ws.begin(), ws.end() ); 
	this->ctl->configuration()[optionStr.c_str()] = valueStr.c_str();
}


void ClingoM::generateControl(int argc, char**argv){
	if(this->ctl != NULL)
		delete this->ctl;
	Clingo::Logger logger = [](Clingo::WarningCode, char const *message) {
		std::cerr << message << std::endl;
	};
	this->ctl = new Clingo::Control({argv, size_t(argc)}, logger, 20); 
}

ClingoM::ClingoM(String args){
	int argc = argCount(args);
	char** argv = argValue(args, argc);
	this->ctl = NULL;
	generateControl(argc, argv);
}

ClingoM::ClingoM(){
	int argc = 0;
	char* s="0";
	char** argv = &s; 
	this->ctl = NULL;
	generateControl(argc, argv); 
}

ClingoM::~ClingoM(){
	delete this->ctl;
}
