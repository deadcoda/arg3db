#ifndef POSTGRES_DB_H
#define POSTGRES_DB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPOSTGRESQL

#include <.h>
#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a mysql specific implementation of a database
         */
        class postgres_db : public sqldb
        {
            friend class base_query;
            friend class postgres_statement;
            friend class postgres_resultset;
            friend class postgres_cached_resultset;

           protected:
            PGconn *db_;

           public:
            postgres_db(const uri &info);
            postgres_db(const postgres_db &other);
            postgres_db(postgres_db &&other);
            postgres_db &operator=(const postgres_db &other);
            postgres_db &operator=(postgres_db &&other);
            virtual ~postgres_db();

            bool is_open() const;
            void open();
            void close();
            void query_schema(const string &tablename, std::vector<column_definition> &columns);

            long long last_insert_id() const;
            int last_number_of_changes() const;
            string last_error() const;

            resultset execute(const string &sql, bool cache = false);

            schema_factory *schemas();

            shared_ptr<statement> create_statement();
        };
    }
}


#endif
