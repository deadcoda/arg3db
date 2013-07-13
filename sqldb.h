/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_SQLDB_H_
#define _ARG3_DB_SQLDB_H_

#include <string>
#include <sstream>
#include <sqlite3.h>
#include <type_traits>
#include "sql_value.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        class resultset;

        // an instanceof a database
        class sqldb
        {
            friend class base_query;
        protected:
            sqlite3 *db_;
        private:
            string filename_;
        public:
            sqldb(const string &name = "arg3.db");
            sqldb(const sqldb &other);
            sqldb(sqldb &&other);
            sqldb &operator=(const sqldb &other);
            sqldb &operator=(sqldb &&other);
            virtual ~sqldb();

            bool isOpen() const;

            void open();
            void close();

            sqlite3_int64 lastInsertId() const;

            int lastNumberOfChanges() const;

            string filename() const;
            void setFilename(const string &value);

            resultset execute(const string &sql);

            string lastError() const;
        };

    }
}

#endif
