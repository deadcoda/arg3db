
#include "where_clause.h"
#include <sstream>

using namespace std;

namespace rj
{
    namespace db
    {
        sql_operator::sql_operator() : not_(false), rvalue_()
        {
        }

        sql_operator::sql_operator(const sql_operator &other)
        {
            copy(other);
        }

        sql_operator::sql_operator(sql_operator &&other)
        {
            move(std::move(other));
        }

        sql_operator &sql_operator::operator=(const sql_operator &other)
        {
            copy(other);
            return *this;
        }

        sql_operator &sql_operator::operator=(sql_operator &&other)
        {
            move(std::move(other));
            return *this;
        }

        sql_operator::~sql_operator()
        {
            switch (type_) {
                case op::EQ:
                case op::LIKE:
                case op::IS:
                    rvalue_.~sql_value();
                    break;
                case op::IN:
                    rvalues_.~vector<sql_value>();
                    break;
                case op::BETWEEN:
                    rrange_.~pair<sql_value, sql_value>();
                    break;
            }
        }

        void sql_operator::copy(const sql_operator &other)
        {
            not_ = other.not_;
            lvalue_ = other.lvalue_;
            type_ = other.type_;
            switch (type_) {
                case op::EQ:
                case op::LIKE:
                case op::IS:
                    new (&rvalue_) sql_value(other.rvalue_);
                    break;
                case op::IN:
                    new (&rvalues_) std::vector<sql_value>(other.rvalues_);
                    break;
                case op::BETWEEN:
                    new (&rrange_) std::pair<sql_value, sql_value>(other.rrange_);
                    break;
            }
        }

        void sql_operator::move(sql_operator &&other)
        {
            not_ = std::move(other.not_);
            lvalue_ = std::move(other.lvalue_);
            type_ = std::move(other.type_);
            switch (type_) {
                case op::EQ:
                case op::LIKE:
                case op::IS:
                    new (&rvalue_) sql_value(std::move(other.rvalue_));
                    break;
                case op::IN:
                    new (&rvalues_) std::vector<sql_value>(std::move(other.rvalues_));
                    break;
                case op::BETWEEN:
                    new (&rrange_) std::pair<sql_value, sql_value>(std::move(other.rrange_));
                    break;
            }
        }

        sql_operator &sql_operator::operator!()
        {
            not_ = !not_;
            return *this;
        }

        op::type sql_operator::type() const
        {
            return type_;
        }

        sql_value sql_operator::lvalue() const
        {
            return lvalue_;
        }

        namespace op
        {
            sql_operator equals(const sql_value &lvalue, const sql_value &rvalue)
            {
                sql_operator op;
                op.lvalue_ = lvalue;
                op.rvalue_ = rvalue;
                op.type_ = op::EQ;
                return op;
            }
            sql_operator like(const sql_value &lvalue, const std::string &rvalue)
            {
                sql_operator op;
                op.lvalue_ = lvalue;
                op.rvalue_ = rvalue;
                op.type_ = op::LIKE;
                return op;
            }
            sql_operator startswith(const sql_value &lvalue, const std::string &rvalue)
            {
                sql_operator op;
                op.lvalue_ = lvalue;
                op.rvalue_ = rvalue + "%";
                op.type_ = op::LIKE;
                return op;
            }
            sql_operator endswith(const sql_value &lvalue, const std::string &rvalue)
            {
                sql_operator op;
                op.lvalue_ = lvalue;
                op.rvalue_ = "%" + rvalue;
                op.type_ = op::LIKE;
                return op;
            }
            sql_operator contains(const sql_value &lvalue, const std::string &rvalue)
            {
                sql_operator op;
                op.lvalue_ = lvalue;
                op.rvalue_ = "%" + rvalue + "%";
                op.type_ = op::LIKE;
                return op;
            }
            sql_operator in(const sql_value &lvalue, const std::vector<sql_value> &rvalue)
            {
                sql_operator op;
                op.lvalue_ = lvalue;
                op.rvalues_ = rvalue;
                op.type_ = op::IN;
                return op;
            }
            sql_operator between(const sql_value &lvalue, const sql_value rvalue1, const sql_value rvalue2)
            {
                sql_operator op;
                op.lvalue_ = lvalue;
                op.rrange_ = {rvalue1, rvalue2};
                op.type_ = op::BETWEEN;
                return op;
            }
            sql_operator is(const sql_value &lvalue, const sql_null_type &rvalue)
            {
                sql_operator op;
                op.lvalue_ = lvalue;
                op.type_ = op::IS;
                return op;
            }
        }
        sql_operator_builder::sql_operator_builder(const sql_value &lvalue)
        {
            this->lvalue_ = lvalue;
        }
        sql_operator_builder::sql_operator_builder(const sql_operator_builder &other) : sql_operator(other)
        {
        }

