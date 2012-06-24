#ifndef GUARD_stored_object_hpp
#define GUARD_stored_object_hpp

#include "identifier.hpp"

class StoredObject
{
public:
	void save();
	// type signature T.B.D. for method... load()
private:
	Identifier m_id;
};
	


#endif
