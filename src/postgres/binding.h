/*!
 * @file binding.h
 * utility for binding data to a postgres query
 */
#ifndef ARG3_DB_POSTGRES_BINDING_H
#define ARG3_DB_POSTGRES_BINDING_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "../sql_value.h"
#include "../bindable.h"
#include <libpq-fe.h>
#include <string>

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            namespace data_mapper
            {
                sql_value to_value(Oid type, const char *value, int len);
            }
            /*
             * utility class to simplify binding query parameters
             */
            class binding : public bindable
            {
                friend class column;
                friend class statement;

               private:
                char **values_;
                Oid *types_;
                int *lengths_;
                int *formats_;
                size_t size_;
                void copy_value(const binding &other);
                void clear_value();
                void clear_value(size_t index);
                bool reallocate_value(size_t index);

               public:
                /*!
                 * default constructor
                 */
                binding();
                /*!
                 * zero out an array of size for binding
                 */
                binding(size_t size);

                /* boilerplate */
                binding(const binding &other);
                binding(binding &&other);
                binding &operator=(const binding &other);
                binding &operator=(binding &&other);
                virtual ~binding();

                /*!
                 * @return the number of bindings
                 */
                size_t size() const;

                /*!
                 * gets the value of a binding
                 * @param  index the index of the binding parameter
                 * @return       the value of the binding at the given index
                 */
                sql_value to_value(size_t index) const;

                /*!
                 * gets the sql type of a binding
                 * @param  index the index of the binding
                 * @return       an integer representing the sql type
                 * Note: the return value will be a postgres specific implementation
                 */
                int sql_type(size_t index) const;

                /* bindable overrides */
                binding &bind(size_t index, int value);
                binding &bind(size_t index, unsigned value);
                binding &bind(size_t index, long long value);
                binding &bind(size_t index, unsigned long long value);
                binding &bind(size_t index, float value);
                binding &bind(size_t index, double value);
                binding &bind(size_t index, const std::string &value, int len = -1);
                binding &bind(size_t index, const std::wstring &value, int len = -1);
                binding &bind(size_t index, const sql_blob &value);
                binding &bind(size_t index, const sql_null_type &value);
                binding &bind(size_t index, const sql_time &value);

                /*!
                 * clear and remove all bindings
                 */
                void reset();
            };
        }
    }
}

#endif


#endif
