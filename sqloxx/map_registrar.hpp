#ifndef GUARD_map_registrar_hpp
#define GUARD_map_registrar_hpp

namespace sqloxx
{


/**
 * T is the subclass of PersistentObject<T>, and Connnection is a subclass
 * of DatabaseConnection.
 * 
 * For types T that client code at the business layer does not want to manage
 * through an IdentityMap, MapRegistrar should be specialized to have
 * a do-nothing method register_id. By default, it is assumed client code
 * will want to use the IdentityMap pattern.
 * WARNING There may be other things that client code needs to do to avoid
 * the IdentityMap pattern for a given T. These should be determined and
 * documented.
 */
template <typename T, typename Connection>
class MapRegistrar
{
public:
	/**
	 * Lets DatabaseConnection know that a newly created instance
	 * of T, with proxy key of \e proxy_key, has just been saved to the
	 * database, and been allocated an id of \e allocated_id.
	 */
	static void notify_id
	(	Connection& dbc,
		typename T::Id proxy_key,
		typename T::Id allocated_id
	)
	{
		// Client code should fully specialize the
		// identity_map<T>(Connection&) function template for any T whose
		// identity it
		// wants to have managed via IdentityMap<T>.
		// The specialization should return a reference to the instance
		// of IdentityMap<T> through which an arbitrary instance of Connection
		// wants
		// to manage instances of T.
		// WARNING Why can't this identity_map be a method of Connection? That
		// would
		// simplify things at the client level.

		identity_map<T>(dbc).template register_id(proxy_key, allocated_id);
		return;
	}

	/**
	 * Lets DatabaseConnection know that an instance of T no longer has any
	 * handles pointing to it.
	 */
	static void notify_nil_handles(Connection& dbc, typename T::Id proxy_key)
	{
		identity_map<T>(dbc).template notify_nil_handles(proxy_key);
		return;
	}
};



}  // namespace sqloxx

#endif  // GUARD_map_registrar_hpp
