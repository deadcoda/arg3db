#include <bandit/bandit.h>
#include "base_record.h"
#include "db.test.h"
#include "sqlite3_column.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;


column get_user_column(const string &name)
{
    select_query q(testdb, "users");

    auto rs = q.execute();

    auto row = rs.begin();

    return row->column(name);
}

go_bandit([]() {
    describe("column", []() {
        before_each([]() { setup_testdb(); });

        after_each([]() { teardown_testdb(); });
        before_each([]() {
            user u;
            u.set("first_name", "Bob");
            u.set("last_name", "Jenkins");
            u.set("dval", 123.321);

            int *data = (int *)malloc(sizeof(int));

            memset(data, 0, sizeof(int));

            *data = 4;

            u.set("data", sql_blob(data, sizeof(int)));

            u.save();

            free(data);
        });

        it("is copyable", []() {
            auto col = get_user_column("first_name");

            column other(col);

            AssertThat(other.is_valid(), IsTrue());

            AssertThat(other.to_value(), Equals(col.to_value()));
        });

        it("is movable", []() {
            auto col = get_user_column("first_name");

            auto val = col.to_value();

            column other(std::move(col));

            AssertThat(col.is_valid(), IsFalse());

            AssertThat(other.is_valid(), IsTrue());

            AssertThat(other.to_value(), Equals(val));

            column last = get_user_column("last_name");

            last = std::move(other);

            AssertThat(other.is_valid(), IsFalse());
            AssertThat(last.is_valid(), IsTrue());
            AssertThat(last.to_value(), Equals(val));
        });

        it("can be a blob", []() {
            auto col = get_user_column("data");

            AssertThat(col.to_value().is_binary(), IsTrue());

            AssertThat(col.to_value().to_binary().size(), Equals(4));
        });

        it("can be a double", []() {
            auto col = get_user_column("dval");

            AssertThat(col.to_value(), Equals(123.321));

            double val = col.to_value();

            AssertThat(val, Equals(123.321));
        });

        it("can be an int64", []() {
            auto col = get_user_column("id");

            AssertThat(col.to_value().to_llong() > 0, IsTrue());

            long long val = col.to_value();

            AssertThat(val > 0, IsTrue());
        });

    });
});
