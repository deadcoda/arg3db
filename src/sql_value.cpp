#include "sql_value.h"
#include "query.h"
#include "sqldb.h"
#include "exception.h"
#include <sstream>

namespace std
{
    string to_string(const nullptr_t &value)
    {
        return "NULL";
    }
    string to_string(const std::string &value)
    {
        return value;
    }

    ostream &operator<<(ostream &out, const nullptr_t &null)
    {
        out << "NULL";
        return out;
    }
}

namespace arg3
{
    namespace db
    {
        const nullptr_t sql_null = nullptr;


        void sql_value::bind_to(bindable *obj, int index) const
        {
            if (is_null()) {
                obj->bind(index, sql_null);
            } else if (is_numeric()) {
                if (is_real()) {
                    obj->bind(index, to_double(0));
                } else {
                    obj->bind(index, to_llong(0));
                }
            } else if (is_binary()) {
                obj->bind(index, to_binary());
            } else if (is_wstring()) {
                obj->bind(index, to_wstring());
            } else {
                obj->bind(index, to_string());
            }
        }
    }
}
