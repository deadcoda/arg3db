#ifndef ARG3_DB_MYSQL_SQLDB_H
#define ARG3_DB_MYSQL_SQLDB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a mysql specific implementation of a database
         */
        class mysql_db : public sqldb
        {
            friend class base_query;
            friend class mysql_statement;
            friend class mysql_resultset;
            friend class mysql_cached_resultset;

           protected:
            MYSQL *db_;

           public:
            mysql_db(const uri &connInfo);
            mysql_db(const mysql_db &other);
            mysql_db(mysql_db &&other);
            mysql_db &operator=(const mysql_db &other);
            mysql_db &operator=(mysql_db &&other);
            virtual ~mysql_db();

            bool is_open() const;
            void open();
            void close();
            void query_schema(const string &tablename, std::vector<column_definition> &columns);

            long long last_insert_id() const;
            int last_number_of_changes() const;
            string last_error() const;

            resultset execute(const string &sql, bool cache = false);

            shared_ptr<statement> create_statement();
        };
    }
}

#endif

#endif
