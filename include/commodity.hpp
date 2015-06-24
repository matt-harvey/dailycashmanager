/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_commodity_hpp_9343820716880178
#define GUARD_commodity_hpp_9343820716880178

#include "dcm_database_connection.hpp"
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/id.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <memory>
#include <string>

namespace dcm
{

/**
 * Class representing commodities, where a commodity is anything of
 * value that can be counted in undifferentiated units, e.g. a particular
 * currency, units in a particular fund, shares in a particular trust,
 * and so forth. A single unique asset, such as a car, might also be
 * regarded as a commodity, of which there is only one unit in
 * existence.
 *
 * Client code must deal with sqloxx::Handle<Commodity> rather than with
 * Commodity instances directly. This is enforced via "Signature"
 * parameters in the constructors for Commodity, which make it
 * impossible for client code to call these constructors directly.
 *
 * NOTE The Commodity funcionality of DCM should for the time
 * being remain almost entirely hidden from the user - mainly because we
 * we can't currently deal with the
 * situation in which there is more than one commodity in the database.
 */
class Commodity:
    public sqloxx::PersistentObject
    <    Commodity,
        DcmDatabaseConnection
    >
{

public:

    using PersistentObject = sqloxx::PersistentObject<Commodity, DcmDatabaseConnection>;
    using IdentityMap = sqloxx::IdentityMap<Commodity>;

    /**
     * Set up tables required in the database for the persistence of
     * Commodity objects.
     */
    static void setup_tables(DcmDatabaseConnection& dbc);

    /**
     * Return the id of the Commodity with abbreviation p_abbreviation.
     */
    static sqloxx::Id id_for_abbreviation
    (   DcmDatabaseConnection& dbc,
        wxString const& p_abbreviation
    );

    /**
     * Construct a fresh Commodity, not yet persisted to the database.
     *
     * Client code cannot call this constructor, as it is solely for
     * use by IdentityMap. This is enforced by the Signature parameter.
     */
    Commodity
    (   IdentityMap& p_identity_map,
        IdentityMap::Signature const& p_signature
    );

    /**
     * Construct Commodity already persisted to database with Id of
     * p_id.
     *
     * Client code cannot call this constructor, as it is solely
     * for use by IdentityMap. This is enforced by the Signature
     * parameter.
     */
    Commodity
    (   IdentityMap& p_identity_map,    
         sqloxx::Id p_id,
        IdentityMap::Signature const& p_signature
    );

    // copy constructor is private
    
    Commodity(Commodity&&) = delete;
    Commodity& operator=(Commodity const&) = delete;
    Commodity& operator=(Commodity&&) = delete;
    ~Commodity();

    /**
     * @returns \c true if and only if \c p_abbreviation is the abbreviation
     * of a Commodity stored in the database.
     */
    static bool exists_with_abbreviation
    (   DcmDatabaseConnection& p_database_connection,
        wxString const& p_abbreviation
    );

    /**
     * @returns \c true if and only if \c p_name is the name of a Commodity
     * stored in the database.
     */
    static bool exists_with_name
    (   DcmDatabaseConnection& p_database_connection,
        wxString const& p_name
    );

    /**
     * @returns the abbreviation of the commodity. E.g "AUD" might be
     * the abbreviation for Australian dollars.
     */
    wxString abbreviation();

    /**
     * @returns the full name of the commodity. E.g. "Australian dollars".
     */
    wxString name();

    /**
     * @returns the description of the commodity, e.g. "notes and coins".
     */
    wxString description();

    /**
     * @returns the number of decimal places of precision for the commodity
     * to which quantities of the commodity are stored.
     */
    jewel::Decimal::places_type precision();

    /**
     * @returns the multiplier by which 1 unit of the commodity should
     * be mulitiplied, to be translated into 1 unit of the base commodity
     * of the entity.
     */
    jewel::Decimal multiplier_to_base();

    /**
     * Set the abbreviation of the Commodity to \e p_abbreviation.
     */
    void set_abbreviation(wxString const& p_abbreviation);

    /**
     * Set the name of the Commodity to \e p_name.
     */
    void set_name(wxString const& p_name);

    /**
     * Set the description of the Commodity to \e p_description.
     */
    void set_description(wxString const& p_description);

    /**
     * Set the precision of the Commodity (i.e. the number of decimal places
     * to the right of the decimal point, such that quantities of the
     * Commodity will be represented and stored with that degree of precision)
     * to \e p_precision.
     */
    void set_precision(jewel::Decimal::places_type p_precision);

    /**
     * Set the amount by which a quantity of the Commodity must be multiplied in
     * order to convert it to an equivalent quantity of the base (default)
     * Commodity. (The default Commodity is the Commodity such that a
     * call to DcmDatabaseConnection::default_commodity() returns a
     * sqloxx::Handle to that Commodity.)
     */
    void set_multiplier_to_base(jewel::Decimal const& p_multiplier_to_base);

    /**
     * @returns the name of the "exclusive" database table for the Commodity
     * class, i.e. the table holding all and only the primary keys for saved
     * Commodity instances.
     *
     * @note This is tied to the Sqloxx API which requires std::string here,
     * not wxString.
     */
    static std::string exclusive_table_name();

    /**
     * @returns the name of the primary key for Commodity instances as stored
     * in the database.
     *
     * @note This is tied to the Sqloxx API, which requires std::string here,
     * not wxString.
     */
    static std::string primary_key_name();

private:

    void swap(Commodity& rhs);

    /**
     * Copy constructor - implemented, but deliberately private.
     */
    Commodity(Commodity const& rhs);

    // Defining pure virtual functions inherited from PersistentObject
    void do_load() override;
    void do_save_existing() override;
    void do_save_new() override;
    void do_ghostify() override;

    // Other functions
    void process_saving_statement(sqloxx::SQLStatement& statement);

    struct CommodityData;
    std::unique_ptr<CommodityData> m_data;
};

}  // namespace dcm

#endif  // GUARD_commodity_hpp_9343820716880178