        sql_operator_builder::sql_operator_builder(sql_operator_builder &&other) : sql_operator(std::move(other))
        {
        }

        sql_operator_builder &sql_operator_builder::operator=(const sql_operator_builder &other)
        {
            sql_operator::operator=(other);
            return *this;
        }

        sql_operator_builder &sql_operator_builder::operator=(sql_operator_builder &&other)
        {
            sql_operator::operator=(std::move(other));
            return *this;
        }

        sql_operator_builder::~sql_operator_builder()
        {
        }

        // equals
        sql_operator_builder &sql_operator_builder::operator=(const sql_value &rvalue)
        {
            this->type_ = op::EQ;
            this->rvalue_ = rvalue;
            return *this;
        }

        // nequals
        sql_operator_builder &sql_operator_builder::operator!=(const sql_value &rvalue)
        {
            this->type_ = op::EQ;
            this->not_ = true;
            this->rvalue_ = rvalue;
            return *this;
        }
        // like
        sql_operator_builder &sql_operator_builder::operator^=(const std::string &rvalue)
        {
            this->type_ = op::LIKE;
            this->rvalue_ = rvalue;
            return *this;
        }
        // starts with
        sql_operator_builder &sql_operator_builder::operator<=(const std::string &rvalue)
        {
            this->type_ = op::LIKE;
            this->rvalue_ = rvalue + "%";
            return *this;
        }
        // ends with
        sql_operator_builder &sql_operator_builder::operator>=(const std::string &rvalue)
        {
            this->type_ = op::LIKE;
            this->rvalue_ = "%" + rvalue;
            return *this;
        }
        // contains
        sql_operator_builder &sql_operator_builder::operator[](const std::string &rvalue)
        {
            this->type_ = op::LIKE;
            this->rvalue_ = "%" + rvalue + "%";
            return *this;
        }
        // in
        sql_operator_builder &sql_operator_builder::operator[](const std::vector<sql_value> &values)
        {
            this->type_ = op::IN;
            this->rvalues_ = values;
            return *this;
        }
        // between
        sql_operator_builder &sql_operator_builder::operator[](const std::pair<sql_value, sql_value> &values)
        {
            this->type_ = op::BETWEEN;
            this->rrange_ = values;
            return *this;
        }
        // is
        sql_operator_builder &sql_operator_builder::operator=(const sql_null_type &rvalue)
        {
            this->type_ = op::IS;
            this->rvalue_ = rvalue;
            return *this;
        }
        // isnot
        sql_operator_builder &sql_operator_builder::operator!=(const sql_null_type &rvalue)
        {
            this->type_ = op::IS;
            this->not_ = true;
            this->rvalue_ = rvalue;
            return *this;
        }

        sql_operator_builder operator"" _op(const char *lvalue, size_t len)
        {
            return sql_operator_builder(sql_value(lvalue, len));
        }

        where_clause::where_clause()
        {
        }

        where_clause::where_clause(const string &value) : value_(value)
        {
        }

        where_clause::where_clause(const where_clause &other) : value_(other.value_), and_(other.and_), or_(other.or_)
        {
        }

        where_clause::where_clause(where_clause &&other)
            : value_(std::move(other.value_)), and_(std::move(other.and_)), or_(std::move(other.or_))
        {
        }

        where_clause::~where_clause()
        {
        }

        where_clause &where_clause::operator=(const where_clause &other)
        {
            value_ = other.value_;
            and_ = other.and_;
            or_ = other.or_;

            return *this;
        }

        where_clause &where_clause::operator=(where_clause &&other)
        {
            value_ = std::move(other.value_);
            and_ = std::move(other.and_);
            or_ = std::move(other.or_);

            return *this;
        }

        std::string where_clause::generate_sql() const
        {
            std::string output;

            if (!and_.empty() && !or_.empty()) {
                output += "(";
            }

            output += value_;

            if (!and_.empty()) {
                std::vector<where_clause>::const_iterator it;
                output += " AND ";
                for (it = and_.begin(); it < and_.end() - 1; ++it) {
                    output += it->to_sql();
                    output += " AND ";
                }
                if (it != and_.end()) {
                    output += it->to_sql();
                }
                if (!or_.empty()) {
                    output += ")";
                }
            }

            if (!or_.empty()) {
                std::vector<where_clause>::const_iterator it;
                if (!and_.empty()) {
                    output += " OR (";
                } else {
                    output += " OR ";
                }
                for (it = or_.begin(); it < or_.end() - 1; ++it) {
                    output += it->to_sql();
                    output += " OR ";
                }
                if (it != or_.end()) {
                    output += it->to_sql();
                }
                if (!and_.empty()) {
                    output += ")";
                }
            }
            return output;
        }

