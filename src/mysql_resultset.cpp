#include "config.h"


#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_resultset.h"
#include "mysql_db.h"
#include "mysql_row.h"
#include "exception.h"
#include "mysql_binding.h"

namespace arg3
{
    namespace db
    {

        extern string last_stmt_error(MYSQL_STMT *stmt);

        mysql_resultset::mysql_resultset(mysql_db *db, MYSQL_RES *res) : res_(res), row_(NULL), db_(db)
        {
            //assert(res_ != NULL);
        }

        mysql_resultset::mysql_resultset(mysql_resultset &&other) : res_(other.res_), row_(other.row_), db_(other.db_)
        {
            other.db_ = NULL;
            other.res_ = NULL;
            other.row_ = NULL;
        }

        mysql_resultset::~mysql_resultset()
        {
            if (res_ != NULL)
            {
                mysql_free_result(res_);
                res_ = NULL;
            }
        }

        mysql_resultset &mysql_resultset::operator=(mysql_resultset && other)
        {
            res_ = other.res_;
            db_ = other.db_;
            row_ = other.row_;
            other.db_ = NULL;
            other.res_ = NULL;
            other.row_ = NULL;

            return *this;
        }

        bool mysql_resultset::is_valid() const
        {
            return res_ != NULL;
        }

        bool mysql_resultset::next()
        {
            assert(db_ != NULL);

            if (res_ == NULL || !db_->is_open())
                return false;

            bool value = (row_ = mysql_fetch_row(res_)) != NULL;

            if (!value && !mysql_more_results(db_->db_))
            {
                mysql_free_result(res_);
                if ((res_ = mysql_use_result(db_->db_)) != NULL)
                    value = (row_ = mysql_fetch_row(res_)) != NULL;
            }

            return value;
        }

        void mysql_resultset::reset()
        {
            mysql_data_seek(res_, 0);
        }

        row mysql_resultset::current_row()
        {
            return row(make_shared<mysql_row>(db_, res_, row_));
        }

        size_t mysql_resultset::column_count() const
        {
            return mysql_num_fields(res_);
        }


        /* Statement version */

        mysql_stmt_resultset::mysql_stmt_resultset(mysql_db *db, MYSQL_STMT *stmt) : stmt_(stmt), metadata_(NULL), db_(db),
            bindings_(nullptr), columnCount_(0), status_(-1)
        {
            assert(stmt_ != NULL);
            assert(db_ != NULL);
        }

        mysql_stmt_resultset::mysql_stmt_resultset(mysql_stmt_resultset &&other) : stmt_(other.stmt_), metadata_(other.metadata_),
            db_(other.db_),
            bindings_(other.bindings_), columnCount_(other.columnCount_), status_(other.status_)
        {
            other.db_ = NULL;
            other.stmt_ = NULL;
            other.bindings_ = nullptr;
            other.metadata_ = NULL;
        }

        mysql_stmt_resultset::~mysql_stmt_resultset()
        {
            if (metadata_)
            {
                mysql_free_result(metadata_);
                metadata_ = NULL;
            }
        }

        mysql_stmt_resultset &mysql_stmt_resultset::operator=(mysql_stmt_resultset && other)
        {
            stmt_ = other.stmt_;
            db_ = other.db_;
            metadata_ = other.metadata_;
            bindings_ = other.bindings_;
            columnCount_ = other.columnCount_;
            status_ = other.status_;
            other.db_ = NULL;
            other.bindings_ = NULL;
            other.metadata_ = NULL;

            return *this;
        }

        void mysql_stmt_resultset::prepare_results()
        {
            if (stmt_ == NULL || bindings_ != NULL || status_ != -1)
                return;

            if ((status_ = mysql_stmt_execute(stmt_)))
            {
                throw database_exception(last_stmt_error(stmt_));
            }

            // get information about the results
            metadata_ = mysql_stmt_result_metadata(stmt_);

            if (metadata_ == NULL)
                throw database_exception("No result data found.");

            columnCount_ = mysql_num_fields(metadata_);

            auto fields = mysql_fetch_fields(metadata_);

            bindings_ = make_shared<mysql_binding>(fields, columnCount_);

            bindings_->bind_result(stmt_);
        }

        bool mysql_stmt_resultset::is_valid() const
        {
            return stmt_ != NULL;
        }

        bool mysql_stmt_resultset::next()
        {
            if (stmt_ == NULL)
                return false;

            if (status_ == -1)
            {
                prepare_results();

                if (status_ == -1)
                    return false;
            }

            int res = mysql_stmt_fetch(stmt_);

            if (res == 1 || res == MYSQL_DATA_TRUNCATED)
                throw database_exception(last_stmt_error(stmt_));

            if (res == MYSQL_NO_DATA)
            {
                return false;
            }

            return true;

        }

        void mysql_stmt_resultset::reset()
        {
            assert(stmt_ != NULL);

            if (mysql_stmt_reset(stmt_))
                throw database_exception(last_stmt_error(stmt_));

            status_ = -1;
        }

        row mysql_stmt_resultset::current_row()
        {
            assert(db_ != NULL);
            assert(metadata_ != NULL);
            assert(bindings_ != nullptr);

            return row(make_shared<mysql_stmt_row>(db_, metadata_, bindings_ ));
        }

        size_t mysql_stmt_resultset::column_count() const
        {
            assert(stmt_ != NULL);
            return mysql_stmt_field_count(stmt_);
        }

    }
}

#endif
