/*!
 * @copyright ryan jennings (ryan-jennings.net), 2013 under LGPL
 */
#include "db.test.h"
#include <bandit/bandit.h>
#include "record.h"
#include "select_query.h"
#include "spec_lib.h"
#include "sqldb.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

namespace rj
{
    namespace db
    {
        namespace test
        {
            std::shared_ptr<rj::db::session> current_session;

            namespace spec
            {
                void load()
                {
#include "spec_lib.h"
                }
            }

            void setup_current_session()
            {
                auto session = dynamic_pointer_cast<test::session>(current_session->impl());

                if (session) {
                    session->setup();
                }
            }

            void teardown_current_session()
            {
                auto session = dynamic_pointer_cast<test::session>(current_session->impl());

                if (session) {
                    session->teardown();
                }

                current_session->clear_schema("users");
                current_session->clear_schema("user_settings");
            }
        }
    }
}