        bool where_clause::empty() const
        {
            return value_.empty() && and_.empty() && or_.empty();
        }

        where_clause::operator string() const
        {
            return to_sql();
        }
        where_clause::operator string()
        {
            return to_sql();
        }

        void where_clause::set_modified()
        {
            sql_generator::reset();
        }

        where_clause &where_clause::operator&&(const where_clause &value)
        {
            if (value_.empty()) {
                value_ = value.to_sql();
            } else {
                and_.push_back(value);
            }
            set_modified();
            return *this;
        }
        where_clause &where_clause::operator&&(const string &value)
        {
            if (value.empty()) {
                return *this;
            }

            if (value_.empty()) {
                value_ = value;
            } else {
                and_.push_back(where_clause(value));
            }
            set_modified();

            return *this;
        }
        where_clause &where_clause::operator||(const where_clause &value)
        {
            if (value.empty()) {
                return *this;
            }
            if (value_.empty()) {
                value_ = value.to_sql();
            } else {
                or_.push_back(value);
            }
            set_modified();
            return *this;
        }
        where_clause &where_clause::operator||(const string &value)
        {
            if (value.empty()) {
                return *this;
            }
            if (value_.empty()) {
                value_ = value;
            } else {
                or_.push_back(where_clause(value));
            }
            set_modified();
            return *this;
        }

        void where_clause::reset()
        {
            value_.clear();
            and_.clear();
            or_.clear();
            sql_generator::reset();
        }

        void where_clause::reset(const std::string &value)
        {
            reset();
            value_ = value;
        }
        void where_clause::reset(const where_clause &other)
        {
            operator=(other);
        }

        ostream &operator<<(ostream &out, const where_clause &where)
        {
            out << where.to_sql();
            return out;
        }


        where_builder::where_builder(const std::shared_ptr<session_impl> &session, bindable *binder)
            : session_(session), binder_(binder)
        {
            assert(binder != nullptr);
        }

        where_builder::where_builder(const where_builder &other)
            : where_clause(other), session_(other.session_), binder_(other.binder_)
        {
        }

        where_builder::where_builder(where_builder &&other)
            : where_clause(std::move(other)), session_(std::move(other.session_)), binder_(other.binder_)
        {
        }

        where_builder &where_builder::operator=(const where_builder &other)
        {
            where_clause::operator=(other);
            session_ = other.session_;
            binder_ = other.binder_;
            return *this;
        }

        where_builder &where_builder::operator=(where_builder &&other)
        {
            where_clause::operator=(std::move(other));
            session_ = std::move(other.session_);
            binder_ = other.binder_;
            return *this;
        }

        where_builder::~where_builder()
        {
        }

        where_builder &where_builder::bind(size_t index, const sql_operator &value)
        {
            switch (value.type_) {
                case op::EQ:
                case op::LIKE:
                case op::IS:
                    binder_->bind(index, value.rvalue_);
                    break;
                case op::IN:
                    for (size_t i = 0; i < value.rvalues_.size(); i++) {
                        binder_->bind(index + i, value.rvalues_[i]);
                    }
                    break;
                case op::BETWEEN:
                    binder_->bind(index, value.rrange_.first);
                    binder_->bind(index, value.rrange_.second);
                    break;
            }
            return *this;
        }

        std::string where_builder::to_sql(size_t index, const sql_operator &value)
        {
            std::string result = value.lvalue_.to_string() + std::string(" ");

            if (value.not_) {
                result += op::not_type_values[value.type_];
            } else {
                result += op::type_values[value.type_];
            }

            result += std::string(" ");
            result += session_->bind_param(index);
            return result;
        }

        where_builder &where_builder::bind(size_t index, const sql_value &value)
        {
            binder_->bind(index, value);
            return *this;
        }
        where_builder &where_builder::bind(const std::string &name, const sql_value &value)
        {
            binder_->bind(name, value);
            return *this;
        }

        void where_builder::reset(const sql_operator &value)
        {
            size_t index = binder_->num_of_bindings() + 1;
            where_clause::reset(to_sql(index, value));
            bind(index, value);
        }

        size_t where_builder::num_of_bindings() const
        {
            return binder_->num_of_bindings();
        }

        /*!
         * Appends and AND part to this where clause
         * @param value   the sql to append
         */
        where_builder &where_builder::operator&&(const sql_operator &value)
        {
            size_t index = binder_->num_of_bindings() + 1;
            where_clause::operator&&(to_sql(index, value));
            return bind(index, value);
        }

        /*!
         * Appends and OR part to this where clause
         * @param value   the sql to append
         */
        where_builder &where_builder::operator||(const sql_operator &value)
        {
            size_t index = binder_->num_of_bindings() + 1;
            where_clause::operator&&(to_sql(index, value));
            return bind(index, value);
        }
    }
}
