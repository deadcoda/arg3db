/*!
 * Mysql specific implementations of a row
 * @file row.h
 */
#ifndef ARG3_DB_MYSQL_ROW_H
#define ARG3_DB_MYSQL_ROW_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include <vector>
#include "../row.h"
#include "column.h"

namespace arg3
{
    namespace db
    {
        namespace mysql
        {
            class db;
            class binding;

            /*!
             *  a mysql specific implementation of a row
             */
            class row : public row_impl
            {
                friend class resultset;

               private:
                MYSQL_ROW row_;
                std::shared_ptr<MYSQL_RES> res_;
                mysql::db *db_;
                size_t size_;

               public:
                /*!
                 * @param db the database in use
                 * @param res the query result
                 * @param row the row values
                 */
                row(mysql::db *db, const std::shared_ptr<MYSQL_RES> &res, MYSQL_ROW row);

                /* non-copyable boilerplate */
                virtual ~row();
                row(const row &other) = delete;
                row(row &&other);
                row &operator=(const row &other) = delete;
                row &operator=(row &&other);

                /* row_impl overrides */
                std::string column_name(size_t position) const;
                column_type column(size_t position) const;
                column_type column(const std::string &name) const;
                size_t size() const;
                bool is_valid() const;
            };

            /*!
             *  a mysql specific implementation of a row using prepared statements
             */
            class stmt_row : public row_impl
            {
                friend class stmt_resultset;

               private:
                std::shared_ptr<mysql::binding> fields_;
                std::shared_ptr<MYSQL_RES> metadata_;
                std::shared_ptr<MYSQL_STMT> stmt_;
                mysql::db *db_;
                size_t size_;

               public:
                /*!
                 * @param db the database in use
                 * @param stmt the query statement
                 * @param metadata the query meta data
                 * @param fields the bindings for the statement
                 */
                stmt_row(mysql::db *db, const std::shared_ptr<MYSQL_STMT> &stmt, const std::shared_ptr<MYSQL_RES> &metadata,
                         const std::shared_ptr<mysql::binding> &fields);

                /* non-copyable boilerplate */
                virtual ~stmt_row();
                stmt_row(const stmt_row &other) = delete;
                stmt_row(stmt_row &&other);
                stmt_row &operator=(const stmt_row &other) = delete;
                stmt_row &operator=(stmt_row &&other);

                /* row_impl overrides */
                std::string column_name(size_t position) const;
                column_type column(size_t position) const;
                column_type column(const std::string &name) const;
                size_t size() const;
                bool is_valid() const;
            };
        }
    }
}

#endif

#endif
