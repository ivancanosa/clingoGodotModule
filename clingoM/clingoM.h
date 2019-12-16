#ifndef GODOT_CLINGOM_H
#define GODOT_CLINGOM_H

#include "core/reference.h"
#include "core/variant.h"
#include "core/dictionary.h"

#include "core/vector.h"
#include "core/array.h"
#include "core/map.h"

#include "clingo/libclingo/clingo.hh" 

class ClingoM: public Reference{
	GDCLASS(ClingoM, Reference);
	Clingo::Control* ctl; 

protected:
	static void _bind_methods();

private:
	void generateControl(int argc, char** argv); 

public:
	void reset(String args);
	void cleanup();
	void load(String filename);
	void ground(String programName, Array arguments); 
	Dictionary solve();
	void setArg(String option, String value);

	ClingoM();
	ClingoM(String args);
	~ClingoM();
};

#endif
