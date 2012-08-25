#ifndef GUARD_commodity_storage_manager_hpp
#define GUARD_commodity_storage_manager_hpp

#include "commodity.hpp"


namespace phatbooks
{

template<Commodity>
class StorageManager
{
public:
	void save(Commodity const& commodity);
	Commodity load(Key key);
		



};

}  // namespace phatbooks
#endif  // GUARD_commodity_storage_manager_hpp
